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

#pragma once

#include "unittestingexport.h"

#include "libopencor/logger.h"

namespace libcombine {
class CombineArchive;
}

namespace libOpenCOR {

class OmexFile;
using OmexFilePtr = std::shared_ptr<OmexFile>;

class LIBOPENCOR_UNIT_TESTING_EXPORT OmexFile: public Logger
{
public:
    OmexFile() = delete;
    ~OmexFile() override;

    OmexFile(const OmexFile &pOther) = delete;
    OmexFile(OmexFile &&pOther) noexcept = delete;

    OmexFile &operator=(const OmexFile &pRhs) = delete;
    OmexFile &operator=(OmexFile &&pRhs) noexcept = delete;

    static OmexFilePtr create(const FilePtr &pFile);

    const FilePtr &masterFile() const;
    size_t cellmlFileCount() const;
    size_t sedmlFileCount() const;

    bool hasFiles() const;
    size_t fileCount() const;
    const Strings &fileNames() const;
    const FilePtrs &files() const;
    const FilePtr &file(size_t pIndex) const;
    const FilePtr &file(const std::string &pFileName) const;

private:
    class Impl;

    explicit OmexFile(const FilePtr &pFile, libcombine::CombineArchive *pArchive, UnsignedChars &&pArchiveContents);

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR
