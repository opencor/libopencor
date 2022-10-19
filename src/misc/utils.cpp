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
#include <filesystem>
#include <fstream>

#ifdef _MSC_VER
#    include <process.h>
#else
#    include <unistd.h>
#endif

#include <curl/curl.h>

namespace libOpenCOR {

bool fuzzyCompare(double pNb1, double pNb2)
{
    static constexpr double ONE_TRILLION = 1000000000000.0;

    return fabs(pNb1 - pNb2) * ONE_TRILLION <= fmin(fabs(pNb1), fabs(pNb2));
}

using TimeVal = struct
{
    uint64_t seconds;
    uint64_t microeconds;
};

static int getTimeOfDay(TimeVal &pTimeVal)
{
    // Based off https://stackoverflow.com/a/58162122.

    const std::chrono::system_clock::duration duration = std::chrono::system_clock::now().time_since_epoch();
    const std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

    pTimeVal.seconds = static_cast<uint64_t>(seconds.count());
    pTimeVal.microeconds = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds).count());

    return 0;
}

std::string uniqueFileName()
{
    // This is based off glibc's __gen_tempname() method, which used in
    // std::tmpfile(). The only reason we don't use std::tmpfile() is that it
    // creates the file for us and as soon as we would be closing std::fclose(),
    // the file would get automatically deleted, which is not what we want. See
    // https://code.woboq.org/userspace/glibc/sysdeps/posix/tempname.c.html#__gen_tempname.

    // The number of times to attempt to generate a temporary file name.
    // Note: ATTEMPTS_MIN is equal to 62x62x62 where 62 is the number of
    //       characters in LETTERS.

#ifndef COVERAGE_ENABLED
    static const uint64_t ATTEMPTS_MIN = 238328U;

    uint64_t maxAttempts = (ATTEMPTS_MIN < TMP_MAX) ? TMP_MAX : ATTEMPTS_MIN;
#endif

    // Get some more or less random data.

    static const std::string LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const uint64_t LETTERS_SIZE = LETTERS.size();
    static auto testFile = (std::filesystem::temp_directory_path() / "libOpenCOR_XXXXXX.tmp").string();
    static const size_t XXXXXX_POS = testFile.size() - 6 - 4;
    static const uint64_t MICROSECONDS_SHIFT = 16U;
    static const uint64_t PID_SHIFT = 32U;
#ifndef COVERAGE_ENABLED
    static const uint64_t VALUE_SHIFT = 7777U;
#endif
    static const uint64_t XXXXXX_POS_SHIFT = 6U;

    TimeVal timeVal;

    getTimeOfDay(timeVal);

    auto value = (timeVal.microeconds << MICROSECONDS_SHIFT) ^ timeVal.seconds;

#ifdef _MSC_VER
    value ^= static_cast<uint64_t>(_getpid()) << PID_SHIFT;
#else
    value ^= static_cast<uint64_t>(getpid()) << PID_SHIFT;
#endif

    std::string res;

#ifndef COVERAGE_ENABLED
    for (uint64_t attempt = 0; attempt < maxAttempts; value += VALUE_SHIFT, ++attempt) {
#endif
        uint64_t val = value;

        for (uint64_t i = 0; i < XXXXXX_POS_SHIFT; ++i) {
            testFile[XXXXXX_POS + i] = LETTERS[val % LETTERS_SIZE];
            val /= LETTERS_SIZE;
        }

#ifndef COVERAGE_ENABLED
        if (!std::filesystem::exists(testFile)) {
#endif
            res = testFile;

#ifndef COVERAGE_ENABLED
            break;
        }
    }
#endif

    return res;
}

static size_t curlWriteFunction(void *pData, size_t pSize, size_t pDataSize, void *pUserData)
{
    const size_t realDataSize = pSize * pDataSize;

    reinterpret_cast<std::ofstream *>(pUserData)->write(reinterpret_cast<char *>(pData), static_cast<std::streamsize>(realDataSize)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

    return realDataSize;
}

std::string downloadFile(const std::string &pUrl)
{
    auto fileName = uniqueFileName();
    std::ofstream file(fileName, std::ios_base::binary);

#ifndef COVERAGE_ENABLED
    if (!file.is_open()) {
        return {};
    }
#endif

    curl_global_init(CURL_GLOBAL_DEFAULT);

    auto res = false;
    auto *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(&file)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);

#ifndef COVERAGE_ENABLED
    if (curl_easy_perform(curl) == CURLE_OK) {
#else
    curl_easy_perform(curl);
#endif
        static const int64_t HTTP_OK = 200;

        int64_t responseCode = 0;

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        res = responseCode == HTTP_OK;
#ifndef COVERAGE_ENABLED
    }
#endif

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (res) {
        return fileName;
    }

    remove(fileName.c_str()); // NOLINT(cert-err33-c)

    return {};
}

std::tuple<std::shared_ptr<char[]>, size_t> fileContents(const std::string &pFileName) // NOLINT(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
{
    std::ifstream file(pFileName, std::ios_base::binary);

#ifndef COVERAGE_ENABLED
    if (!file.is_open()) {
        return {};
    }
#endif

    auto size = std::filesystem::file_size(pFileName);
    const std::shared_ptr<char[]> contents(new char[size + 1]); // NOLINT(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

    file.read(contents.get(), static_cast<std::streamsize>(size));

    contents[static_cast<ptrdiff_t>(size)] = 0;

    return std::make_tuple(contents, size);
}

} // namespace libOpenCOR
