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

#include "libopencor/file.h"

namespace libOpenCOR {

namespace Support {
class CellmlFile;
class SedmlFile;
class CombineArchive;
} // namespace Support

struct File::Impl
{
    Type mType = Type::UNKNOWN_FILE;

    std::string mFileName;
    std::string mUrl;

    bool mContentsRetrieved = false;
    std::vector<unsigned char> mContents;

    Support::CellmlFile *mCellmlFile = nullptr;
    Support::SedmlFile *mSedmlFile = nullptr;
    Support::CombineArchive *mCombineArchive = nullptr;

    Impl(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents);
    ~Impl();

    void checkType(const FilePtr &pOwner);

#ifndef __EMSCRIPTEN__
    void retrieveContents();
#endif

    std::vector<unsigned char> contents();
    size_t size();
};

} // namespace libOpenCOR
