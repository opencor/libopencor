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

    if ((pContents != nullptr) && (pSize != 0)) {
        mIsVirtual = true;

        mContents = new char[pSize] {};
        mSize = pSize;

        std::copy_n(pContents, pSize, mContents);
    }
}

File::Impl::~Impl()
{
    reset();
}

void File::Impl::reset()
{
    mType = Type::UNRESOLVED;

    if (!mUrl.empty() && !mFileName.empty()) {
        remove(mFileName.c_str()); // NOLINT(cert-err33-c)

        mFileName = {};
    }

    delete mContents;

    mContents = nullptr;

    delete mCellmlFile;
    delete mSedmlFile;
    delete mCombineArchive;

    mCellmlFile = nullptr;
    mSedmlFile = nullptr;
    mCombineArchive = nullptr;
}

File::Status File::Impl::resolve()
{
    reset();

#ifndef __EMSCRIPTEN__
    // Download a local copy of the remote file, if needed.

    if (mFileName.empty()) {
        mFileName = downloadFile(mUrl);
    }

    // Make sure that the (local) file exists.

    if (mFileName.empty() || !std::filesystem::exists(mFileName.c_str())) {
        return File::Status::NON_RETRIEVABLE_FILE;
    }

    // Check whether the local/remote file is a CellML file, a SED-ML file, a COMBINE archive, or an unknown file.

    if (Support::isCellmlFile(mFileName)) {
        mType = Type::CELLML_FILE;
    } else if (Support::isSedmlFile(mFileName)) {
        mType = Type::SEDML_FILE;
    } else if (Support::isCombineArchive(mFileName)) {
        mType = Type::COMBINE_ARCHIVE;
    } else {
        mType = Type::UNKNOWN_FILE;
    }
#endif

    return File::Status::OK;
}

File::Status File::Impl::instantiate()
{
    // Make sure that the file has been resolved.

    if (mType == Type::UNRESOLVED) {
        auto res = resolve();

        if (res != File::Status::OK) {
            return res;
        }
    }

    // Make sure that we are dealing with a CellML file.

    if (mType != Type::CELLML_FILE) {
        return File::Status::NON_INSTANTIABLE_FILE;
    }

#ifndef __EMSCRIPTEN__
    // Generate and compile the C code associated with the CellML file.

    auto [contents, size] = fileContents(mFileName);
    auto *rawContents = contents.get();
    auto parser = libcellml::Parser::create();
    auto model = parser->parseModel(rawContents);

    if (parser->errorCount() != 0) {
        // We couldn't parse the file contents as a CellML 2.0 file, so this means that we are dealing with a CellML 1.x
        // file, hence we should use a non-strict parser instead.
        // Note: we don't need to check whether the parsing was fine since it's a prequisite to a file being considered
        //       a CellML file. We just didn't know whether it was a CellML 1.x file or a CellML 2.0 file. So, now that
        //       we know that it's not a CellML 2.0 file, it means that it can only be a CellML 1.x file.

        parser = libcellml::Parser::create(false);
        model = parser->parseModel(rawContents);
    }

    auto analyser = libcellml::Analyser::create();

    analyser->analyseModel(model);

    if (analyser->errorCount() != 0) {
        return File::Status::NON_INSTANTIABLE_FILE;
    }

    auto generator = libcellml::Generator::create();
    auto generatorProfile = libcellml::GeneratorProfile::create();

    generatorProfile->setOriginCommentString("");
    generatorProfile->setImplementationHeaderString("");
    generatorProfile->setImplementationVersionString("");
    generatorProfile->setImplementationStateCountString("");
    generatorProfile->setImplementationVariableCountString("");
    generatorProfile->setImplementationLibcellmlVersionString("");
    generatorProfile->setImplementationVoiInfoString("");
    generatorProfile->setImplementationStateInfoString("");
    generatorProfile->setImplementationVariableInfoString("");
    generatorProfile->setImplementationCreateStatesArrayMethodString("");
    generatorProfile->setImplementationCreateVariablesArrayMethodString("");
    generatorProfile->setImplementationDeleteArrayMethodString("");

    generator->setModel(analyser->model());
    generator->setProfile(generatorProfile);

    auto compiler = std::make_unique<Compiler>();

#    ifdef COVERAGE_ENABLED
    compiler->compile(generator->implementationCode());
#    else
    if (!compiler->compile(generator->implementationCode())) {
        return File::Status::NON_INSTANTIABLE_FILE;
    }
#    endif
#endif

    return File::Status::OK;
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
    return std::shared_ptr<File> {new File {pFileNameOrUrl}};
}
#endif

FilePtr File::create(const std::string &pFileNameOrUrl, const char *pContents, size_t pSize)
{
    return std::shared_ptr<File> {new File {pFileNameOrUrl, pContents, pSize}};
}

bool File::isVirtual() const
{
    return mPimpl->mIsVirtual;
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

File::Status File::instantiate()
{
    return mPimpl->instantiate();
}

} // namespace libOpenCOR
