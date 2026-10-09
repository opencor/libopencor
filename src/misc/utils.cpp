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

#include "libopencor/issue.h"
#include "libopencor/logger.h"

#ifndef __EMSCRIPTEN__
#    include "curl/curl.h"
#endif

#ifdef BUILDING_USING_MSVC
#    include <process.h>
#else
#    include <unistd.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

#ifdef BUILDING_USING_MSVC
#    include <codecvt>
#endif

#ifdef ERROR
#    undef ERROR
#endif

#ifdef NAN
#    undef NAN
#endif

#ifdef min
#    undef min
#endif

namespace libOpenCOR {

#ifndef CODE_COVERAGE_ENABLED
void printIssues(const LoggerPtr &pLogger, const std::string &pHeader)
{
    std::cout << "---[ISSUES]---[BEGIN]\n";

    if (!pHeader.empty()) {
        std::cout << "---[" << pHeader << "]\n";
    }

    for (const auto &issue : pLogger->issues()) {
        std::cout << ((issue->type() == Issue::Type::ERROR) ? "ERROR" : "WARNING") << ": " << issue->description() << "\n";
    }

    std::cout << "---[ISSUES]---[END]\n";

    std::cout.flush();
}

void printHexDump(const UnsignedChars &pBytes)
{
    static constexpr auto BYTES_PER_LINE {16};
    static constexpr auto ADDRESS_WIDTH {8};
    static constexpr auto FIRST_ASCII_CHARACTER {32};
    static constexpr auto LAST_ASCII_CHARACTER {126};

    std::cout << "---[BYTES]---[BEGIN]\n";

    for (size_t i {0}; i < pBytes.size(); i += BYTES_PER_LINE) {
        // Print the offset.

        std::cout << std::hex << std::setfill('0') << std::setw(ADDRESS_WIDTH) << i << "  ";

        // Print hex bytes.

        for (size_t j {0}; j < BYTES_PER_LINE; ++j) {
            if (i + j < pBytes.size()) {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(pBytes[i + j]);
            } else {
                std::cout << "  ";
            }

            // Add an extra space after 8 bytes.

            if (j == ADDRESS_WIDTH - 1) {
                std::cout << "  ";
            } else {
                std::cout << " ";
            }
        }

        std::cout << " |";

        // Print the ASCII representation.

        const size_t bytesOnThisLine {(pBytes.size() - i < BYTES_PER_LINE) ? (pBytes.size() - i) : BYTES_PER_LINE};

        for (size_t j {0}; j < bytesOnThisLine; ++j) {
            auto byte {pBytes[i + j]};

            if (byte >= FIRST_ASCII_CHARACTER && byte <= LAST_ASCII_CHARACTER) {
                std::cout << static_cast<char>(byte);
            } else {
                std::cout << ".";
            }
        }

        for (size_t j {bytesOnThisLine}; j < BYTES_PER_LINE; ++j) {
            std::cout << " ";
        }

        std::cout << "|\n";
    }

    std::cout << std::dec;

    std::cout << "---[BYTES]---[END]\n";

    std::cout.flush();
}

void printArray(const std::string &pName, const Doubles &pDoubles)
{
    std::cout << "---[ARRAY]---[" << pName << "]---[BEGIN]\n";

    if (!pDoubles.empty()) {
        const auto arraySize {pDoubles.size()};
        const auto indexWidth {static_cast<int>(log10(static_cast<double>(arraySize - 1))) + 1};

        for (size_t i {0}; i < arraySize; ++i) {
            std::cout << "[" << std::setfill('0') << std::setw(indexWidth) << i << "] " << pDoubles[i] << "\n";
        }
    }

    std::cout << "---[ARRAY]---[" << pName << "]---[END]\n";

    std::cout.flush();
}
#endif

bool fuzzyCompare(double pNb1, double pNb2)
{
    static constexpr double ONE_TRILLION {1000000000000.0};

    return std::fabs(pNb1 - pNb2) * ONE_TRILLION <= std::fmin(std::fabs(pNb1), std::fabs(pNb2));
}

std::string encodeUrl(const std::string &pUrl)
{
    static constexpr std::string_view URL_SAFE_CHARACTERS {"!#$&'()*+,-./:;=?@_~"};
    static constexpr std::string_view HEX_DIGITS {"0123456789ABCDEF"};
    static constexpr unsigned int HEX_HIGH_NIBBLE_SHIFT {4U};
    static constexpr unsigned int HEX_LOW_NIBBLE_MASK {0x0FU};

    std::string res;

    res.reserve(pUrl.size() * 3); // NOLINT

    for (const auto urlChar : pUrl) {
        const auto unsignedChar {static_cast<unsigned char>(urlChar)};
        const auto encodedChar {static_cast<unsigned int>(unsignedChar)};

        if ((isalnum(unsignedChar) != 0) || (URL_SAFE_CHARACTERS.find(urlChar) != std::string::npos)) {
            res += urlChar;
        } else {
            res += '%';
            res += HEX_DIGITS[encodedChar >> HEX_HIGH_NIBBLE_SHIFT];
            res += HEX_DIGITS[encodedChar & HEX_LOW_NIBBLE_MASK];
        }
    }

    return res;
}

std::string decodeUrl(const std::string &pUrl)
{
    static constexpr auto BASE_16 {16};

    std::string res;

    for (size_t i {0}; i < pUrl.size(); ++i) {
        if ((pUrl[i] == '%') && (i + 2 < pUrl.size()) && (std::isxdigit(pUrl[i + 1]) != 0) && (std::isxdigit(pUrl[i + 2]) != 0)) {
            res += static_cast<char>(std::stoi(pUrl.substr(i + 1, 2), nullptr, BASE_16));

            i += 2;
        } else {
            res += pUrl[i];
        }
    }

    return res;
}

#ifdef BUILDING_USING_MSVC
std::string forwardSlashPath(const std::string &pPath)
{
    static constexpr auto BACKSLASH {"\\\\"};
    static const auto BACKSLASH_REGEX {std::regex(BACKSLASH)};
    static constexpr auto FORWARD_SLASH {"/"};

    return std::regex_replace(pPath, BACKSLASH_REGEX, FORWARD_SLASH);
}
#endif

std::filesystem::path stringToPath(const std::string &pString)
{
    return std::u8string {pString.begin(), pString.end()};
}

std::string pathToString(const std::filesystem::path &pPath)
{
    auto path {pPath};

#ifdef BUILDING_USING_MSVC
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(path.make_preferred().wstring());
#else
    return path.make_preferred().string();
#endif
}

std::string canonicalFileName(const std::string &pFileName)
{
    // Determine the canonical version of the file name.
    // Note #1: we must never change the current working directory here since it is shared by all the threads of the
    //          process.
    // Note #2: we use the std::error_code versions of the std::filesystem functions so that nothing gets thrown (e.g.,
    //          if the file name is too long or if the file gets deleted while we are dealing with it).
    // Note #3: when building using Emscripten, the file system is virtual and std::filesystem::weakly_canonical()
    //          doesn't work as expected (e.g., it throws an exception rather than return "/" for "/some/path/../.."),
    //          so we only ever normalise the file name lexically.

    // An empty file name stays empty (std::filesystem::canonical() would otherwise return the current working
    // directory).

    if (pFileName.empty()) {
        return {};
    }

    auto filePath {stringToPath(pFileName)};

#ifndef __EMSCRIPTEN__
    std::error_code errorCode;

    // If the file exists then use std::filesystem::canonical() since it resolves symbolic links, etc.
    // Note: this fails if the file doesn't exist.

    auto res {std::filesystem::canonical(filePath, errorCode)};

    if (!errorCode) {
        return pathToString(res);
    }

    // The file doesn't exist, so if its file name has a root directory (i.e., it is an absolute file name or, on
    // Windows, a file name relative to the root of the current drive) then use std::filesystem::weakly_canonical()
    // since it returns a file name that is as close to the canonical version as possible.

    if (filePath.has_root_directory()) {
        res = std::filesystem::weakly_canonical(filePath, errorCode);

        if (!errorCode) {
            return pathToString(res);
        }
    }
#endif

    // The file name is relative (or something went wrong), so normalise it lexically. Unlike
    // std::filesystem::weakly_canonical(), this doesn't depend on the current working directory, and it keeps leading
    // ".." components (e.g., "a/../../b" becomes "../b").

    return pathToString(filePath.lexically_normal());
}

std::string canonicalUrl(const std::string &pUrl)
{
    // Determine the canonical version of the URL, i.e. remove the "." and ".." segments from its path, as described in
    // RFC 3986 (https://www.rfc-editor.org/rfc/rfc3986#section-5.2.4), as well as its empty segments (e.g., "/a//b"
    // becomes "/a/b").
    // Note: a URL is never a local file name, so we must not access the file system (a local file that happens to have
    //       the same name as the host and path of the URL would otherwise be used) or use std::filesystem::path (its
    //       separators are platform specific).

    static constexpr auto FORWARD_SLASH {'/'};
    static constexpr auto DOT {"."};
    static constexpr auto DOT_DOT {".."};

#ifdef BUILDING_USING_MSVC
    auto url {forwardSlashPath(pUrl)};
#else
    const auto &url {pUrl};
#endif

    // Retrieve the scheme and authority (e.g., "https://example.com"), the path (e.g., "/some/path/file.txt"), and the
    // query and/or fragment (e.g., "?a=b#c") of the URL.

    static constexpr auto SCHEME_SEPARATOR {"://"};
    static const auto SCHEME_SEPARATOR_LENGTH {strlen(SCHEME_SEPARATOR)};

    auto schemeSeparatorPos {url.find(SCHEME_SEPARATOR)};
    auto authorityPos {(schemeSeparatorPos == std::string::npos) ? 0 : schemeSeparatorPos + SCHEME_SEPARATOR_LENGTH};
    auto queryOrFragmentPos {url.find_first_of("?#", authorityPos)};
    auto pathPos {std::min(url.find(FORWARD_SLASH, authorityPos), queryOrFragmentPos)};

    if ((pathPos == std::string::npos) || (url[pathPos] != FORWARD_SLASH)) {
        return url;
    }

    auto path {url.substr(pathPos, (queryOrFragmentPos == std::string::npos) ? std::string::npos : queryOrFragmentPos - pathPos)};

    // Remove the ".", "..", and empty segments from the path, keeping a trailing forward slash if the last segment is a
    // ".", "..", or empty segment (e.g., "/a/b/.." and "/a//" become "/a/").
    // Note: path starts with a forward slash, so the first segment that we retrieve is always empty and we skip it.

    std::vector<std::string> segments;
    size_t segmentPos {1};

    while (true) {
        auto nextSegmentPos {path.find(FORWARD_SLASH, segmentPos)};
        auto isLastSegment {nextSegmentPos == std::string::npos};
        auto segment {path.substr(segmentPos, isLastSegment ? std::string::npos : nextSegmentPos - segmentPos)};

        if (segment == DOT_DOT) {
            if (!segments.empty()) {
                segments.pop_back();
            }
        } else if ((segment != DOT) && !segment.empty()) {
            segments.push_back(segment);
        }

        if (isLastSegment) {
            if ((segment == DOT) || (segment == DOT_DOT) || segment.empty()) {
                segments.emplace_back();
            }

            break;
        }

        segmentPos = nextSegmentPos + 1;
    }

    std::string res {url.substr(0, pathPos)};

    for (const auto &segment : segments) {
        res += FORWARD_SLASH;
        res += segment;
    }

    if (queryOrFragmentPos != std::string::npos) {
        res += url.substr(queryOrFragmentPos);
    }

    return res;
}

std::tuple<bool, std::string> retrieveFileInfo(const std::string &pFileNameOrUrl, bool pCanonicalise)
{
    // Check whether the given file name or URL is a local file name or a URL.
    // Note: a URL represents a local file when used with the "file" scheme.

#ifdef BUILDING_ON_WINDOWS
    static constexpr auto FILE_SCHEME {"file:///"};
#else
    static constexpr auto FILE_SCHEME {"file://"};
#endif
    static auto FILE_SCHEME_LENGTH {strlen(FILE_SCHEME)};
    static constexpr auto HTTP_SCHEME {"http://"};
    static constexpr auto HTTPS_SCHEME {"https://"};

    if (pFileNameOrUrl.starts_with(HTTP_SCHEME) || pFileNameOrUrl.starts_with(HTTPS_SCHEME)) {
        return {false, pCanonicalise ? canonicalUrl(pFileNameOrUrl) : pFileNameOrUrl};
    }

    auto res {pFileNameOrUrl};

    if (res.starts_with(FILE_SCHEME)) {
        res.erase(0, FILE_SCHEME_LENGTH);
    }

    return {true, pCanonicalise ? canonicalFileName(res) : res};
}

namespace {

std::filesystem::path canonicalPath(const std::string &pPath)
{
    auto [isLocalPath, path] {retrieveFileInfo(pPath)};

    return stringToPath(path);
}

} // namespace

std::string relativePath(const std::string &pPath, const std::string &pBasePath)
{
    return pathToString(std::filesystem::relative(canonicalPath(pPath), canonicalPath(pBasePath)));
}

std::string urlPath(const std::string &pPath)
{
    auto [isLocalPath, path] {retrieveFileInfo(pPath)};

    if (isLocalPath && stringToPath(path).is_absolute()) {
        static const std::string FILE_SCHEME {"file://"};
#ifdef BUILDING_USING_MSVC
        static constexpr auto FORWARD_SLASH {"/"};

        return FILE_SCHEME + FORWARD_SLASH + forwardSlashPath(path);
#else
        return FILE_SCHEME + path;
#endif
    }

    return path;
}

#ifndef __EMSCRIPTEN__
namespace {

using TimeVal = struct
{
    uint64_t seconds;
    uint64_t microeconds;
};

void getTimeOfDay(TimeVal &pTimeVal)
{
    // Based off https://stackoverflow.com/a/58162122.

    const auto duration {std::chrono::system_clock::now().time_since_epoch()};
    const auto seconds {std::chrono::duration_cast<std::chrono::seconds>(duration)};

    pTimeVal.seconds = static_cast<uint64_t>(seconds.count());
    pTimeVal.microeconds = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds).count());
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
    static constexpr uint64_t ATTEMPTS_MIN {238328U};
    static constexpr uint64_t MAX_ATTEMPTS {(ATTEMPTS_MIN < TMP_MAX) ? TMP_MAX : ATTEMPTS_MIN};
#    endif

    // Get some more or less random data.

    static const std::string LETTERS {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    static const uint64_t LETTERS_SIZE {LETTERS.size()};

    static auto testFile {pathToString(std::filesystem::temp_directory_path() / "libOpenCOR_XXXXXX.tmp")};

    static const size_t XXXXXX_POS {testFile.size() - 6 - 4};
    static constexpr uint64_t MICROSECONDS_SHIFT {16U};
    static constexpr uint64_t PID_SHIFT {32U};
#    ifndef CODE_COVERAGE_ENABLED
    static constexpr uint64_t VALUE_SHIFT {7777U};
#    endif
    static constexpr uint64_t XXXXXX_POS_SHIFT {6U};

    TimeVal timeVal;

    getTimeOfDay(timeVal);

    auto value {(timeVal.microeconds << MICROSECONDS_SHIFT) ^ timeVal.seconds};

#    ifdef BUILDING_USING_MSVC
    value ^= static_cast<uint64_t>(_getpid()) << PID_SHIFT;
#    else
    value ^= static_cast<uint64_t>(getpid()) << PID_SHIFT;
#    endif

    std::string res;

#    ifndef CODE_COVERAGE_ENABLED
    for (uint64_t attempt {0}; attempt < MAX_ATTEMPTS; value += VALUE_SHIFT, ++attempt) {
#    endif
        uint64_t val {value};

        for (uint64_t i {0}; i < XXXXXX_POS_SHIFT; ++i) {
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

// Initialise libcurl when constructed and clean it up when destroyed (see downloadFile()).

class CurlGlobal
{
public:
    CurlGlobal()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~CurlGlobal()
    {
        curl_global_cleanup();
    }

    CurlGlobal(const CurlGlobal &pOther) = delete;
    CurlGlobal(CurlGlobal &&pOther) noexcept = delete;

    CurlGlobal &operator=(const CurlGlobal &pRhs) = delete;
    CurlGlobal &operator=(CurlGlobal &&pRhs) noexcept = delete;
};

size_t curlWriteFunction(char *pData, size_t pSize, size_t pDataSize, void *pUserData)
{
    const auto realDataSize {pSize * pDataSize};

    static_cast<std::ofstream *>(pUserData)->write(pData, static_cast<std::streamsize>(realDataSize));

    return realDataSize;
}

} // namespace

std::tuple<bool, std::filesystem::path> downloadFile(const std::string &pUrl)
{
    static const std::tuple<bool, std::filesystem::path> NO_TUPLE {false, std::filesystem::path()};

    auto filePath {uniqueFilePath()};
    std::ofstream file(filePath, std::ios_base::binary);

#    ifndef CODE_COVERAGE_ENABLED
    if (!file.is_open()) {
        return NO_TUPLE;
    }
#    endif

    // Make sure that libcurl is initialised.
    // Note: libcurl must only be initialised once (and only be cleaned up once no thread uses it anymore), hence we
    //       initialise it the first time a file is downloaded (in a thread-safe way, this being a function-local static
    //       variable) and clean it up when libOpenCOR is unloaded. After that, several threads can download a file at
    //       the same time since each of them uses its own libcurl handle and our OpenSSL is thread-safe.

    static const CurlGlobal curlGlobal;

    auto res {false};
    auto *curl {curl_easy_init()};

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_URL, encodeUrl(pUrl).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&file));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);

    if (curl_easy_perform(curl) == CURLE_OK) {
        static constexpr int64_t HTTP_OK {200};

        int64_t responseCode {0};

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        res = responseCode == HTTP_OK;
    }

    curl_easy_cleanup(curl);

    file.close();

    if (res) {
        return {true, filePath};
    }

    std::filesystem::remove(filePath);

    return NO_TUPLE;
}

UnsignedChars fileContents(const std::filesystem::path &pFilePath)
{
    static const UnsignedChars NO_UNSIGNED_CHARS;

    // Retrieve and return the contents of the given file.

    std::ifstream file(pFilePath, std::ios_base::binary);

    if (!file.is_open()) {
        return NO_UNSIGNED_CHARS;
    }

    const auto fileSize {std::filesystem::file_size(pFilePath)};
    UnsignedChars contents;

    contents.resize(fileSize);

    file.read(reinterpret_cast<char *>(contents.data()), static_cast<std::streamsize>(fileSize)); // NOLINT

    return contents;
}
#endif

bool isInfOrNan(double pNumber)
{
    return std::isinf(pNumber) || std::isnan(pNumber);
}

bool toBool(const std::string &pString)
{
    return pString == "true";
}

std::string toString(bool pBoolean)
{
    return pBoolean ? "true" : "false";
}

bool isInt(const std::string &pString)
{
    static const auto INT_REGEX {std::regex("^([-+]?[1-9][0-9]*([eE][+]?[0-9]+)?|0)$")};

    if (std::regex_match(pString, INT_REGEX)) {
        std::istringstream iss(pString);
        int res {0};

        iss >> res;

        return !iss.fail();
    }

    return false;
}

int toInt(const std::string &pString)
{
    std::istringstream iss(pString);
    int res {0};

    iss >> res;

    return res;
}

std::string toString(int pNumber)
{
    return std::format("{}", pNumber);
}

std::string toString(size_t pNumber)
{
    return std::format("{}", pNumber);
}

bool isDouble(const std::string &pString)
{
    static const auto DOUBLE_REGEX {std::regex("^[+-]?[0-9]*\\.?[0-9]+([eE][+-]?[0-9]+)?$")};

    return std::regex_match(pString, DOUBLE_REGEX);
}

double toDouble(const std::string &pString)
{
    if (!isDouble(pString)) {
        return NAN;
    }

    std::istringstream iss(pString);
    double res {NAN};

    iss >> res;

    return iss.fail() ? NAN : res;
}

std::string toString(double pNumber)
{
    std::ostringstream res;

    res << std::setprecision(std::numeric_limits<double>::digits10) << pNumber;

    return res.str();
}

std::string toString(const UnsignedChars &pBytes)
{
    return {reinterpret_cast<const char *>(pBytes.data()), pBytes.size()};
}

const xmlChar *toConstXmlCharPtr(const std::string &pString)
{
    return reinterpret_cast<const xmlChar *>(pString.c_str());
}

libcellml::ComponentPtr owningComponent(const libcellml::VariablePtr &pVariable)
{
    return std::dynamic_pointer_cast<libcellml::Component>(pVariable->parent());
}

std::string name(const std::string &pComponentName, const std::string &pVariableName)
{
    return pComponentName + "/" + pVariableName;
}

std::string name(const libcellml::VariablePtr &pVariable)
{
    return name(owningComponent(pVariable)->name(), pVariable->name());
}

} // namespace libOpenCOR
