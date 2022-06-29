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

static constexpr const char *WINDOWS_FILE = "C:\\some\\path\\file.txt";
static constexpr const char *UNIX_FILE = "/some/path/file.txt";
static constexpr const char *URL_BASED_WINDOWS_FILE = "file:///C:/some/path/file.txt";
static constexpr const char *URL_BASED_UNIX_FILE = "file:///some/path/file.txt";
static constexpr const char *REMOTE_FILE = "https://models.physiomeproject.org/workspace/noble_1962/rawfile/c70f8962407db00673f1fdcac9f35a2593781c17/noble_1962.cellml";
static constexpr const char *UNKNOWN_FILE = "unknown_file.txt";
static constexpr const char *CELLML_1_X_FILE = "cellml_1_x.cellml";
static constexpr const char *SEDML_1_X_FILE = "cellml_1_x.sedml";
static constexpr const char *COMBINE_1_X_ARCHIVE = "cellml_1_x.omex";
static constexpr const char *CELLML_2_FILE = "cellml_2.cellml";
static constexpr const char *SEDML_2_FILE = "cellml_2.sedml";
static constexpr const char *COMBINE_2_ARCHIVE = "cellml_2.omex";

TEST(FileTest, windowsFile)
{
    auto file = libOpenCOR::File::create(WINDOWS_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::UNDEFINED, file->type());
    EXPECT_EQ(WINDOWS_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, unixFile)
{
    auto file = libOpenCOR::File::create(UNIX_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::UNDEFINED, file->type());
    EXPECT_EQ(UNIX_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, urlBasedWindowsFile)
{
    auto file = libOpenCOR::File::create(URL_BASED_WINDOWS_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::UNDEFINED, file->type());
    EXPECT_EQ(WINDOWS_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, urlBasedUnixFile)
{
    auto file = libOpenCOR::File::create(URL_BASED_UNIX_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::UNDEFINED, file->type());
    EXPECT_EQ(UNIX_FILE, file->fileName());
    EXPECT_EQ("", file->url());
}

TEST(FileTest, remoteFile)
{
    auto file = libOpenCOR::File::create(REMOTE_FILE);

    EXPECT_EQ(libOpenCOR::File::Type::UNDEFINED, file->type());
    EXPECT_EQ("", file->fileName());
    EXPECT_EQ(REMOTE_FILE, file->url());
}

TEST(FileTest, cellml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(CELLML_1_X_FILE));

    file->resolve();

    EXPECT_EQ(libOpenCOR::File::Type::CELLML_FILE, file->type());
}

TEST(FileTest, sedml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(SEDML_1_X_FILE));

    file->resolve();

    EXPECT_EQ(libOpenCOR::File::Type::SEDML_FILE, file->type());
}

TEST(FileTest, combine1xArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(COMBINE_1_X_ARCHIVE));

    file->resolve();

    EXPECT_EQ(libOpenCOR::File::Type::COMBINE_ARCHIVE, file->type());
}

TEST(FileTest, cellml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(CELLML_2_FILE));

    file->resolve();

    EXPECT_EQ(libOpenCOR::File::Type::CELLML_FILE, file->type());
}

TEST(FileTest, sedml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(SEDML_2_FILE));

    file->resolve();

    EXPECT_EQ(libOpenCOR::File::Type::SEDML_FILE, file->type());
}

TEST(FileTest, combine2Archive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(COMBINE_2_ARCHIVE));

    file->resolve();

    EXPECT_EQ(libOpenCOR::File::Type::COMBINE_ARCHIVE, file->type());
}

TEST(FileTest, unknownFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(UNKNOWN_FILE));

    file->resolve();

    EXPECT_EQ(libOpenCOR::File::Type::UNKNOWN_FILE, file->type());
}
