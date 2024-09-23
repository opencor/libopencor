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

// #define PRINT_MANAGED_FILES

namespace libOpenCOR {

FileManager::Impl &FileManager::Impl::instance()
{
    static Impl instance;

    return instance;
}

#ifdef PRINT_MANAGED_FILES
namespace {

void printListOfFiles(const std::string &pInfo, const Files &pFiles)
{
    size_t i = 0;

    printf("---[MANAGED FILES]-[%s]---[BEGIN]\n", pInfo.c_str()); // NOLINT

    for (const auto &file : pFiles) {
        auto type = file->type();
        const auto *typeAsString = [&]() {
            switch (type) {
            case File::Type::UNKNOWN_FILE:
                return "Unknown file";
            case File::Type::CELLML_FILE:
                return "CellML file";
            case File::Type::SEDML_FILE:
                return "SED-ML file";
#    ifdef __EMSCRIPTEN__
            default: // File::Type::COMBINE_ARCHIVE.
                return "COMBINE archive";
#    else
            case File::Type::COMBINE_ARCHIVE:
                return "COMBINE archive";
            default: // File::Type::IRRETRIEVABLE_FILE.
                return "Irretrievable file";
#    endif
            }
        }();

        printf("[%03zu] %s [%s]\n", ++i, file->fileName().c_str(), typeAsString); // NOLINT
    }

    printf("---[MANAGED FILES]-[%s]---[END]\n", pInfo.c_str()); // NOLINT
}

} // namespace
#endif

void FileManager::Impl::manage(File *pFile)
{
    mFiles.push_back(pFile);

#ifdef PRINT_MANAGED_FILES
    printListOfFiles(std::string("Manage ") + pFile->fileName(), mFiles);
#endif
}

void FileManager::Impl::unmanage(File *pFile)
{
    auto iter = std::find(mFiles.cbegin(), mFiles.cend(), pFile);

    if (iter != mFiles.cend()) {
        mFiles.erase(iter);
    }

#ifdef PRINT_MANAGED_FILES
    printListOfFiles(std::string("Unmanage ") + pFile->fileName(), mFiles);
#endif
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

FilePtr FileManager::Impl::file(const std::string &pFileNameOrUrl) const
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

FilePtr FileManager::file(const std::string &pFileNameOrUrl) const
{
    return mPimpl.file(pFileNameOrUrl);
}

} // namespace libOpenCOR
