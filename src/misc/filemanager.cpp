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

#include "filemanager.h"

#include "utils.h"

#include <algorithm>

namespace libOpenCOR {

FileManager *FileManager::instance()
{
    static FileManager instance;

    return &instance;
}

void FileManager::manage(File *pFile)
{
    mFiles.push_back(pFile);
}

void FileManager::unmanage(File *pFile)
{
    mFiles.erase(std::find(mFiles.cbegin(), mFiles.cend(), pFile));
}

FilePtr FileManager::file(const std::string &pFileNameOrUrl) const
{
#if __clang_major__ < 16
    auto [tIsLocalFile, tFileNameOrUrl] = retrieveFileInfo(pFileNameOrUrl);
    auto isLocalFile = tIsLocalFile;
    auto fileNameOrUrl = tFileNameOrUrl;
#else
    auto [isLocalFile, fileNameOrUrl] = retrieveFileInfo(pFileNameOrUrl);
#endif
    auto res = std::find_if(mFiles.cbegin(), mFiles.cend(), [&](const auto &file) {
        return isLocalFile ?
                   file->fileName() == fileNameOrUrl :
                   file->url() == fileNameOrUrl;
    });

    if (res != mFiles.end()) {
        return (*res)->shared_from_this();
    }

    return {};
}

} // namespace libOpenCOR
