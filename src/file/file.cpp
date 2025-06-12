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

#include "file_p.h"
#include "filemanager_p.h"

#include "utils.h"

namespace libOpenCOR {

File::Impl::Impl(const std::string &pFileNameOrUrl, bool pRetrieveContents)
    : Logger::Impl()
    , mRetrieveContents(pRetrieveContents)
{
    // Check whether we are dealing with a local file or a URL.

    auto [isLocalFile, fileNameOrUrl] = retrieveFileInfo(pFileNameOrUrl);

    if (isLocalFile) {
        mFilePath = stringToPath(fileNameOrUrl);
    } else {
        mUrl = fileNameOrUrl;
    }

#ifndef __EMSCRIPTEN__
    // Download a local copy of the remote file, if needed and requested.

    if (mFilePath.empty()) {
        if (pRetrieveContents) {
            auto [res, filePath] = downloadFile(mUrl);

            if (res) {
                mFilePath = filePath;
            } else {
                mType = Type::IRRETRIEVABLE_FILE;

                addError("The file could not be downloaded.");
            }
        } else {
            mFilePath = stringToPath("/some/path/file");
        }
    } else if (!std::filesystem::exists(mFilePath)) {
        mType = Type::IRRETRIEVABLE_FILE;

        addError("The file does not exist.");
    }
#else
    if (mFilePath.empty()) {
        mFilePath = stringToPath("/some/path/file");
    }
#endif
}

File::Impl::~Impl()
{
    // Delete the local file associated with a remote file.

    if (!mUrl.empty() && !mFilePath.empty()) {
        std::filesystem::remove(mFilePath);
    }
}

void File::Impl::checkType(const FilePtr &pOwner, bool pResetType)
{
    // Reset he type of the file, if needed.

    if (pResetType) {
        removeAllIssues();

        mType = Type::UNKNOWN_FILE;
#ifndef __EMSCRIPTEN__
    } else if (mType == Type::IRRETRIEVABLE_FILE) {
        return;
#endif
    }

    // Try to get a CellML file, a SED-ML file, or a COMBINE archive.

    mCellmlFile = CellmlFile::create(pOwner);

    if (mCellmlFile != nullptr) {
        mType = Type::CELLML_FILE;

        addIssues(mCellmlFile);
    } else {
        mSedmlFile = SedmlFile::create(pOwner);

        if (mSedmlFile != nullptr) {
            mType = Type::SEDML_FILE;

            addIssues(mSedmlFile);
        } else {
            mCombineArchive = CombineArchive::create(pOwner);

            if (mCombineArchive != nullptr) {
                mType = Type::COMBINE_ARCHIVE;

                addIssues(mCombineArchive);
            } else {
                addError("The file is not a CellML file, a SED-ML file, or a COMBINE archive.");
            }
        }
    }
}

File::Type File::Impl::type() const
{
    return mType;
}

std::string File::Impl::fileName() const
{
    return pathToString(mFilePath);
}

std::string File::Impl::url() const
{
    return mUrl;
}

std::string File::Impl::path() const
{
    return mUrl.empty() ? fileName() : mUrl;
}

UnsignedChars File::Impl::contents()
{
#ifndef __EMSCRIPTEN__
    // Retrieve the contents of the file, if needed.

    if (mRetrieveContents && !mContentsRetrieved) {
        setContents(fileContents(mFilePath));
    }
#endif

    return mContents;
}

void File::Impl::setContents(const UnsignedChars &pContents)
{
    mContents = pContents;

    mContentsRetrieved = true;
}

bool File::Impl::hasChildFiles() const
{
    if (mType == Type::COMBINE_ARCHIVE) {
        return mCombineArchive->hasFiles();
    }

    return false;
}

size_t File::Impl::childFileCount() const
{
    if (mType == Type::COMBINE_ARCHIVE) {
        return mCombineArchive->fileCount();
    }

    return 0;
}

Strings File::Impl::childFileNames() const
{
    if (mType == Type::COMBINE_ARCHIVE) {
        return mCombineArchive->fileNames();
    }

    return {};
}

FilePtrs File::Impl::childFiles() const
{
    if (mType == Type::COMBINE_ARCHIVE) {
        return mCombineArchive->files();
    }

    return {};
}

FilePtr File::Impl::childFile(size_t pIndex) const
{
    if (mType == Type::COMBINE_ARCHIVE) {
        if (pIndex >= mCombineArchive->fileCount()) {
            return {};
        }

        return mCombineArchive->file(pIndex);
    }

    return {};
}

#ifdef __EMSCRIPTEN__
FilePtr File::Impl::childFileFromFileName(const std::string &pFileName) const
#else
FilePtr File::Impl::childFile(const std::string &pFileName) const
#endif
{
    if (mType == Type::COMBINE_ARCHIVE) {
        return mCombineArchive->file(pFileName);
    }

    return {};
}

File::File(const std::string &pFileNameOrUrl, bool pRetrieveContents)
    : Logger(new Impl {pFileNameOrUrl, pRetrieveContents})
{
}

File::~File()
{
    // Have ourselves unmanaged.

    FileManager::instance().mPimpl.unmanage(this);

    delete pimpl();
}

File::Impl *File::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl);
}

const File::Impl *File::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

#ifdef __EMSCRIPTEN__
FilePtr File::create(const std::string &pFileNameOrUrl)
#else
FilePtr File::create(const std::string &pFileNameOrUrl, bool pRetrieveContents)
#endif
{
    // Check whether the given file name or URL is already managed and if so then return it otherwise create, manage,
    // and return a new file object.

    auto fileManager = FileManager::instance();
#ifdef __EMSCRIPTEN__
    auto file = fileManager.fileFromFileNameOrUrl(pFileNameOrUrl);
#else
    auto file = fileManager.file(pFileNameOrUrl);
#endif

    if (file != nullptr) {
        return file;
    }

#ifdef __EMSCRIPTEN__
    auto res = FilePtr {new File {pFileNameOrUrl, false}};
#else
    auto res = FilePtr {new File {pFileNameOrUrl, pRetrieveContents}};
#endif

    res->pimpl()->checkType(res);

    fileManager.mPimpl.manage(res.get());

    return res;
}

File::Type File::type() const
{
    return pimpl()->type();
}

std::string File::fileName() const
{
    return pimpl()->fileName();
}

std::string File::url() const
{
    return pimpl()->url();
}

std::string File::path() const
{
    return pimpl()->path();
}

UnsignedChars File::contents()
{
    return pimpl()->contents();
}

void File::setContents(const UnsignedChars &pContents)
{
    pimpl()->setContents(pContents);

    pimpl()->checkType(shared_from_this(), true);
}

bool File::hasChildFiles() const
{
    return pimpl()->hasChildFiles();
}

size_t File::childFileCount() const
{
    return pimpl()->childFileCount();
}

Strings File::childFileNames() const
{
    return pimpl()->childFileNames();
}

FilePtrs File::childFiles() const
{
    return pimpl()->childFiles();
}

FilePtr File::childFile(size_t pIndex) const
{
    return pimpl()->childFile(pIndex);
}

#ifdef __EMSCRIPTEN__
FilePtr File::childFileFromFileName(const std::string &pFileName) const
#else
FilePtr File::childFile(const std::string &pFileName) const
#endif
{
#ifdef __EMSCRIPTEN__
    return pimpl()->childFileFromFileName(pFileName);
#else
    return pimpl()->childFile(pFileName);
#endif
}

} // namespace libOpenCOR
