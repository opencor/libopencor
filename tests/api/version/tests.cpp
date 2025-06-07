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

#include "tests/utils.h"

#include <array>
#include <chrono>
#include <libopencor>

TEST(VersionTest, libOpenCOR)
{
    static const auto VERSION_MAJOR = 0;
    static const auto VERSION_PATCH = 0;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm {};

#ifdef BUILDING_ON_WINDOWS
    localtime_s(&tm, &now); // NOLINT
#else
    localtime_r(&now, &tm); // NOLINT
#endif

    static const auto NINETEEN_HUNDRED = 1900;
    static const auto ONE = 1;

    const auto year = NINETEEN_HUNDRED + tm.tm_year;
    const auto month = ONE + tm.tm_mon;
    const auto day = tm.tm_mday;

    static const uint64_t TEN_BILLION = 10000000000;
    static const uint64_t TEN_THOUSAND = 10000;
    static const uint64_t HUNDRED = 100;
    static const uint64_t TEN = 10;

    uint64_t version = 0;
    auto number = TEN_BILLION * VERSION_MAJOR + HUNDRED * (TEN_THOUSAND * static_cast<uint64_t>(year) + HUNDRED * static_cast<uint64_t>(month) + static_cast<uint64_t>(day)) + VERSION_PATCH;

    for (int i = 0; number != 0; i += 4) {
        version |= (number % TEN) << i; // NOLINT
        number /= TEN;
    }

    static const int INT_TEN = 10;

    auto versionString = std::to_string(VERSION_MAJOR)
                         + "."
                         + std::to_string(year) + ((month < INT_TEN) ? "0" : "") + std::to_string(month) + ((day < INT_TEN) ? "0" : "") + std::to_string(day)
                         + "."
                         + std::to_string(VERSION_PATCH);

    EXPECT_EQ(version, libOpenCOR::version());
    EXPECT_EQ(versionString.data(), libOpenCOR::versionString());
}

TEST(VersionTest, Clang)
{
    EXPECT_EQ(0x160006U, libOpenCOR::clangVersion());
    EXPECT_EQ("16.0.6", libOpenCOR::clangVersionString());
}

TEST(VersionTest, libCellML)
{
    EXPECT_EQ(0x000500U, libOpenCOR::libcellmlVersion());
    EXPECT_EQ("0.5.0", libOpenCOR::libcellmlVersionString());
}

TEST(VersionTest, libCOMBINE)
{
    EXPECT_EQ(220U, libOpenCOR::libcombineVersion());
    EXPECT_EQ("0.2.20", libOpenCOR::libcombineVersionString());
}

TEST(VersionTest, libcurl)
{
    EXPECT_EQ(0x080D00U, libOpenCOR::libcurlVersion());
    EXPECT_EQ("8.13.0-DEV", libOpenCOR::libcurlVersionString());
}

TEST(VersionTest, libSEDML)
{
    EXPECT_EQ(20033U, libOpenCOR::libsedmlVersion());
    EXPECT_EQ("2.0.33", libOpenCOR::libsedmlVersionString());
}

TEST(VersionTest, LLVM)
{
    EXPECT_EQ(0x160006U, libOpenCOR::llvmVersion());
    EXPECT_EQ("16.0.6", libOpenCOR::llvmVersionString());
}

TEST(VersionTest, SUNDIALS)
{
    EXPECT_EQ(0x070300U, libOpenCOR::sundialsVersion());
    EXPECT_EQ("7.3.0", libOpenCOR::sundialsVersionString());
}
