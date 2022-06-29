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

#include <regex>

#include <curl/curl.h>

namespace libOpenCOR {

File::File(const std::string &pFileNameOrUrl)
    : Logger(new Impl())
{
    // By default, we assume that we are dealing with a local file, but it may
    // be in the form of a URL, i.e. starting with "file://".

    auto *pimpl = File::pimpl();

    if (pFileNameOrUrl.find("file://") == 0) {
        // We are dealing with a URL representing a local file, so retrieve its
        // actual path.

        static const size_t SCHEME_LENGTH = 7;

        pimpl->mFileName = pFileNameOrUrl;

        pimpl->mFileName.erase(0, SCHEME_LENGTH);

        // On Windows, we also need to remove the leading "/" and replace all
        // the other "/"s with "\"s.

        static const auto WINDOWS_PATH_REGEX = std::regex("^/[A-Z]:/.*");

        std::smatch match;

        std::regex_search(pimpl->mFileName, match, WINDOWS_PATH_REGEX);

        if (!match.empty()) {
            static const size_t SLASH_LENGTH = 1;
            static const auto FORWARD_SLASH_REGEX = std::regex("/");

            pimpl->mFileName.erase(0, SLASH_LENGTH);

            pimpl->mFileName = std::regex_replace(pimpl->mFileName, FORWARD_SLASH_REGEX, "\\");
        }
    } else {
        // If we can get a URL from pFileNameOrUrl then it means that we are
        // dealing with a remote file otherwise we are dealing with a local
        // file.

        CURLU *url = curl_url();

        if (curl_url_set(url, CURLUPART_URL, pFileNameOrUrl.c_str(), 0) == CURLUE_OK) {
            pimpl->mUrl = pFileNameOrUrl;
        } else {
            pimpl->mFileName = pFileNameOrUrl;
        }

        curl_url_cleanup(url);
    }
}

File::~File()
{
    delete pimpl();
}

File::Impl *File::pimpl()
{
    return reinterpret_cast<File::Impl *>(Logger::pimpl());
}

const File::Impl *File::pimpl() const
{
    return reinterpret_cast<File::Impl const *>(Logger::pimpl());
}

FilePtr File::create(const std::string &pFileNameOrUrl)
{
    return std::shared_ptr<File> {new File {pFileNameOrUrl}};
}

File::Type File::type() const
{
    return pimpl()->mType;
}

std::string File::fileName() const
{
    return pimpl()->mFileName;
}

std::string File::url() const
{
    return pimpl()->mUrl;
}

} // namespace libOpenCOR
