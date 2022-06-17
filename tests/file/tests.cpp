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

static constexpr const char *WINDOWS_FILE = "C:\\some\\path\\file.txt";
static constexpr const char *UNIX_FILE = "/some/path/file.txt";
static constexpr const char *URL_BASED_WINDOWS_FILE = "file:///C:/some/path/file.txt";
static constexpr const char *URL_BASED_UNIX_FILE = "file:///some/path/file.txt";
static constexpr const char *REMOTE_FILE = "https://models.physiomeproject.org/workspace/noble_1962/rawfile/c70f8962407db00673f1fdcac9f35a2593781c17/noble_1962.cellml";

TEST(FileTest, windowsFile)
{
    auto file = libOpenCOR::File::create(WINDOWS_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::LOCAL, file->type());
    EXPECT_EQ(WINDOWS_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, unixFile)
{
    auto file = libOpenCOR::File::create(UNIX_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::LOCAL, file->type());
    EXPECT_EQ(UNIX_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, urlBasedWindowsFile)
{
    auto file = libOpenCOR::File::create(URL_BASED_WINDOWS_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::LOCAL, file->type());
    EXPECT_EQ(WINDOWS_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, urlBasedUnixFile)
{
    auto file = libOpenCOR::File::create(URL_BASED_UNIX_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::LOCAL, file->type());
    EXPECT_EQ(UNIX_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, remoteFile)
{
    auto file = libOpenCOR::File::create(REMOTE_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::REMOTE, file->type());
    EXPECT_EQ("", file->fileName());
    EXPECT_EQ(REMOTE_FILE, file->url());
}
