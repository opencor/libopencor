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
    : Logger(new Impl(pArchive))
{
}

CombineArchive::~CombineArchive()
{
    delete pimpl();
}

CombineArchive::Impl *CombineArchive::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const CombineArchive::Impl *CombineArchive::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

CombineArchivePtr CombineArchive::create(const FilePtr &pFile)
{
#ifdef __EMSCRIPTEN__
    (void)pFile;

    return nullptr;
#else
    // Try to retrieve a COMBINE archive.

    auto *archive = new libcombine::CombineArchive();

    if (archive->initializeFromArchive(pFile->fileName())) {
        return CombineArchivePtr {new CombineArchive {archive}};
    }

    delete archive;

    return nullptr;
#endif
}

} // namespace libOpenCOR
