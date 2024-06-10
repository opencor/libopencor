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

#include "cellmlfileruntime.h"
#include "utils.h"

#include "libopencor/file.h"

namespace libOpenCOR {

CellmlFile::Impl::Impl(const FilePtr &pFile, const libcellml::ModelPtr &pModel, bool pStrict)
    : mModel(pModel)
{
    // Resolve imports and flatten the model, if needed and possible.

    if (mModel->hasUnresolvedImports()) {
        auto importer = libcellml::Importer::create(pStrict);

        if (importer->resolveImports(mModel, pathToString(stringToPath(pFile->path()).parent_path()))) {
            mModel = importer->flattenModel(mModel);
        } else {
            addIssues(importer);
        }
    }

    // Analyse the model.
    // Note: we do this even if there are some errors (as a result of resolving imports). This is so that we can
    //       retrieve the analyser's issues if needed (e.g., when wanting to retrieve a runtime) and so that we don't
    //       have to test mAnalyserModel for nullity everywhere.

    mAnalyser->analyseModel(mModel);

    mAnalyserModel = mAnalyser->model();

    if (mAnalyser->errorCount() != 0) {
        addIssues(mAnalyser);
    }

    //---GRY--- WHEN USING OUR JavaScript BINDINGS, TEMPORARILY MAKE SURE THAT THE MODEL IS EITHER AN ALGEBRAIC MODEL OR AN ODE MODEL.

#ifdef __EMSCRIPTEN__
    if (!hasIssues()
        && (mAnalyserModel->type() != libcellml::AnalyserModel::Type::ALGEBRAIC)
        && (mAnalyserModel->type() != libcellml::AnalyserModel::Type::ODE)) {
        addError("Only CellML files describing either an algebraic model or an ODE model are currently supported.");
    }
#endif
}

CellmlFile::CellmlFile(const FilePtr &pFile, const libcellml::ModelPtr &pModel, bool pStrict)
    : Logger(new Impl {pFile, pModel, pStrict})
{
}

CellmlFile::~CellmlFile()
{
    delete pimpl();
}

CellmlFile::Impl *CellmlFile::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

const CellmlFile::Impl *CellmlFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}

CellmlFilePtr CellmlFile::create(const FilePtr &pFile)
{
    // Try to parse the file contents as a CellML 2.0 file.

    auto isCellmlFile = false;

    if (!pFile->contents().empty()) {
        auto strict = true;
        auto parser = libcellml::Parser::create(strict);
        auto contents = toString(pFile->contents());
        auto model = parser->parseModel(contents);

        if (parser->errorCount() != 0) {
            // We couldn't parse the file contents as a CellML 2.0 file, so maybe it is a CellML 1.x file?

            strict = false;
            parser = libcellml::Parser::create(strict);
            model = parser->parseModel(contents);

            isCellmlFile = parser->errorCount() == 0;
        } else {
            isCellmlFile = true;
        }

        if (isCellmlFile) {
            return CellmlFilePtr {new CellmlFile {pFile, model, strict}};
        }
    }

    return nullptr;
}

libcellml::AnalyserModel::Type CellmlFile::type() const
{
    return pimpl()->mAnalyserModel->type();
}

libcellml::AnalyserPtr CellmlFile::analyser() const
{
    return pimpl()->mAnalyser;
}

libcellml::AnalyserModelPtr CellmlFile::analyserModel() const
{
    return pimpl()->mAnalyserModel;
}

CellmlFileRuntimePtr CellmlFile::runtime(const SolverNlaPtr &pNlaSolver, bool pCompiled)
{
    return CellmlFileRuntime::create(shared_from_this(), pNlaSolver, pCompiled);
}

} // namespace libOpenCOR
