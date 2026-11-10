/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "cellmlfile_p.h"
#include "file_p.h"

#include "utils.h"

#include "libopencor/filemanager.h"
#include "libopencor/seddocument.h"
#include "libopencor/sedmodel.h"
#include "libopencor/sedsteadystate.h"
#include "libopencor/sedtask.h"
#include "libopencor/seduniformtimecourse.h"
#include "libopencor/solvercvode.h"
#include "libopencor/solverkinsol.h"

#include <algorithm>
#include <mutex>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace libOpenCOR {

namespace {

// Cache of compiled runtimes, keyed by CellmlFile pointer.

std::mutex sRuntimesMutex; // NOLINT
std::unordered_map<const CellmlFile *, CellmlFileRuntimePtr> sRuntimes; // NOLINT

// Normalise the given path the way libCellML does it, so that we can determine the key that libCellML uses for an
// imported model in the library of its importer.

std::string normalisedDirectorySeparator(const std::string &pPath)
{
    auto res {pPath};

    std::ranges::replace(res, '\\', '/');

    return res;
}

std::string pathFromUrl(const std::string &pUrl)
{
    auto res {normalisedDirectorySeparator(pUrl)};
    auto path {res.substr(0, res.find_first_of("?#"))};

    return path.substr(0, path.find_last_of('/') + 1);
}

// Retrieve the contents of an imported file.

std::optional<std::string> importedFileContents(bool pIsLocalFile, const std::string &pFileNameOrUrl)
{
    // Use the contents of the managed file, if any, with the given file name or URL. This is the case for a file in a
    // COMBINE archive or for a file which contents were set in memory (e.g., from JavaScript).

#ifdef __EMSCRIPTEN__
    auto file {FileManager::instance().fileFromFileNameOrUrl(pFileNameOrUrl)};
#else
    auto file {FileManager::instance().file(pFileNameOrUrl)};
#endif

    if (file != nullptr) {
        const auto &contents {file->contents()};

        if (!contents.empty()) {
            return toString(contents);
        }
    }

#ifdef __EMSCRIPTEN__
    (void)pIsLocalFile;
#else
    // Retrieve the contents of the local file or download the remote file and retrieve its contents.
    // Note #1: we use the std::error_code version of std::filesystem::is_regular_file() so that nothing gets thrown
    //          (e.g., if the file name is too long), in which case we consider that the file doesn't exist.
    // Note #2: the file name cannot contain a null character (which would truncate it) since it comes from an XML
    //          attribute, which cannot contain one, and since decodeUrl() never decodes a percent-encoded null
    //          character.

    if (pIsLocalFile) {
        std::error_code errorCode;
        auto filePath {stringToPath(pFileNameOrUrl)};

        if (std::filesystem::is_regular_file(filePath, errorCode)) {
            return toString(fileContents(filePath));
        }
    } else {
        auto [res, filePath] {downloadFile(pFileNameOrUrl)};

        if (res) {
            auto contents {toString(fileContents(filePath))};
            std::error_code errorCode;

            std::filesystem::remove(filePath, errorCode);

            return contents;
        }
    }
#endif

    return std::nullopt;
}

bool hasXmlError(const libcellml::ParserPtr &pParser)
{
    // Return whether the given parser has found some XML errors.

    for (size_t i {0}; i < pParser->errorCount(); ++i) {
        if (pParser->error(i)->referenceRule() == libcellml::Issue::ReferenceRule::XML) {
            return true;
        }
    }

    return false;
}

Strings importUrls(const libcellml::ModelPtr &pModel)
{
    // Retrieve the URLs of the imports of the given model, in the order in which they are encountered.

    Strings res;
    std::unordered_set<std::string> urls;

    auto addUrl = [&](const libcellml::ImportedEntityPtr &pImportedEntity) {
        if (pImportedEntity->isImport()) {
            const auto &url {pImportedEntity->importSource()->url()};

            if (urls.insert(url).second) {
                res.push_back(url);
            }
        }
    };

    for (size_t i {0}; i < pModel->unitsCount(); ++i) {
        addUrl(pModel->units(i));
    }

    std::vector<libcellml::ComponentPtr> components;

    for (size_t i {0}; i < pModel->componentCount(); ++i) {
        components.push_back(pModel->component(i));
    }

    while (!components.empty()) {
        auto component {components.back()};

        components.pop_back();

        addUrl(component);

        for (size_t i {0}; i < component->componentCount(); ++i) {
            components.push_back(component->component(i));
        }
    }

    return res;
}

// A model that has some imports and that we need to go through to retrieve the models that it imports.

struct ImportingModel
{
    libcellml::ModelPtr model;
    std::string keyBase;
    FileLocation location;
    Strings fileNamesOrUrls;
};

using ImportedFileContentsCache = std::unordered_map<std::string, std::optional<std::string>>;

libcellml::ModelPtr retrieveImportedModel(ImportedFileContentsCache &pCache, bool pIsLocalFile,
                                          const std::string &pFileNameOrUrl, bool pStrict)
{
    // Retrieve and parse an imported model, returning a null pointer if we cannot retrieve its contents or if it is not
    // valid XML (libCellML will report it).
    // Note: the same file may be imported several times (with different keys), so we only retrieve its contents once.

    auto contents {pCache.find(pFileNameOrUrl)};

    if (contents == pCache.end()) {
        contents = pCache.emplace(pFileNameOrUrl, importedFileContents(pIsLocalFile, pFileNameOrUrl)).first;
    }

    if (!contents->second.has_value()) {
        return nullptr;
    }

    auto parser {libcellml::Parser::create(pStrict)};
    auto res {parser->parseModel(*contents->second)};

    return hasXmlError(parser) ? nullptr : res;
}

void populateImporterLibrary(const libcellml::ImporterPtr &pImporter, const libcellml::ModelPtr &pModel,
                             const FileLocation &pLocation, const std::string &pFileNameOrUrl)
{
    // Populate the library of the given importer with the models that are (directly or indirectly) imported by the
    // given model.
    // Note #1: libCellML can only resolve imports that are local files. So, we retrieve the imported models ourselves
    //          (be they local files, remote files, files in a COMBINE archive, or files which contents were set in
    //          memory) and add them to the library of its importer, which is what libCellML looks up before trying to
    //          open a file.
    // Note #2: libCellML looks up an imported model using its URL as is or, if there is no such key, using the URL
    //          prefixed with a base path. The base path for the imports of the given model is the location of its file
    //          (which is either empty or ends with a directory separator) while the base path for the imports of an
    //          imported model is the base path of its importing model followed by the path (i.e. up to the last
    //          forward slash) of its URL.
    // Note #3: we keep track of the files that lead to an imported model, so that we don't end up in an infinite loop
    //          in the case of an import cycle (which libCellML will report).
    // Note #4: we don't report any issue. If we cannot retrieve or parse an imported model then we simply don't add it
    //          to the library of the importer, and libCellML will report the issue when trying to resolve the imports.

    std::stack<ImportingModel> importingModels;
    ImportedFileContentsCache importedFileContentsCache;

    importingModels.push({pModel, normalisedDirectorySeparator(pLocation.location), pLocation, {pFileNameOrUrl}});

    while (!importingModels.empty()) {
        auto importingModel {std::move(importingModels.top())};

        importingModels.pop();

        for (const auto &url : importUrls(importingModel.model)) {
            // Determine the key that libCellML uses for the imported model and skip it if we already have it in the
            // library of the importer.

            auto normalisedUrl {normalisedDirectorySeparator(url)};
            auto key {hasScheme(url) ? normalisedUrl : importingModel.keyBase + normalisedUrl};

            if (pImporter->library(key) != nullptr) {
                continue;
            }

            // Resolve the URL against the location of the importing model, skipping it if it cannot be retrieved,
            // e.g., if it has an unsupported scheme.

            auto resolvedUrl {resolveUriReference(importingModel.location, url)};

            if (!resolvedUrl.has_value()) {
                continue;
            }

            // Skip the imported file if it is one of the files that lead to the importing model, i.e. if there is an
            // import cycle.

            auto [isLocalFile, fileNameOrUrl] {retrieveFileInfo(*resolvedUrl)};

            if (std::ranges::find(importingModel.fileNamesOrUrls, fileNameOrUrl) != importingModel.fileNamesOrUrls.end()) {
                continue;
            }

            // Retrieve the imported model, skipping it if we cannot.

            auto model {retrieveImportedModel(importedFileContentsCache, isLocalFile, fileNameOrUrl, pImporter->isStrict())};

            if (model == nullptr) {
                continue;
            }

            // Add the imported model to the library of the importer and keep track of the files that lead to it, so
            // that we can retrieve the models that it imports.

            pImporter->addModel(model, key);

            auto fileNamesOrUrls {importingModel.fileNamesOrUrls};

            fileNamesOrUrls.push_back(fileNameOrUrl);

            importingModels.push({model, importingModel.keyBase + pathFromUrl(url),
                                  fileLocation(isLocalFile, fileNameOrUrl), std::move(fileNamesOrUrls)});
        }
    }
}

} // namespace

CellmlFile::Impl::Impl(const FilePtr &pFile, const libcellml::ModelPtr &pModel, bool pStrict)
    : mFile(pFile)
    , mModel(pModel)
{
    // Resolve imports and flatten the model, if needed and possible.

    if (mModel->hasUnresolvedImports()) {
        resolveImports(pFile, pStrict);
    }

    // Analyse the model.
    // Note: we do this even if there are some errors (as a result of resolving imports). This is so that we can
    //       retrieve the analyser's issues if needed (e.g., when wanting to retrieve a runtime) and so that we don't
    //       have to test mAnalyserModel for nullity everywhere.

    mAnalyser->analyseModel(mModel);

    mAnalyserModel = mAnalyser->analyserModel();

    if (mAnalyser->errorCount() != 0) {
        addIssues(mAnalyser, "Analyser");
    }
}

void CellmlFile::Impl::resolveImports(const FilePtr &pFile, bool pStrict)
{
    // Resolve the imports of our model and flatten it, if possible.

    auto importer {libcellml::Importer::create(pStrict)};
    auto isLocalFile {pFile->url().empty()};
    const auto &fileNameOrUrl {isLocalFile ? pFile->fileName() : pFile->url()};
    auto location {fileLocation(isLocalFile, fileNameOrUrl)};

    populateImporterLibrary(importer, mModel, location, fileNameOrUrl);

    if (importer->resolveImports(mModel, location.location)) {
        mModel = importer->flattenModel(mModel);
    } else {
        addIssues(importer, "Importer");
    }
}

void CellmlFile::Impl::populateDocument(const SedDocumentPtr &pDocument) const
{
    // Add a model.

    auto model {SedModel::create(pDocument, mFile.lock())};

    pDocument->addModel(model);

    // Add a uniform time course simulation in the case of an ODE/DAE model while a steady state simulation in the case
    // of an algebraic or NLA model.

    SedSimulationPtr simulation;
    auto type {mAnalyserModel->type()};

    if ((type == libcellml::AnalyserModel::Type::ODE)
        || (type == libcellml::AnalyserModel::Type::DAE)) {
        simulation = SedUniformTimeCourse::create(pDocument);
    } else {
        simulation = SedSteadyState::create(pDocument);
    }

    pDocument->addSimulation(simulation);

    // Add the required solver(s) depending on the type of our model.

    if ((type == libcellml::AnalyserModel::Type::ODE)
        || (type == libcellml::AnalyserModel::Type::DAE)) {
        simulation->setOdeSolver(SolverCvode::create());
    }

    if ((type == libcellml::AnalyserModel::Type::NLA)
        || (type == libcellml::AnalyserModel::Type::DAE)) {
        simulation->setNlaSolver(SolverKinsol::create());
    }

    // Add a task.

    pDocument->addTask(SedTask::create(pDocument, model, simulation));
}

libcellml::AnalyserModel::Type CellmlFile::Impl::type() const
{
    return mAnalyserModel->type();
}

libcellml::AnalyserPtr CellmlFile::Impl::analyser() const
{
    return mAnalyser;
}

libcellml::AnalyserModelPtr CellmlFile::Impl::analyserModel() const
{
    return mAnalyserModel;
}

CellmlFileRuntimePtr CellmlFile::Impl::runtime(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
{
    // Check whether we already have a compiled runtime and if so then return it.

    {
        const std::scoped_lock<std::mutex> lock(sRuntimesMutex);
        const auto it = sRuntimes.find(pCellmlFile.get());

        if (it != sRuntimes.end()) {
            return it->second;
        }
    }

    // There is no compiled runtime for this CellML file, so create one, track it, and return it.

    auto runtime = CellmlFileRuntime::create(pCellmlFile, pNlaSolver);

    {
        const std::scoped_lock<std::mutex> lock(sRuntimesMutex);

        sRuntimes.try_emplace(pCellmlFile.get(), runtime);
    }

    return runtime;
}

CellmlFile::CellmlFile(const FilePtr &pFile, const libcellml::ModelPtr &pModel, bool pStrict)
    : Logger(std::make_unique<Impl>(pFile, pModel, pStrict))
{
}

CellmlFile::~CellmlFile()
{
    // Stop tracking our compiled runtime.

    {
        const std::scoped_lock<std::mutex> lock(sRuntimesMutex);

        sRuntimes.erase(this);
    }
}

CellmlFile::Impl *CellmlFile::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl.get());
}

const CellmlFile::Impl *CellmlFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl.get());
}

CellmlFilePtr CellmlFile::create(const FilePtr &pFile)
{
    static const CellmlFilePtr NO_CELLML_FILE_PTR;

    // Check whether the file is a CellML file and if so then return its CellmlFile object.

    if (pFile->pimpl()->type() == File::Type::CELLML_FILE) {
        return pFile->pimpl()->mCellmlFile;
    }

    // Check whether the type of the file is already known and if so then return since there is no need to parse the
    // file contents again.

    if (pFile->pimpl()->mTypeChecked) {
        return NO_CELLML_FILE_PTR;
    }

    // Try to parse the file contents as a CellML file, be it a CellML 1.x or a CellML 2.0 file.

    auto parser {libcellml::Parser::create(false)};
    auto model {parser->parseModel(toString(pFile->contents()))};

    if (parser->errorCount() == 0) {
        return CellmlFilePtr {new CellmlFile {pFile, model, false}};
    }

    return NO_CELLML_FILE_PTR;
}

void CellmlFile::populateDocument(const SedDocumentPtr &pDocument)
{
    pimpl()->populateDocument(pDocument);
}

libcellml::AnalyserModel::Type CellmlFile::type() const
{
    return pimpl()->type();
}

libcellml::AnalyserPtr CellmlFile::analyser() const
{
    return pimpl()->analyser();
}

libcellml::AnalyserModelPtr CellmlFile::analyserModel() const
{
    return pimpl()->analyserModel();
}

CellmlFileRuntimePtr CellmlFile::runtime(const SolverNlaPtr &pNlaSolver)
{
    return CellmlFile::Impl::runtime(shared_from_this(), pNlaSolver);
}

} // namespace libOpenCOR
