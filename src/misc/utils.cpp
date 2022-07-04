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

#ifdef _MSC_VER
#    include <process.h>
#else
#    include <unistd.h>
#endif

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
    return std::filesystem::canonical(pPath).string();
}

} // namespace libOpenCOR
