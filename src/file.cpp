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

#include "cellmlfile.h"
#include "combinearchive.h"

#ifndef __EMSCRIPTEN__
#    include "compiler.h"
#endif

#include "file_p.h"
#include "sedmlfile.h"
#include "support.h"
#include "utils.h"

#include <filesystem>
#include <regex>

#include <curl/curl.h>

#include <libcellml>

namespace libOpenCOR {

File::Impl::Impl(const std::string &pFileNameOrUrl, const char *pContents, size_t pSize)
{
    // By default, we assume that we are dealing with a local file, but it may be in the form of a URL, i.e. starting
    // with "file://".

    if (pFileNameOrUrl.find("file://") == 0) {
        // We are dealing with a URL representing a local file, so retrieve its actual path.

        static constexpr size_t SCHEME_LENGTH = 7;

        mFileName = pFileNameOrUrl;

        mFileName.erase(0, SCHEME_LENGTH);

        // On Windows, we also need to remove the leading "/" and replace all the other "/"s with "\"s.

        static const auto WINDOWS_PATH_REGEX = std::regex("^/[A-Z]:/.*");

        std::smatch match;

        std::regex_search(mFileName, match, WINDOWS_PATH_REGEX);

        if (!match.empty()) {
            static constexpr size_t SLASH_LENGTH = 1;
            static const auto FORWARD_SLASH_REGEX = std::regex("/");

            mFileName.erase(0, SLASH_LENGTH);

            mFileName = std::regex_replace(mFileName, FORWARD_SLASH_REGEX, "\\");
        }
    } else {
        // If we can get a URL from pFileNameOrUrl then it means that we are dealing with a remote file otherwise we are
        // dealing with a local file.

        CURLU *url = curl_url();

        if (curl_url_set(url, CURLUPART_URL, pFileNameOrUrl.c_str(), 0) == CURLUE_OK) {
            mUrl = pFileNameOrUrl;
        } else {
            mFileName = pFileNameOrUrl;
        }

        curl_url_cleanup(url);
    }

    // Check whether we have some contents and, if so, keep track of it and consider the file virtual.
    // Note: we end the contents with a '\0' so that we can use it as a C string.

    if ((pContents != nullptr) && (pSize != 0)) {
        mContents = new char[pSize + 1] {};
        mSize = pSize;

        std::copy_n(pContents, pSize, mContents);

        mContentsRetrieved = true;
    }
#ifndef __EMSCRIPTEN__
    // If we don't have any contents, then we get ready to retrieve it.

    else {
        // Download a local copy of the remote file, if needed.

        if (mFileName.empty()) {
            auto [res, fileName] = downloadFile(mUrl);

            mFileName = fileName;
        }

        // Make sure that the (local) file exists.

        if (mFileName.empty() || !std::filesystem::exists(mFileName.c_str())) {
            mType = Type::IRRETRIEVABLE_FILE;

            return;
        }
    }
#endif
}

File::Impl::~Impl()
{
    // Delete the local file associated with a remote file.

    if (!mUrl.empty() && !mFileName.empty()) {
        remove(mFileName.c_str()); // NOLINT(cert-err33-c)
    }

    // Delete some internal objects.

    delete mContents;

    delete mCellmlFile;
    delete mSedmlFile;
    delete mCombineArchive;
}

void File::Impl::checkType(const FilePtr &pOwner)
{
    // Check whether the file is a CellML file, a SED-ML file, or a COMBINE archive.

    if (Support::isCellmlFile(pOwner)) {
        mType = Type::CELLML_FILE;
    } else if (Support::isSedmlFile(pOwner)) {
        mType = Type::SEDML_FILE;
    } else if (Support::isCombineArchive(pOwner)) {
        mType = Type::COMBINE_ARCHIVE;
    }
}

#ifndef __EMSCRIPTEN__
void File::Impl::retrieveContents()
{
    // Retrieve the contents of the file, if needed.

    if (!mContentsRetrieved) {
        auto [res, contents, size] = fileContents(mFileName);

        if (res) {
            mContents = contents;
            mSize = size;
        } else {
            mType = Type::IRRETRIEVABLE_FILE;
        }

        mContentsRetrieved = true;
    }
}
#endif

const char *File::Impl::contents()
{
#ifndef __EMSCRIPTEN__
    retrieveContents();
#endif

    return mContents;
}

size_t File::Impl::size()
{
#ifndef __EMSCRIPTEN__
    retrieveContents();
#endif

    return mSize;
}

File::File(const std::string &pFileNameOrUrl, const char *pContents, size_t pSize)
    : mPimpl(new Impl(pFileNameOrUrl, pContents, pSize))
{
}

File::~File()
{
    delete mPimpl;
}

#ifndef __EMSCRIPTEN__
FilePtr File::create(const std::string &pFileNameOrUrl)
{
    auto res = std::shared_ptr<File> {new File {pFileNameOrUrl}};

    res->mPimpl->checkType(res);

    return res;
}
#endif

FilePtr File::create(const std::string &pFileNameOrUrl, const char *pContents, size_t pSize)
{
    auto res = std::shared_ptr<File> {new File {pFileNameOrUrl, pContents, pSize}};

    res->mPimpl->checkType(res);

    return res;
}

File::Type File::type() const
{
    return mPimpl->mType;
}

std::string File::fileName() const
{
    return mPimpl->mFileName;
}

std::string File::url() const
{
    return mPimpl->mUrl;
}

#ifdef __EMSCRIPTEN__
emscripten::val File::jsContents() const
{
    return emscripten::val(emscripten::typed_memory_view(size(), contents()));
}
#endif

const char *File::contents() const
{
    return mPimpl->contents();
}

size_t File::size() const
{
    return mPimpl->size();
}

} // namespace libOpenCOR
