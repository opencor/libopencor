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

typedef struct
{
    long seconds;
    long microeconds;
} TimeVal;

int getTimeOfDay(TimeVal &pTimeVal)
{
    // Based off https://stackoverflow.com/a/58162122.

    std::chrono::system_clock::duration duration = std::chrono::system_clock::now().time_since_epoch();
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

    pTimeVal.seconds = long(seconds.count());
    pTimeVal.microeconds = long(std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds).count());

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

#define ATTEMPTS_MIN (62 * 62 * 62)

#if ATTEMPTS_MIN < TMP_MAX
    uint64_t maxAttempts = TMP_MAX;
#else
    uint64_t maxAttempts = ATTEMPTS_MIN;
#endif

    // Get some more or less random data.

    static const char LETTERS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static auto res = (std::filesystem::temp_directory_path() / "libOpenCOR_XXXXXX.tmp").string();
    static const size_t XXXXXX_POS = res.size() - 6 - 4;

    TimeVal timeVal;

    getTimeOfDay(timeVal);

    uint64_t value = ((uint64_t)timeVal.microeconds << 16) ^ timeVal.seconds;

#ifdef _MSC_VER
    value ^= uint64_t(_getpid()) << 32;
#else
    value ^= uint64_t(getpid()) << 32;
#endif

    for (uint64_t attempt = 0; attempt < maxAttempts; value += 7777, ++attempt) {
        uint64_t v = value;

        for (int i = 0; i < 6; ++i) {
            res[XXXXXX_POS + i] = LETTERS[v % 62];
            v /= 62;
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

    reinterpret_cast<std::ofstream *>(pUserData)->write(reinterpret_cast<char *>(pData), realDataSize);

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
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(&file));

    if (curl_easy_perform(curl) == CURLE_OK) {
        static const int64_t HTTP_OK = 200;

        int64_t responseCode = 0;

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        if (responseCode != HTTP_OK) {
            remove(res.c_str());

            res = {};
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

std::tuple<char *, size_t> fileContents(const std::string &pFileName)
{
    std::ifstream file(pFileName, std::ios_base::binary);

    if (!file.is_open()) {
        return {};
    }

    auto size = std::filesystem::file_size(pFileName);
    auto contents = new char[size+1];

    file.read(contents, size);

    contents[size] = 0;

    return std::make_tuple(contents, size);
}

} // namespace libOpenCOR
