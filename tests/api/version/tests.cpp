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
    static const int TEN_THOUSAND = 10000;
    static const int HUNDRED = 100;
    static const int TEN = 10;

    auto now = std::chrono::year_month_day {std::chrono::time_point_cast<std::chrono::days>(std::chrono::system_clock::now())};
    auto year = static_cast<int>(now.year());
    auto month = static_cast<int>(static_cast<unsigned>(now.month()));
    auto day = static_cast<int>(static_cast<unsigned>(now.day()));
    int version = 0;
    int number = TEN_THOUSAND * year + HUNDRED * month + day;

    for (int i = 0; number != 0; i += 4) {
        version |= (number % TEN) << i; // NOLINT
        number /= TEN;
    }

    static const size_t VERSION_STRING_SIZE = 11;

    std::array<char, VERSION_STRING_SIZE> versionString {};

    EXPECT_EQ(std::snprintf(versionString.data(), VERSION_STRING_SIZE, "%d.%02d.%02d", year, month, day), VERSION_STRING_SIZE - 1); // NOLINT
    // Note: ideally, we would be using std::format(), but it is not available on some of the CI systems we are using.

    EXPECT_EQ(version, libOpenCOR::version());
    EXPECT_EQ(versionString.data(), libOpenCOR::versionString());
}

TEST(VersionTest, Clang)
{
    EXPECT_EQ(0x140006U, libOpenCOR::clangVersion());
    EXPECT_EQ("14.0.6", libOpenCOR::clangVersionString());
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
    EXPECT_EQ(0x080800U, libOpenCOR::libcurlVersion());
    EXPECT_EQ("8.8.0-DEV", libOpenCOR::libcurlVersionString());
}

TEST(VersionTest, libSEDML)
{
    EXPECT_EQ(20032U, libOpenCOR::libsedmlVersion());
    EXPECT_EQ("2.0.32", libOpenCOR::libsedmlVersionString());
}

TEST(VersionTest, LLVM)
{
    EXPECT_EQ(0x140006U, libOpenCOR::llvmVersion());
    EXPECT_EQ("14.0.6", libOpenCOR::llvmVersionString());
}

TEST(VersionTest, SUNDIALS)
{
    EXPECT_EQ(0x070100U, libOpenCOR::sundialsVersion());
    EXPECT_EQ("7.1.0", libOpenCOR::sundialsVersionString());
}
