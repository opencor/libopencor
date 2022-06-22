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

#include <regex>

#include <curl/curl.h>

namespace libOpenCOR {

/**
 * Private implementation.
 */

struct File::Impl
{
    Type mType = Type::LOCAL;
    std::string mFileName;
    std::string mUrl;
};

/**
 * Public implementation.
 */

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

        static const auto WINDOWS_PATH_REGEX = std::regex("/[A-Z]:/.*");

        std::smatch match;

        std::regex_search(mPimpl->mFileName, match, WINDOWS_PATH_REGEX);

        if ((match.size() == 1) && (match.position(0) == 0)) {
            static const size_t SLASH_LENGTH = 1;
            static const auto FORWARD_SLASH_REGEX = std::regex("/");

            mPimpl->mFileName.erase(0, SLASH_LENGTH);

            mPimpl->mFileName = std::regex_replace(mPimpl->mFileName, FORWARD_SLASH_REGEX, "\\");
        }
    } else {
        // We are dealing with either a local file name or a URL.

        CURLU *url = curl_url();

        if (curl_url_set(url, CURLUPART_URL, pFileNameOrUrl.c_str(), 0) == CURLUE_OK) {
            // We were able to get a URL, which means that we are dealing with a
            // remote file.

            mPimpl->mType = Type::REMOTE;
            mPimpl->mUrl = pFileNameOrUrl;
        } else {
            // We didn't get a URL, which means that we are dealing with a local
            // file.

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

} // namespace libOpenCOR
