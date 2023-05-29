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

/*---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
TEST(VersionTest, Clang)
{
    EXPECT_EQ(0x140003U, libOpenCOR::clangVersion());
    EXPECT_EQ("14.0.3", libOpenCOR::clangVersionString());
}
*/

TEST(VersionTest, libCellML)
{
    EXPECT_EQ(0x000400U, libOpenCOR::libcellmlVersion());
    EXPECT_EQ("0.4.0", libOpenCOR::libcellmlVersionString());
}

TEST(VersionTest, libCOMBINE)
{
    EXPECT_EQ(218U, libOpenCOR::libcombineVersion());
    EXPECT_EQ("0.2.18", libOpenCOR::libcombineVersionString());
}

/*---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
TEST(VersionTest, libcurl)
{
    EXPECT_EQ(0x075301U, libOpenCOR::libcurlVersion());
    EXPECT_EQ("7.83.1-DEV", libOpenCOR::libcurlVersionString());
}
*/

TEST(VersionTest, libSEDML)
{
    EXPECT_EQ(20032U, libOpenCOR::libsedmlVersion());
    EXPECT_EQ("2.0.32", libOpenCOR::libsedmlVersionString());
}

/*---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
TEST(VersionTest, LLVM)
{
    EXPECT_EQ(0x140003U, libOpenCOR::llvmVersion());
    EXPECT_EQ("14.0.3", libOpenCOR::llvmVersionString());
}
*/

TEST(VersionTest, SUNDIALS)
{
    EXPECT_EQ(0x060200U, libOpenCOR::sundialsVersion());
    EXPECT_EQ("6.2.0", libOpenCOR::sundialsVersionString());
}
