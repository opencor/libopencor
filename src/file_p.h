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

#include "cellmlfile.h"
#include "combinearchive.h"
#include "logger_p.h"
#include "sedmlfile.h"

#include "libopencor/file.h"

namespace libOpenCOR {

class File::Impl: public Logger::Impl
{
public:
    Type mType = Type::UNKNOWN_FILE;

    std::string mFileName;
    std::string mUrl;

    bool mContentsRetrieved = false;
    std::vector<unsigned char> mContents;

    CellmlFilePtr mCellmlFile;
    SedmlFilePtr mSedmlFile;
    CombineArchivePtr mCombineArchive;

    Impl(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents);
    ~Impl();

    void checkType(const FilePtr &pOwner);

    std::string path() const;

#ifndef __EMSCRIPTEN__
    void retrieveContents();
#endif

    std::vector<unsigned char> contents();
    void setContents(const std::vector<unsigned char> &pContents);
};

} // namespace libOpenCOR
