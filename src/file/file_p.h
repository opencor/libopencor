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

#include "cellmlfile.h"
#include "combinearchive.h"
#include "sedmlfile.h"

#include "libopencor/file.h"

#include <filesystem>

namespace libOpenCOR {

class File::Impl: public Logger::Impl
{
public:
    Type mType = Type::UNKNOWN_FILE;

    std::filesystem::path mFilePath;
    std::string mUrl;

    bool mRetrieveContents = true;
    bool mContentsRetrieved = false;
    UnsignedChars mContents;

    CellmlFilePtr mCellmlFile;
    SedmlFilePtr mSedmlFile;
    CombineArchivePtr mCombineArchive;

    explicit Impl(const std::string &pFileNameOrUrl, bool pRetrieveContents);
    ~Impl();

    void checkType(const FilePtr &pOwner, bool pResetType = false);

    Type type() const;
    std::string fileName() const;
    std::string url() const;
    std::string path() const;

    UnsignedChars contents();
    void setContents(const UnsignedChars &pContents);

    bool hasChildFiles() const;
    size_t childFileCount() const;
    Strings childFileNames() const;
    FilePtrs childFiles() const;
    FilePtr childFile(size_t pIndex) const;
#ifdef __EMSCRIPTEN__
    FilePtr childFileFromFileName(const std::string &pFileName) const;
#else
    FilePtr childFile(const std::string &pFileName) const;
#endif
};

} // namespace libOpenCOR
