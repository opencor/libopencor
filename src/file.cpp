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

#include "cellmlfilemanager.h"
#include "combinefilemanager.h"
#include "file_p.h"
#include "sedmlfilemanager.h"
#include "utils.h"

#include <regex>

#include <curl/curl.h>

namespace libOpenCOR {

File::Impl::~Impl()
{
    reset();
}

void File::Impl::reset()
{
    // Reset ourself which means removing the local copy of a remote file, as
    // well as clearning our contents, if any.

    if (!mUrl.empty() && !mFileName.empty()) {
        remove(mFileName.c_str());

        mFileName = {};
    }

    delete[] mContents;
    mSize = 0;
}

File::Status File::Impl::retrieveContents()
{
    reset();

    // Download a local copy of the remote file, if appropriate.

    if (mFileName.empty()) {
        mFileName = downloadFile(mUrl);

        if (mFileName.empty()) {
            return File::Status::NON_RETRIEVABLE_REMOTE_FILE;
        }
    }

    // Retrieve the contents of the local file or of the local copy of a remote
    // file.

    if (!mFileName.empty()) {
        auto [mContents, mSize] = fileContents(mFileName);

        if ((mContents == nullptr) && (mSize == 0)) {
            // We assume that we are always able to open the local copy of a
            // remote file. So, if we can't open it, it's because we are dealing
            // with a non-existing local file.

            assert(mUrl.empty());

            return File::Status::NON_RETRIEVABLE_LOCAL_FILE;
        }
    }

    return File::Status::OK;
}

File::Status File::Impl::resolve()
{
    auto res = retrieveContents();

    if (res == File::Status::OK) {
        // The file contents could be retrieved so now check whether it's a
        // CellML file.

        mCellmlFile = CellmlFileManager::instance()->cellmlFile(mFileName);
        mSedmlFile = (mCellmlFile != nullptr) ?
                         nullptr :
                         SedmlFileManager::instance()->sedmlFile(mFileName);
        mCombineArchive = (mSedmlFile != nullptr) ?
                              nullptr :
                              CombineFileManager::instance()->combineArchive(mFileName);
    }

    return res;
}

File::File(const std::string &pFileNameOrUrl)
    : mPimpl(std::make_unique<Impl>())
{
    // By default, we assume that we are dealing with a local file, but it may
    // be in the form of a URL, i.e. starting with "file://".

    if (pFileNameOrUrl.find("file://") == 0) {
        // We are dealing with a URL representing a local file, so retrieve its
        // actual path.

        static const size_t SCHEME_LENGTH = 7;

        mPimpl->mFileName = pFileNameOrUrl;

        mPimpl->mFileName.erase(0, SCHEME_LENGTH);

        // On Windows, we also need to remove the leading "/" and replace all
        // the other "/"s with "\"s.

        static const auto WINDOWS_PATH_REGEX = std::regex("^/[A-Z]:/.*");

        std::smatch match;

        std::regex_search(mPimpl->mFileName, match, WINDOWS_PATH_REGEX);

        if (!match.empty()) {
            static const size_t SLASH_LENGTH = 1;
            static const auto FORWARD_SLASH_REGEX = std::regex("/");

            mPimpl->mFileName.erase(0, SLASH_LENGTH);

            mPimpl->mFileName = std::regex_replace(mPimpl->mFileName, FORWARD_SLASH_REGEX, "\\");
        }
    } else {
        // If we can get a URL from pFileNameOrUrl then it means that we are
        // dealing with a remote file otherwise we are dealing with a local
        // file.

        CURLU *url = curl_url();

        if (curl_url_set(url, CURLUPART_URL, pFileNameOrUrl.c_str(), 0) == CURLUE_OK) {
            mPimpl->mUrl = pFileNameOrUrl;
        } else {
            mPimpl->mFileName = pFileNameOrUrl;
        }

        curl_url_cleanup(url);
    }
}

FilePtr File::create(const std::string &pFileNameOrUrl)
{
    return std::shared_ptr<File> {new File {pFileNameOrUrl}};
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

File::Status File::resolve()
{
    return mPimpl->resolve();
}

} // namespace libOpenCOR
