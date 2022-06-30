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

#include "file_p.h"
#include "utils.h"

#include <fstream>
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
    mContentsSize = 0;
}

static size_t curlWriteFunction(void *pData, size_t pSize, size_t pDataSize, void *pUserData)
{
    size_t realDataSize = pSize * pDataSize;

    reinterpret_cast<std::ofstream *>(pUserData)->write(reinterpret_cast<char *>(pData), realDataSize);

    return realDataSize;
}

File::Status File::Impl::retrieveContents()
{
    reset();

    // Retrieve the contents of the file based on whether it's a local file or a
    // remote file.

    auto res = File::Status::OK;

    if (mFileName.empty()) {
        // We assume that we will always be able to open a temporary file.

        mFileName = temporaryFileName();

        std::ofstream file(mFileName, std::ios_base::binary);

        assert(file.is_open());

        curl_global_init(CURL_GLOBAL_DEFAULT);

        auto *curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, mUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(&file));

        if (curl_easy_perform(curl) == CURLE_OK) {
            static const int64_t HTTP_OK = 200;
            static const int64_t HTTP_NOT_FOUND = 404;

            int64_t responseCode = 0;

            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode); // NOLINT(cppcoreguidelines-pro-type-vararg, hicpp-vararg)

            if (responseCode != HTTP_OK) {
                reset();

                res = (responseCode == HTTP_NOT_FOUND) ?
                          File::Status::NON_EXISTING_REMOTE_FILE :
                          File::Status::NON_DOWNLOADABLE_REMOTE_FILE;
            }
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    // Retrieve the contents of the local file or the local copy of a remote
    // file.

    if (!mFileName.empty()) {
        std::ifstream file(mFileName, std::ios_base::binary);

        if (!file.is_open()) {
            // We assume that we will always be able to open the local copy of
            // a remote file. So, if we can't open the file it's because we are
            // dealing with a non-existing local file.

            assert(mUrl.empty());

            res = File::Status::NON_EXISTING_LOCAL_FILE;
        } else {
            mContentsSize = std::filesystem::file_size(mFileName);

            mContents = new char[mContentsSize];

            file.read(mContents, mContentsSize);
        }
    }

    return res;
}

File::Status File::Impl::resolve()
{
    auto res = retrieveContents();

    if (res == File::Status::OK) {
        // The file contents could be retrieved so now check whether it's a
        // CellML file.

        //---GRY--- TO BE DONE...
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
