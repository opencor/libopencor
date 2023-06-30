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

#ifndef __EMSCRIPTEN__
#    include <curl/curl.h>
#endif

#include <libcellml>

namespace libOpenCOR {

File::Impl::Impl(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents)
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
    } else if ((pFileNameOrUrl.find("http://") == 0)
               || (pFileNameOrUrl.find("https://") == 0)) {
        // We are dealing with a URL representing a remote file.

        mUrl = pFileNameOrUrl;
    } else {
        // We are dealing with a local file.

        mFileName = pFileNameOrUrl;
    }

    // Check whether we have some contents and, if so, keep track of it and consider the file virtual.

    if (!pContents.empty()) {
        mContents = pContents;

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
        auto [res, contents] = fileContents(mFileName);

        if (res) {
            mContents = contents;
        } else {
            mType = Type::IRRETRIEVABLE_FILE;
        }

        mContentsRetrieved = true;
    }
}
#endif

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
    auto res = std::shared_ptr<File> {new File {pFileNameOrUrl}};

    res->pimpl()->checkType(res);

    return res;
}
#endif

FilePtr File::create(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents)
{
    auto res = std::shared_ptr<File> {new File {pFileNameOrUrl, pContents}};

    res->pimpl()->checkType(res);

    return res;
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

#ifdef __EMSCRIPTEN__
emscripten::val File::jsContents()
{
    return emscripten::val(emscripten::typed_memory_view(contents().size(), contents().data()));
}
#endif

std::vector<unsigned char> File::contents()
{
    return pimpl()->contents();
}

} // namespace libOpenCOR
