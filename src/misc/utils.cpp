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

#include <cassert>
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

    std::chrono::system_clock::duration duration = std::chrono::system_clock::now().time_since_epoch();
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

    pTimeVal.seconds = static_cast<uint64_t>(seconds.count());
    pTimeVal.microeconds = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds).count());

    return 0;
}

std::string temporaryFileName()
{
    // This is based off glibc's __gen_tempname() method, which used in
    // std::tmpfile(). The only reason we don't use std::tmpfile() is that it
    // creates the file for us and as soon as we would be closing std::fclose(),
    // the file would get automatically deleted, which is not what we want. See
    // https://code.woboq.org/userspace/glibc/sysdeps/posix/tempname.c.html#__gen_tempname.

    // The number of times to attempt to generate a temporary file name.
    // Note: ATTEMPTS_MIN is equal to 62x62x62 where 62 is the number of
    //       characters in LETTERS.

    static const uint64_t ATTEMPTS_MIN = 238328U;

    uint64_t maxAttempts = (ATTEMPTS_MIN < TMP_MAX) ? TMP_MAX : ATTEMPTS_MIN;

    // Get some more or less random data.

    static const std::string LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static auto res = (std::filesystem::temp_directory_path() / "libOpenCOR_XXXXXX.tmp").string();
    static const size_t XXXXXX_POS = res.size() - 6 - 4;
    static const uint64_t MICROSECONDS_SHIFT = 16U;
    static const uint64_t PID_SHIFT = 32U;
    static const uint64_t VALUE_SHIFT = 7777U;
    static const uint64_t XXXXXX_POS_SHIFT = 6U;

    TimeVal timeVal;

    getTimeOfDay(timeVal);

    auto value = (timeVal.microeconds << MICROSECONDS_SHIFT) ^ timeVal.seconds;

#ifdef _MSC_VER
    value ^= static_cast<uint64_t>(_getpid()) << PID_SHIFT;
#else
    value ^= static_cast<uint64_t>(getpid()) << PID_SHIFT;
#endif

    for (uint64_t attempt = 0; attempt < maxAttempts; value += VALUE_SHIFT, ++attempt) {
        uint64_t val = value;

        for (uint64_t i = 0; i < XXXXXX_POS_SHIFT; ++i) {
            res[XXXXXX_POS + i] = LETTERS[val % LETTERS.size()];
            val /= LETTERS.size();
        }

        if (!std::filesystem::exists(res)) {
            return res;
        }
    }

    return {};
}

std::string canonicalPath(const std::string &pPath)
{
    std::error_code errorCode;
    auto res = std::filesystem::canonical(pPath, errorCode);

    if (errorCode.value() == 0) {
        return res.string();
    }

    return pPath;
}

static size_t curlWriteFunction(void *pData, size_t pSize, size_t pDataSize, void *pUserData)
{
    size_t realDataSize = pSize * pDataSize;

    reinterpret_cast<std::ofstream *>(pUserData)->write(reinterpret_cast<char *>(pData), static_cast<std::streamsize>(realDataSize)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

    return realDataSize;
}

std::string downloadFile(const std::string &pUrl)
{
    // Note: we assume that we are always able to open a temporary file. This
    //       should not only always be the case, but there is also no way to
    //       reproducibly test the case where we wouldn't be able to open a
    //       temporary file. Still, we use an assert to make sure that we are
    //       indeed able to open it.

    auto res = temporaryFileName();

    std::ofstream file(res, std::ios_base::binary);

    assert(file.is_open());

    curl_global_init(CURL_GLOBAL_DEFAULT);

    auto *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(&file)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

    if (curl_easy_perform(curl) == CURLE_OK) {
        static const int64_t HTTP_OK = 200;

        int64_t responseCode = 0;

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        if (responseCode != HTTP_OK) {
            remove(res.c_str()); // NOLINT(cert-err33-c)

            res = {};
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

std::tuple<std::shared_ptr<char[]>, size_t> fileContents(const std::string &pFileName) // NOLINT(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
{
    std::ifstream file(pFileName, std::ios_base::binary);

    if (!file.is_open()) {
        return {};
    }

    auto size = std::filesystem::file_size(pFileName);
    std::shared_ptr<char[]> contents(new char[size + 1]); // NOLINT(cppcoreguidelines-avoid-c-arrays, hicpp-avoid-c-arrays, modernize-avoid-c-arrays)

    file.read(contents.get(), static_cast<std::streamsize>(size));

    contents[static_cast<ptrdiff_t>(size)] = 0;

    return std::make_tuple(contents, size);
}

} // namespace libOpenCOR
