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

#include <libopencor>

static const libOpenCOR::ExpectedIssues EXPECTED_NON_EXISTING_FILE_ISSUES = {
    {libOpenCOR::Issue::Type::ERROR, "The file does not exist."},
};
static const libOpenCOR::ExpectedIssues EXPECTED_NON_DOWNLOADABLE_FILE_ISSUES = {
    {libOpenCOR::Issue::Type::ERROR, "The file could not be downloaded."},
};
static const libOpenCOR::ExpectedIssues EXPECTED_UNKNOWN_FILE_ISSUES = {
    {libOpenCOR::Issue::Type::ERROR, "The file is not a CellML file, a SED-ML file, or a COMBINE archive."},
};

TEST(BasicFileTest, localFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ(file->fileName(), libOpenCOR::LOCAL_FILE);
    EXPECT_EQ(file->url(), "");
    EXPECT_EQ(file->path(), libOpenCOR::LOCAL_FILE);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, EXPECTED_NON_EXISTING_FILE_ISSUES);
}

TEST(BasicFileTest, relativeLocalFile)
{
#ifdef _WIN32
    auto file = libOpenCOR::File::create(R"(some\.\relative\..\..\path\.\..\dir\file.txt)");
#else
    auto file = libOpenCOR::File::create("some/relative/../../path/../dir/file.txt");
#endif

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
#ifdef _WIN32
    EXPECT_EQ(file->fileName(), R"(dir\file.txt)");
#else
    EXPECT_EQ(file->fileName(), "dir/file.txt");
#endif
    EXPECT_EQ(file->url(), "");
#ifdef _WIN32
    EXPECT_EQ(file->path(), R"(dir\file.txt)");
#else
    EXPECT_EQ(file->path(), "dir/file.txt");
#endif
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, EXPECTED_NON_EXISTING_FILE_ISSUES);
}

TEST(BasicFileTest, urlBasedLocalFile)
{
#ifdef _WIN32
    auto file = libOpenCOR::File::create("file:///P:/some/path/file.txt");
#else
    auto file = libOpenCOR::File::create("file:///some/path/file.txt");
#endif

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ(file->fileName(), libOpenCOR::LOCAL_FILE);
    EXPECT_EQ(file->url(), "");
    EXPECT_EQ(file->path(), libOpenCOR::LOCAL_FILE);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, EXPECTED_NON_EXISTING_FILE_ISSUES);
}

TEST(BasicFileTest, remoteFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_NE(file->fileName(), "");
    EXPECT_EQ(file->url(), libOpenCOR::REMOTE_FILE);
    EXPECT_EQ(file->path(), libOpenCOR::REMOTE_FILE);
    EXPECT_FALSE(file->contents().empty());
}

TEST(BasicFileTest, localVirtualFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ(file->fileName(), libOpenCOR::LOCAL_FILE);
    EXPECT_EQ(file->url(), "");
    EXPECT_EQ(file->path(), libOpenCOR::LOCAL_FILE);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, EXPECTED_NON_EXISTING_FILE_ISSUES);

    auto someUnknownContentsVector = libOpenCOR::charArrayToVector(libOpenCOR::SOME_UNKNOWN_CONTENTS);

    file->setContents(someUnknownContentsVector);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), someUnknownContentsVector);
    EXPECT_EQ_ISSUES(file, EXPECTED_UNKNOWN_FILE_ISSUES);
}

TEST(BasicFileTest, remoteVirtualFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::IRRETRIEVABLE_REMOTE_FILE);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ(file->fileName(), "");
    EXPECT_EQ(file->url(), libOpenCOR::IRRETRIEVABLE_REMOTE_FILE);
    EXPECT_EQ(file->path(), libOpenCOR::IRRETRIEVABLE_REMOTE_FILE);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, EXPECTED_NON_DOWNLOADABLE_FILE_ISSUES);

    auto someUnknownContentsVector = libOpenCOR::charArrayToVector(libOpenCOR::SOME_UNKNOWN_CONTENTS);

    file->setContents(someUnknownContentsVector);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), someUnknownContentsVector);
    EXPECT_EQ_ISSUES(file, EXPECTED_UNKNOWN_FILE_ISSUES);
}
