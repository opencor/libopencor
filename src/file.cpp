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

#include "cellmlfile.h"
#include "combinearchive.h"
#include "filemanager.h"
#include "sedmlfile.h"
#include "utils.h"

#include <filesystem>

namespace libOpenCOR {

File::Impl::Impl(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents)
{
    // Check whether we are dealing with a local file or a URL.

    auto [isLocalFile, fileNameOrUrl] = retrieveFileInfo(pFileNameOrUrl);

    if (isLocalFile) {
        mFilePath = std::filesystem::u8path(fileNameOrUrl);
    } else {
        mUrl = fileNameOrUrl;
    }

    // Check whether we have some contents and, if so, keep track of it otherwise retrieve it.

    if (!pContents.empty()) {
        setContents(pContents);
    }
#ifndef __EMSCRIPTEN__
    else {
        // Download a local copy of the remote file, if needed.

        if (mFilePath.empty()) {
            auto [res, filePath] = downloadFile(mUrl);

            if (res) {
                mFilePath = std::filesystem::u8path(fileName);
            } else {
                mType = Type::IRRETRIEVABLE_FILE;

                addError("The file could not be downloaded.");
            }
        } else if (!std::filesystem::exists(mFileName.c_str())) {
            mType = Type::IRRETRIEVABLE_FILE;

            addError("The file does not exist.");
        }
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

void File::Impl::checkType(const FilePtr &pOwner)
{
    // Reset the type of the file, if it isn't that of an irretrievable file.

#ifndef __EMSCRIPTEN__
    if (mType != Type::IRRETRIEVABLE_FILE) {
#endif
        mType = Type::UNKNOWN_FILE;
#ifndef __EMSCRIPTEN__
    }
#endif

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
            } else if (mType == Type::UNKNOWN_FILE) {
                addError("The file is not a CellML file, a SED-ML file, or a COMBINE archive.");
            }
        }
    }
}

std::string File::Impl::path() const
{
    return mUrl.empty() ? mFileName : mUrl;
}

#ifndef __EMSCRIPTEN__
void File::Impl::retrieveContents()
{
    // Retrieve the contents of the file, if needed.

    if (!mContentsRetrieved) {
        setContents(fileContents(mFilePath));
    }
}
#endif

void File::Impl::setContents(const std::vector<unsigned char> &pContents)
{
    mContents = pContents;

    mContentsRetrieved = true;
}

std::vector<unsigned char> File::Impl::contents()
{
#ifndef __EMSCRIPTEN__
    retrieveContents();
#endif

    return mContents;
}

File::File(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents)
    : Logger(new Impl(pFileNameOrUrl, pContents))
{
}

File::~File()
{
    // Have ourselves unmanaged.

    FileManager::instance()->unmanage(this);

    delete pimpl();
}

File::Impl *File::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

const File::Impl *File::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}

#ifndef __EMSCRIPTEN__
FilePtr File::create(const std::string &pFileNameOrUrl)
{
    return create(pFileNameOrUrl, {});
}
#endif

FilePtr File::create(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents)
{
    // Check whether the given file name or URL is already managed and if so then return it (after having updated its
    // contents and rechecked its type) otherwise create, manage, and return a new file object.

    auto file = FileManager::instance()->file(pFileNameOrUrl);

    if (file != nullptr) {
        file->pimpl()->setContents(pContents);
        file->pimpl()->checkType(file);

        return file->shared_from_this();
    }

    auto res = std::shared_ptr<File> {new File {pFileNameOrUrl, pContents}};

    res->pimpl()->checkType(res);

    FileManager::instance()->manage(res.get());

    return res;
}

File::Type File::type() const
{
    return pimpl()->mType;
}

std::string File::fileName() const
{
    return pimpl()->mFilePath.string();
}

std::string File::url() const
{
    return pimpl()->mUrl;
}

std::string File::path() const
{
    return pimpl()->path();
}

#ifdef __EMSCRIPTEN__
emscripten::val File::jsContents()
{
    auto contents = this->contents();
    auto view = emscripten::typed_memory_view(contents.size(), contents.data());
    auto res = emscripten::val::global("Uint8Array").new_(contents.size());

    res.call<void>("set", view);

    return res;
}
#endif

std::vector<unsigned char> File::contents()
{
    return pimpl()->contents();
}

} // namespace libOpenCOR
