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

#include "combinearchive_p.h"

#include "libopencor/file.h"

#include <combine/combinearchive.h>

namespace libOpenCOR {

CombineArchive::Impl::Impl(libcombine::CombineArchive *pArchive)
    : mArchive(pArchive)
{
}

CombineArchive::Impl::~Impl()
{
    delete mArchive;
}

CombineArchive::CombineArchive(libcombine::CombineArchive *pArchive)
    : Logger(new Impl {pArchive})
{
}

CombineArchive::~CombineArchive()
{
    delete pimpl();
}

CombineArchive::Impl *CombineArchive::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const CombineArchive::Impl *CombineArchive::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}
*/

CombineArchivePtr CombineArchive::create(const FilePtr &pFile)
{
    // Try to retrieve a COMBINE archive.
    // Note: a COMBINE archive is a ZIP file, so we make sure that it starts with 0x04034b50, which is the magic number
    //       used by a ZIP file (we ignore empty and spanned ZIP files). Indeed, libCOMBINE may crash depending on the
    //       file contents that is given to it.

    static const auto LS8 = 8U;
    static const auto LS16 = 16U;
    static const auto LS24 = 24U;
    static const auto ZIP_MAGIC_NUMBER = 0x04034b50;

    auto fileContents = pFile->contents();

    if ((fileContents.size() > 4)
        && (fileContents[0] + (fileContents[1] << LS8) + (fileContents[2] << LS16) + (fileContents[3] << LS24) == ZIP_MAGIC_NUMBER)) {
        auto *archive = new libcombine::CombineArchive {};

        archive->initializeFromArchive(fileContents);

        return CombineArchivePtr {new CombineArchive {archive}};
    }

    return nullptr;
}

} // namespace libOpenCOR
