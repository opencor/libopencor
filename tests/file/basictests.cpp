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

#include "tests/utils.h"

#include <libopencor>

static constexpr auto WINDOWS_LOCAL_FILE = R"(C:\some\path\file.txt)";
static constexpr auto UNIX_LOCAL_FILE = "/some/path/file.txt";

TEST(BasicFileTest, windowsLocalFile)
{
    auto file = libOpenCOR::File::create(WINDOWS_LOCAL_FILE);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNRESOLVED);
    EXPECT_EQ(file->fileName(), WINDOWS_LOCAL_FILE);
    EXPECT_EQ(file->url(), "");
}

TEST(BasicFileTest, unixLocalFile)
{
    auto file = libOpenCOR::File::create(UNIX_LOCAL_FILE);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNRESOLVED);
    EXPECT_EQ(file->fileName(), UNIX_LOCAL_FILE);
    EXPECT_EQ(file->url(), "");
}

TEST(BasicFileTest, urlBasedWindowsLocalFile)
{
    auto file = libOpenCOR::File::create("file:///C:/some/path/file.txt");

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNRESOLVED);
    EXPECT_EQ(file->fileName(), WINDOWS_LOCAL_FILE);
    EXPECT_EQ(file->url(), "");
}

TEST(BasicFileTest, urlBasedUnixLocalFile)
{
    auto file = libOpenCOR::File::create("file:///some/path/file.txt");

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNRESOLVED);
    EXPECT_EQ(file->fileName(), UNIX_LOCAL_FILE);
    EXPECT_EQ(file->url(), "");
}

TEST(BasicFileTest, remoteFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNRESOLVED);
    EXPECT_EQ(file->fileName(), "");
    EXPECT_EQ(file->url(), libOpenCOR::REMOTE_FILE);
}
