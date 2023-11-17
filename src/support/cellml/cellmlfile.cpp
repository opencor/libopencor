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

        if (importer->resolveImports(mModel, stringToPath(pFile->path()).parent_path().string())) {
            mModel = importer->flattenModel(mModel);
        } else {
            addIssues(importer);
        }
    }

    // Analyse the model, if possible.

    if (mErrors.empty()) {
        auto analyser = libcellml::Analyser::create();

        analyser->analyseModel(mModel);

        mAnalyserModel = analyser->model();

        if (analyser->errorCount() != 0) {
            addIssues(analyser);
        }
    }
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
    return (pimpl()->mAnalyserModel != nullptr) ? pimpl()->mAnalyserModel->type() : libcellml::AnalyserModel::Type::UNKNOWN;
}

libcellml::ModelPtr CellmlFile::model() const
{
    return pimpl()->mModel;
}

CellmlFileRuntimePtr CellmlFile::runtime()
{
    return CellmlFileRuntime::create(shared_from_this());
}

} // namespace libOpenCOR
