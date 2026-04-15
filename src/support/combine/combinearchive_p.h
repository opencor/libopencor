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

#include "logger_p.h"

#include "combinearchive.h"

namespace libOpenCOR {

class CombineArchive::Impl: public Logger::Impl
{
public:
    UnsignedChars mArchiveContents;
    libcombine::CombineArchive *mArchive;
    std::string mArchiveLocation;
    size_t mArchiveLocationSize;
    std::vector<FilePtr> mFiles;
    std::vector<std::string> mFileNames;
    FilePtr mMasterFile;

    explicit Impl(const FilePtr &pFile, libcombine::CombineArchive *pArchive, UnsignedChars &&pArchiveContents);
    ~Impl() override;

    const FilePtr &masterFile() const;
    bool hasFiles() const;
    size_t fileCount() const;
    const Strings &fileNames() const;
    const FilePtrs &files() const;
    const FilePtr &file(size_t pIndex) const;
    const FilePtr &file(const std::string &pFileName) const;
};

} // namespace libOpenCOR
