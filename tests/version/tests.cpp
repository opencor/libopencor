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

#include "gtest/gtest.h"

#include <libopencor>

TEST(VersionTest, libOpenCOR)
{
    EXPECT_EQ(0x000100U, libOpenCOR::version());
    EXPECT_EQ("0.1.0", libOpenCOR::versionString());
}

TEST(VersionTest, Clang)
{
    EXPECT_EQ(0x130000U, libOpenCOR::clangVersion());
    EXPECT_EQ("13.0.0", libOpenCOR::clangVersionString());
}

TEST(VersionTest, libCellML)
{
    EXPECT_EQ(0x000200U, libOpenCOR::libcellmlVersion());
    EXPECT_EQ("0.2.0", libOpenCOR::libcellmlVersionString());
}

TEST(VersionTest, libCOMBINE)
{
    EXPECT_EQ(213U, libOpenCOR::libcombineVersion());
    EXPECT_EQ("0.2.13", libOpenCOR::libcombineVersionString());
}

TEST(VersionTest, libcurl)
{
    EXPECT_EQ(0x075000U, libOpenCOR::libcurlVersion());
    EXPECT_EQ("7.80.0-DEV", libOpenCOR::libcurlVersionString());
}

TEST(VersionTest, libSEDML)
{
    EXPECT_EQ(404U, libOpenCOR::libsedmlVersion());
    EXPECT_EQ("0.4.4", libOpenCOR::libsedmlVersionString());
}

TEST(VersionTest, LLVM)
{
    EXPECT_EQ(0x130000U, libOpenCOR::llvmVersion());
    EXPECT_EQ("13.0.0", libOpenCOR::llvmVersionString());
}

TEST(VersionTest, SUNDIALS)
{
    EXPECT_EQ(0x050800U, libOpenCOR::sundialsVersion());
    EXPECT_EQ("5.8.0", libOpenCOR::sundialsVersionString());
}
