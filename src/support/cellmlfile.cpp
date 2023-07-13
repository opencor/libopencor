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

#include "cellmlfile.h"

#include "cellmlfile_p.h"
#include "utils.h"

#include "libopencor/file.h"

namespace libOpenCOR {

CellmlFile::Impl::Impl(const libcellml::ModelPtr &pModel)
    : mModel(pModel)
{
}

CellmlFile::CellmlFile(const libcellml::ModelPtr &pModel)
    : Logger(new Impl(pModel))
{
}

CellmlFile::~CellmlFile()
{
    delete pimpl();
}

CellmlFile::Impl *CellmlFile::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const CellmlFile::Impl *CellmlFile::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

CellmlFilePtr CellmlFile::create(const FilePtr &pFile)
{
    // Try to parse the file contents as a CellML 2.0 file.

    auto isCellmlFile = false;

    if (!pFile->contents().empty()) {
        auto parser = libcellml::Parser::create();
        auto contents = contentsAsString(pFile->contents());
        auto model = parser->parseModel(contents);

        if (parser->errorCount() != 0) {
            // We couldn't parse the file contents as a CellML 2.0 file, so maybe it is a CellML 1.x file?

            parser = libcellml::Parser::create(false);
            model = parser->parseModel(contents);

            isCellmlFile = parser->errorCount() == 0;
        } else {
            isCellmlFile = true;
        }

        if (isCellmlFile) {
            return std::shared_ptr<CellmlFile> {new CellmlFile {model}};
        }
    }

    return nullptr;
}

} // namespace libOpenCOR
