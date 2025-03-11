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

#include "filemanager_p.h"

#include "utils.h"

#include <algorithm>

namespace libOpenCOR {

FileManager::Impl &FileManager::Impl::instance()
{
    static Impl instance;

    return instance;
}

void FileManager::Impl::manage(File *pFile)
{
    mFiles.push_back(pFile);
}

void FileManager::Impl::unmanage(File *pFile)
{
    // First, unmanage all the child files.

    for (const auto &childFile : pFile->childFiles()) {
        unmanage(childFile.get());
    }

    // Then, unmanage the file itself.

    auto iter = std::ranges::find(mFiles, pFile);

    if (iter != mFiles.cend()) {
        mFiles.erase(iter);
    }
}

void FileManager::Impl::reset()
{
    mFiles.clear();
}

bool FileManager::Impl::hasFiles() const
{
    return !mFiles.empty();
}

size_t FileManager::Impl::fileCount() const
{
    return mFiles.size();
}

FilePtrs FileManager::Impl::files() const
{
    FilePtrs res;

    for (const auto &file : mFiles) {
        res.push_back(file->shared_from_this());
    }

    return res;
}

FilePtr FileManager::Impl::file(size_t pIndex) const
{
    if (pIndex >= mFiles.size()) {
        return {};
    }

    return mFiles[pIndex]->shared_from_this();
}

#ifdef __EMSCRIPTEN__
FilePtr FileManager::Impl::fileFromFileNameOrUrl(const std::string &pFileNameOrUrl) const
#else
FilePtr FileManager::Impl::file(const std::string &pFileNameOrUrl) const
#endif
{
#if __clang_major__ < 16
    auto [tIsLocalFile, tFileNameOrUrl] = retrieveFileInfo(pFileNameOrUrl);
    auto isLocalFile = tIsLocalFile;
    auto fileNameOrUrl = tFileNameOrUrl;
#else
    auto [isLocalFile, fileNameOrUrl] = retrieveFileInfo(pFileNameOrUrl);
#endif
    auto res = std::ranges::find_if(mFiles, [&](const auto &file) {
        return isLocalFile ?
                   file->fileName() == fileNameOrUrl :
                   file->url() == fileNameOrUrl;
    });

    if (res != mFiles.end()) {
        return (*res)->shared_from_this();
    }

    return {};
}

FileManager &FileManager::instance()
{
    static FileManager instance;

    return instance;
}

FileManager::FileManager()
    : mPimpl(Impl::instance())
{
}

void FileManager::manage(const FilePtr &pFile)
{
    mPimpl.manage(pFile.get());
}

void FileManager::unmanage(const FilePtr &pFile)
{
    mPimpl.unmanage(pFile.get());
}

void FileManager::reset()
{
    mPimpl.reset();
}

bool FileManager::hasFiles() const
{
    return mPimpl.hasFiles();
}

size_t FileManager::fileCount() const
{
    return mPimpl.fileCount();
}

FilePtrs FileManager::files() const
{
    return mPimpl.files();
}

FilePtr FileManager::file(size_t pIndex) const
{
    return mPimpl.file(pIndex);
}

#ifdef __EMSCRIPTEN__
FilePtr FileManager::fileFromFileNameOrUrl(const std::string &pFileNameOrUrl) const
#else
FilePtr FileManager::file(const std::string &pFileNameOrUrl) const
#endif
{
#ifdef __EMSCRIPTEN__
    return mPimpl.fileFromFileNameOrUrl(pFileNameOrUrl);
#else
    return mPimpl.file(pFileNameOrUrl);
#endif
}

} // namespace libOpenCOR
