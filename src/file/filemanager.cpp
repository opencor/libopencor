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
#include <stack>

namespace libOpenCOR {

FileManager::Impl &FileManager::Impl::instance()
{
    static Impl instance;

    return instance;
}

FilePtr FileManager::Impl::manage(const FilePtr &pFile)
{
    const std::unique_lock<std::shared_mutex> lock(mMutex);

    // Opportunistically remove any expired entries and correct our file count.

    const auto expiredEnd = std::remove_if(mFiles.begin(), mFiles.end(), [](const auto &file) {
        return file.expired();
    });

    mFileCount -= static_cast<size_t>(mFiles.end() - expiredEnd);

    mFiles.erase(expiredEnd, mFiles.end());

    // Check whether we already manage a file with the same name or URL. This must be done under the exclusive lock to
    // avoid a TOCTOU race with another thread that might have just managed the same file between our caller's existence
    // check and this call.

    const bool isLocalFile = pFile->url().empty();
    const auto &fileNameOrUrl = isLocalFile ? pFile->fileName() : pFile->url();

    for (const auto &file : mFiles) {
        auto managedFile {file.lock()};

        if (managedFile != nullptr) {
            if (isLocalFile ? managedFile->fileName() == fileNameOrUrl : managedFile->url() == fileNameOrUrl) {
                return managedFile;
            }
        }
    }

    // No duplicate found, so manage the new file.

    mFiles.emplace_back(pFile);

    ++mFileCount;

    return pFile;
}

void FileManager::Impl::unmanage(File *pFile)
{
    const std::unique_lock<std::shared_mutex> lock(mMutex);

    // Iteratively unmanage the file and all its child files.
    // Note: it would be much simpler to use recursion, but Clang-Tidy does not like it.

    std::stack<File *> files;

    files.push(pFile);

    while (!files.empty()) {
        // Retrieve the file at the top of the stack.

        auto *file {files.top()};

        files.pop();

        // Add the child files to the stack.

        for (const auto &childFile : file->childFiles()) {
            files.push(childFile.get());
        }

        // Unmanage the current file.

        const auto removeEnd = std::remove_if(mFiles.begin(), mFiles.end(), [&file](const auto &managedFile) {
            auto managedFilePtr {managedFile.lock()};

            return (managedFilePtr == nullptr) || (managedFilePtr.get() == file);
        });

        mFileCount -= static_cast<size_t>(mFiles.end() - removeEnd);

        mFiles.erase(removeEnd, mFiles.end());
    }
}

void FileManager::Impl::reset()
{
    const std::unique_lock<std::shared_mutex> lock(mMutex);

    mFiles.clear();

    mFileCount = 0;
}

bool FileManager::Impl::hasFiles() const
{
    return mFileCount != 0;
    // Note: mFileCount is an atomic near-real-time counter. It is updated in manage(), unmanage(), and reset(), so it
    //       may briefly lag behind a file that has just expired but whose destructor has not yet entered unmanage().
    //       This is an inherent TOCTOU property of weak_ptr semantics. Callers that need a strict point-in-time
    //       snapshot should use files() instead.
}

size_t FileManager::Impl::fileCount() const
{
    return mFileCount;
    // Note: same semantics as hasFiles() (see the note above).
}

FilePtrs FileManager::Impl::files() const
{
    const std::shared_lock<std::shared_mutex> lock(mMutex);

    FilePtrs res;

    for (const auto &file : mFiles) {
        if (auto managedFile {file.lock()}; managedFile != nullptr) {
            res.push_back(std::move(managedFile));
        }
    }

    return res;
}

FilePtr FileManager::Impl::file(size_t pIndex) const
{
    const std::shared_lock<std::shared_mutex> lock(mMutex);

    size_t index {0};

    for (const auto &file : mFiles) {
        if (auto managedFile {file.lock()}; managedFile != nullptr) {
            if (index == pIndex) {
                return managedFile;
            }

            ++index;
        }
    }

    return nullptr;
}

#ifdef __EMSCRIPTEN__
FilePtr FileManager::Impl::fileFromFileNameOrUrl(const std::string &pFileNameOrUrl) const
#else
FilePtr FileManager::Impl::file(const std::string &pFileNameOrUrl) const
#endif
{
    const std::shared_lock<std::shared_mutex> lock(mMutex);

#if __clang_major__ < 16
    auto [tIsLocalFile, tFileNameOrUrl] {retrieveFileInfo(pFileNameOrUrl)};
    auto isLocalFile {tIsLocalFile};
    auto fileNameOrUrl {tFileNameOrUrl};
#else
    auto [isLocalFile, fileNameOrUrl] {retrieveFileInfo(pFileNameOrUrl)};
#endif
    for (const auto &file : mFiles) {
        if (auto managedFile {file.lock()}; managedFile != nullptr) {
            if (isLocalFile ? managedFile->fileName() == fileNameOrUrl : managedFile->url() == fileNameOrUrl) {
                return managedFile;
            }
        }
    }

    return nullptr;
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
    mPimpl.manage(pFile);
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
