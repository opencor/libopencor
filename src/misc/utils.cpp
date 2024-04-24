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

#include "utils.h"

#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

#ifdef BUILDING_USING_MSVC
#    include <process.h>
#else
#    include <unistd.h>
#endif

#ifndef __EMSCRIPTEN__
#    include <curl/curl.h>
#endif

#ifdef BUILDING_USING_MSVC
#    include <codecvt>
#endif

namespace libOpenCOR {

bool fuzzyCompare(double pNb1, double pNb2)
{
    static constexpr double ONE_TRILLION = 1000000000000.0;

    return fabs(pNb1 - pNb2) * ONE_TRILLION <= fmin(fabs(pNb1), fabs(pNb2));
}

#ifdef BUILDING_USING_MSVC
std::string wideStringToString(const std::wstring &pString)
{
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(pString);
}

std::string forwardSlashPath(const std::string &pPath)
{
    static constexpr auto BACKSLASH = "\\\\";
    static const auto BACKSLASH_REGEX = std::regex(BACKSLASH);
    static constexpr auto FORWARD_SLASH = "/";

    return std::regex_replace(pPath, BACKSLASH_REGEX, FORWARD_SLASH);
}
#endif

std::filesystem::path stringToPath(const std::string &pString)
{
    return std::u8string {pString.begin(), pString.end()};
}

namespace {

#ifdef BUILDING_USING_MSVC
std::string canonicalFileName(const std::string &pFileName, bool pIsRemoteFile)
#else
std::string canonicalFileName(const std::string &pFileName)
#endif
{
    // Determine the canonical version of the file name.
    // Note: when building using Emscripten, std::filesystem::weakly_canonical() doesn't work as expected. For instance,
    //       if pFileName is equal to "/some/path/../.." then the call to std::filesystem::weakly_canonical() will
    //       generate an exception rather than return "/". So, we prepend a dummy folder to pFileName and then remove it
    //       from the result of std::filesystem::weakly_canonical().

    static constexpr auto FORWARD_SLASH = "/";

    auto currentPath = std::filesystem::current_path();

    std::filesystem::current_path(FORWARD_SLASH);

#if defined(BUILDING_USING_MSVC)
    auto res = wideStringToString(std::filesystem::weakly_canonical(stringToPath(pFileName)).wstring());
#elif defined(BUILDING_USING_GNU) || defined(BUILDING_USING_CLANG)
    auto res = std::filesystem::weakly_canonical(stringToPath(pFileName)).string();
#else
    static constexpr auto DUMMY_FOLDER = "/dummy";

    auto res = pFileName;

    res = DUMMY_FOLDER + std::string((res.find(FORWARD_SLASH) != 0) ? FORWARD_SLASH : "") + res;
    res = std::filesystem::weakly_canonical(stringToPath(res)).string();

    res.erase(0, strlen(DUMMY_FOLDER));
#endif

    std::filesystem::current_path(currentPath);

#if defined(BUILDING_USING_MSVC)
    // Replace "\"s with "/"s, if needed.

    if (pIsRemoteFile) {
        res = forwardSlashPath(pFileName);
    }
#elif defined(BUILDING_USING_CLANG) || defined(__EMSCRIPTEN__)
    // The file name may be relative rather than absolute, in which case we need to remove the forward slash that got
    // added (at the beginning of the file name) by std::filesystem::weakly_canonical().

    if (!pFileName.starts_with(FORWARD_SLASH)) {
        static const auto FORWARD_SLASH_LENGTH = strlen(FORWARD_SLASH);

        res.erase(0, FORWARD_SLASH_LENGTH);
    }
#endif

    // Return the canonical version of the file name.

    return res;
}

} // namespace

std::tuple<bool, std::string> retrieveFileInfo(const std::string &pFileNameOrUrl)
{
    // Check whether the given file name or URL is a local file name or a URL.
    // Note: a URL represents a local file when used with the "file" scheme.

#ifdef _WIN32
    static constexpr auto FILE_SCHEME = "file:///";
#else
    static constexpr auto FILE_SCHEME = "file://";
#endif
    static auto FILE_SCHEME_LENGTH = strlen(FILE_SCHEME);
    static constexpr auto HTTP_SCHEME = "http://";
    static auto HTTP_SCHEME_LENGTH = strlen(HTTP_SCHEME);
    static constexpr auto HTTPS_SCHEME = "https://";
    static auto HTTPS_SCHEME_LENGTH = strlen(HTTPS_SCHEME);

    auto res = pFileNameOrUrl;
    size_t schemeLength = 0;
    auto requiresHttpScheme = false;
    auto requiresHttpsScheme = false;

    if (pFileNameOrUrl.starts_with(FILE_SCHEME)) {
        schemeLength = FILE_SCHEME_LENGTH;
    } else if (pFileNameOrUrl.starts_with(HTTP_SCHEME)) {
        schemeLength = HTTP_SCHEME_LENGTH;
        requiresHttpScheme = true;
    } else if (pFileNameOrUrl.starts_with(HTTPS_SCHEME)) {
        schemeLength = HTTPS_SCHEME_LENGTH;
        requiresHttpsScheme = true;
    }

    res.erase(0, schemeLength);

    return {!requiresHttpScheme && !requiresHttpsScheme,
            (requiresHttpScheme ?
                 HTTP_SCHEME :
             requiresHttpsScheme ?
                 HTTPS_SCHEME :
                 "")
#ifdef BUILDING_USING_MSVC
                + canonicalFileName(res, requiresHttpScheme || requiresHttpsScheme)};
#else
                + canonicalFileName(res)};
#endif
}

namespace {

std::filesystem::path canonicalPath(const std::string &pPath)
{
    auto [isLocalPath, path] = retrieveFileInfo(pPath);

    return stringToPath(path);
}

} // namespace

std::string relativePath(const std::string &pPath, const std::string &pBasePath)
{
    return std::filesystem::relative(canonicalPath(pPath), canonicalPath(pBasePath)).string();
}

std::string urlPath(const std::string &pPath)
{
    auto [isLocalPath, path] = retrieveFileInfo(pPath);

    if (isLocalPath && stringToPath(path).is_absolute()) {
        static const std::string FILE_SCHEME = "file://";
#ifdef BUILDING_USING_MSVC
        static constexpr auto FORWARD_SLASH = "/";

        return FILE_SCHEME + FORWARD_SLASH + forwardSlashPath(path);
#else
        return FILE_SCHEME + path;
#endif
    }

    return path;
}

#ifndef __EMSCRIPTEN__
using TimeVal = struct
{
    uint64_t seconds;
    uint64_t microeconds;
};

namespace {

int getTimeOfDay(TimeVal &pTimeVal)
{
    // Based off https://stackoverflow.com/a/58162122.

    const auto duration = std::chrono::system_clock::now().time_since_epoch();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

    pTimeVal.seconds = static_cast<uint64_t>(seconds.count());
    pTimeVal.microeconds = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds).count());

    return 0;
}

std::filesystem::path uniqueFilePath()
{
    // This is based off glibc's __gen_tempname() method, which is used in std::tmpfile(). The only reason we don't use
    // std::tmpfile() is that it creates the file for us and as soon as we would be closing (though an automatic call
    // to std::fclose()), the file would get automatically deleted, which is not what we want. See
    // https://code.woboq.org/userspace/glibc/sysdeps/posix/tempname.c.html#__gen_tempname.

    // The number of times to attempt to generate a temporary file name.
    // Note: ATTEMPTS_MIN is equal to 62x62x62 where 62 is the number of characters in LETTERS.

#    ifndef CODE_COVERAGE_ENABLED
    static constexpr uint64_t ATTEMPTS_MIN = 238328U;
    static constexpr uint64_t MAX_ATTEMPTS = (ATTEMPTS_MIN < TMP_MAX) ? TMP_MAX : ATTEMPTS_MIN;
#    endif

    // Get some more or less random data.

    static const std::string LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const uint64_t LETTERS_SIZE = LETTERS.size();

    static auto testFile = (std::filesystem::temp_directory_path() / "libOpenCOR_XXXXXX.tmp").string();

    static const size_t XXXXXX_POS = testFile.size() - 6 - 4;
    static constexpr uint64_t MICROSECONDS_SHIFT = 16U;
    static constexpr uint64_t PID_SHIFT = 32U;
#    ifndef CODE_COVERAGE_ENABLED
    static constexpr uint64_t VALUE_SHIFT = 7777U;
#    endif
    static constexpr uint64_t XXXXXX_POS_SHIFT = 6U;

    TimeVal timeVal;

    getTimeOfDay(timeVal);

    auto value = (timeVal.microeconds << MICROSECONDS_SHIFT) ^ timeVal.seconds;

#    ifdef BUILDING_USING_MSVC
    value ^= static_cast<uint64_t>(_getpid()) << PID_SHIFT;
#    else
    value ^= static_cast<uint64_t>(getpid()) << PID_SHIFT;
#    endif

    std::string res;

#    ifndef CODE_COVERAGE_ENABLED
    for (uint64_t attempt = 0; attempt < MAX_ATTEMPTS; value += VALUE_SHIFT, ++attempt) {
#    endif
        uint64_t val = value;

        for (uint64_t i = 0; i < XXXXXX_POS_SHIFT; ++i) {
            testFile[XXXXXX_POS + i] = LETTERS[val % LETTERS_SIZE];
            val /= LETTERS_SIZE;
        }

#    ifndef CODE_COVERAGE_ENABLED
        if (!std::filesystem::exists(testFile)) {
#    endif
            res = testFile;

#    ifndef CODE_COVERAGE_ENABLED
            break;
        }
    }
#    endif

    return res;
}

size_t curlWriteFunction(void *pData, size_t pSize, size_t pDataSize, void *pUserData)
{
    const auto realDataSize = pSize * pDataSize;

    static_cast<std::ofstream *>(pUserData)->write(static_cast<char *>(pData), static_cast<std::streamsize>(realDataSize));

    return realDataSize;
}

} // namespace

std::tuple<bool, std::filesystem::path> downloadFile(const std::string &pUrl)
{
    auto filePath = uniqueFilePath();
    std::ofstream file(filePath, std::ios_base::binary);

#    ifndef CODE_COVERAGE_ENABLED
    if (!file.is_open()) {
        return {};
    }
#    endif

    curl_global_init(CURL_GLOBAL_DEFAULT);

    auto res = false;
    auto *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&file));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);

    if (curl_easy_perform(curl) == CURLE_OK) {
        static constexpr int64_t HTTP_OK = 200;

        int64_t responseCode = 0;

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        res = responseCode == HTTP_OK;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    file.close();

    if (res) {
        return {true, filePath};
    }

    std::filesystem::remove(filePath);

    return {};
}
#endif

#ifndef __EMSCRIPTEN__
UnsignedCharVector fileContents(const std::filesystem::path &pFilePath)
{
    // Retrieve and return the contents of the given file.

    std::ifstream file(pFilePath, std::ios_base::binary);

    if (!file.is_open()) {
        return {};
    }

    const auto fileSize = std::filesystem::file_size(pFilePath);
    UnsignedCharVector contents(fileSize);

    file.read(reinterpret_cast<char *>(&contents[0]), static_cast<std::streamsize>(fileSize)); // NOLINT

    return contents;
}
#endif

char *nlaSolverAddress(SolverNla *pNlaSolver)
{
    std::ostringstream oss;

    oss << pNlaSolver;

    auto str = oss.str();
    auto len = str.size();
    auto *res = new char[len + 1];

    std::copy(str.begin(), str.end(), res);

    res[len] = '\0'; // NOLINT

    return res;
}

std::string toString(int pNumber)
{
    std::ostringstream res;

    res << pNumber;

    return res.str();
}

std::string toString(size_t pNumber)
{
    std::ostringstream res;

    res << pNumber;

    return res.str();
}

std::string toString(double pNumber)
{
    std::ostringstream res;

    res << std::setprecision(std::numeric_limits<double>::digits10) << pNumber;

    return res.str();
}

std::string toString(const UnsignedCharVector &pBytes)
{
    return {reinterpret_cast<const char *>(pBytes.data()), pBytes.size()};
}

const xmlChar *toConstXmlCharPtr(const std::string &pString)
{
    return reinterpret_cast<const xmlChar *>(pString.c_str());
}

bool differentialModel(const CellmlFilePtr &pCellmlFile)
{
    auto type = pCellmlFile->type();

    return (type == libcellml::AnalyserModel::Type::ODE)
           || (type == libcellml::AnalyserModel::Type::DAE);
}

} // namespace libOpenCOR
