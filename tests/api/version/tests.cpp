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
#include <fstream>
#include <regex>
#include <libopencor>

TEST(VersionTest, libOpenCOR)
{
    static const uint64_t TEN_BILLION = 10000000000;
    static const uint64_t TEN_THOUSAND = 10000;
    static const uint64_t HUNDRED = 100;
    static const uint64_t TEN = 10;

    std::ifstream versionFile("../../VERSION.txt");
    std::string versionString;
    std::getline(versionFile, versionString);

    versionFile.close();

    const std::regex versionRegex(R"((\d+)\.(\d{8})\.(\d+))");
    std::smatch match;

    ASSERT_TRUE(std::regex_match(versionString, match, versionRegex));

    const uint64_t majorVersion = static_cast<uint64_t>(std::stoi(match[1]));
    const uint64_t year = static_cast<uint64_t>(std::stoi(match[2].str().substr(0, 4)));
    const uint64_t month = static_cast<uint64_t>(std::stoi(match[2].str().substr(4, 2)));
    const uint64_t day = static_cast<uint64_t>(std::stoi(match[2].str().substr(6, 2)));
    const uint64_t patchVersion = static_cast<uint64_t>(std::stoi(match[3]));
    uint64_t version = 0;
    auto number = TEN_BILLION * majorVersion
                  + HUNDRED * (TEN_THOUSAND * year + HUNDRED * month + day)
                  + patchVersion;

    for (int i = 0; number != 0; i += 4) {
        version |= (number % TEN) << i; // NOLINT
        number /= TEN;
    }

    EXPECT_EQ(version, libOpenCOR::version());
    EXPECT_EQ(versionString, libOpenCOR::versionString());
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
