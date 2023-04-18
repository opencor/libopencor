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

#include "version.h"

#include "clangbegin.h"
#include "clang/Basic/Version.h"
#include "clangend.h"

#include "curl/curlver.h"

#include "libcellml/version.h"

#include "libcombinebegin.h"
#include "omex/common/libcombine-version.h"
#include "libcombineend.h"

#include "libopencor/version.h"

#include "libsedmlbegin.h"
#include "sedml/common/libsedml-version.h"
#include "libsedmlend.h"

#include "llvmbegin.h"
#include "llvm/Config/llvm-config.h"
#include "llvmend.h"

#include "sundialsbegin.h"
#include "sundials/sundials_version.h"
#include "sundialsend.h"

#include <array>
#include <cmath>

namespace libOpenCOR {

static constexpr unsigned int MAJOR_10 = 1048576;
static constexpr unsigned int MAJOR_01 = 65536;
static constexpr unsigned int MINOR_10 = 4096;
static constexpr unsigned int MINOR_01 = 256;
static constexpr unsigned int PATCH_10 = 16;
static constexpr unsigned int PATCH_01 = 1;

inline unsigned int firstDigit(unsigned int pTwoDigitNumber)
{
    static constexpr double ONE_TENTH = 0.1;

    return static_cast<unsigned int>(floor(ONE_TENTH * pTwoDigitNumber));
}

inline unsigned int secondDigit(unsigned int pTwoDigitNumber)
{
    static constexpr unsigned int TEN = 10;

    return pTwoDigitNumber % TEN;
}

unsigned int version()
{
    return LIBOPENCOR_VERSION;
}

std::string versionString()
{
    return LIBOPENCOR_VERSION_STRING;
}

unsigned int clangVersion()
{
    return static_cast<unsigned int>(MAJOR_10 * firstDigit(CLANG_VERSION_MAJOR) + MAJOR_01 * secondDigit(CLANG_VERSION_MAJOR)
                                     + MINOR_10 * firstDigit(CLANG_VERSION_MINOR) + MINOR_01 * secondDigit(CLANG_VERSION_MINOR)
                                     + PATCH_10 * firstDigit(CLANG_VERSION_PATCHLEVEL) + PATCH_01 * secondDigit(CLANG_VERSION_PATCHLEVEL));
}

std::string clangVersionString()
{
    return CLANG_VERSION_STRING;
}

unsigned int libcellmlVersion()
{
    return libcellml::version();
}

std::string libcellmlVersionString()
{
    return libcellml::versionString();
}

unsigned int libcombineVersion()
{
    return static_cast<unsigned int>(libcombine::getLibCombineVersion());
}

std::string libcombineVersionString()
{
    return libcombine::getLibCombineDottedVersion();
}

unsigned int libcurlVersion()
{
    return LIBCURL_VERSION_NUM;
}

std::string libcurlVersionString()
{
    return LIBCURL_VERSION;
}

unsigned int libsedmlVersion()
{
    return static_cast<unsigned int>(libsedml::getLibSEDMLVersion());
}

std::string libsedmlVersionString()
{
    return libsedml::getLibSEDMLDottedVersion();
}

unsigned int llvmVersion()
{
    return static_cast<unsigned int>(MAJOR_10 * firstDigit(LLVM_VERSION_MAJOR) + MAJOR_01 * secondDigit(LLVM_VERSION_MAJOR)
                                     + MINOR_10 * firstDigit(LLVM_VERSION_MINOR) + MINOR_01 * secondDigit(LLVM_VERSION_MINOR)
                                     + PATCH_10 * firstDigit(LLVM_VERSION_PATCH) + PATCH_01 * secondDigit(LLVM_VERSION_PATCH));
}

std::string llvmVersionString()
{
    return LLVM_VERSION_STRING;
}

unsigned int sundialsVersion()
{
    static constexpr int MAJOR = 65536;
    static constexpr int MINOR = 256;
    static constexpr int LABEL_SIZE = 10;

    int major = 0;
    int minor = 0;
    int patch = 0;
    std::array<char, LABEL_SIZE> label {""};

    SUNDIALSGetVersionNumber(&major, &minor, &patch, label.data(), LABEL_SIZE);

    return static_cast<unsigned int>(MAJOR * major + MINOR * minor + patch);
}

std::string sundialsVersionString()
{
    static constexpr int VERSION_SIZE = 25;

    std::array<char, VERSION_SIZE> version {""};

    SUNDIALSGetVersion(version.data(), VERSION_SIZE);

    std::string res {std::begin(version), std::end(version)};

    res.erase(res.find_last_not_of('\0') + 1, std::string::npos);

    return res;
}

} // namespace libOpenCOR
