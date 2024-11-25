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

#include <filesystem>
#include <libopencor>

static const libOpenCOR::ExpectedIssues EXPECTED_NO_ISSUES = {};
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

TEST(BasicSedTest, existingRelativeLocalFile)
{
    auto origDir = std::filesystem::current_path();

    std::filesystem::current_path(libOpenCOR::resourcePath());

    auto file = libOpenCOR::File::create(libOpenCOR::CELLML_2_FILE);

    EXPECT_FALSE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, EXPECTED_NO_ISSUES);

    std::filesystem::current_path(origDir);
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

    auto someUnknownContents = libOpenCOR::charArrayToUnsignedChars(libOpenCOR::SOME_UNKNOWN_CONTENTS);

    file->setContents(someUnknownContents);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), someUnknownContents);
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

    auto someUnknownContents = libOpenCOR::charArrayToUnsignedChars(libOpenCOR::SOME_UNKNOWN_CONTENTS);

    file->setContents(someUnknownContents);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), someUnknownContents);
    EXPECT_EQ_ISSUES(file, EXPECTED_UNKNOWN_FILE_ISSUES);
}

TEST(BasicFileTest, fileManager)
{
    auto fileManager = libOpenCOR::FileManager::instance();

    EXPECT_FALSE(fileManager.hasFiles());
    EXPECT_EQ(fileManager.fileCount(), 0);
    EXPECT_TRUE(fileManager.files().empty());
    EXPECT_EQ(fileManager.file(0), nullptr);
    EXPECT_EQ(fileManager.file(libOpenCOR::LOCAL_FILE), nullptr);

    auto localFile = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto sameFileManager = libOpenCOR::FileManager::instance();

    EXPECT_TRUE(sameFileManager.hasFiles());
    EXPECT_EQ(sameFileManager.fileCount(), 1);
    EXPECT_EQ(sameFileManager.files().size(), 1);
    EXPECT_EQ(fileManager.file(0), localFile);
    EXPECT_EQ(sameFileManager.file(libOpenCOR::LOCAL_FILE), localFile);

    auto remoteFile = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);

    EXPECT_TRUE(fileManager.hasFiles());
    EXPECT_EQ(fileManager.fileCount(), 2);
    EXPECT_EQ(fileManager.files().size(), 2);
    EXPECT_EQ(fileManager.file(1), remoteFile);
    EXPECT_EQ(fileManager.file(libOpenCOR::REMOTE_FILE), remoteFile);

    sameFileManager.unmanage(localFile);

    EXPECT_TRUE(sameFileManager.hasFiles());
    EXPECT_EQ(sameFileManager.fileCount(), 1);
    EXPECT_EQ(sameFileManager.files().size(), 1);
    EXPECT_EQ(fileManager.file(1), nullptr);
    EXPECT_EQ(sameFileManager.file(libOpenCOR::LOCAL_FILE), nullptr);

    sameFileManager.manage(localFile);

    EXPECT_TRUE(sameFileManager.hasFiles());
    EXPECT_EQ(sameFileManager.fileCount(), 2);
    EXPECT_EQ(sameFileManager.files().size(), 2);
    EXPECT_EQ(fileManager.file(1), localFile);
    EXPECT_EQ(sameFileManager.file(libOpenCOR::LOCAL_FILE), localFile);

    fileManager.reset();

    EXPECT_FALSE(fileManager.hasFiles());
    EXPECT_EQ(fileManager.fileCount(), 0);
    EXPECT_EQ(fileManager.files().size(), 0);
    EXPECT_EQ(fileManager.file(0), nullptr);
    EXPECT_EQ(fileManager.file(1), nullptr);
    EXPECT_EQ(fileManager.file(libOpenCOR::REMOTE_FILE), nullptr);
    EXPECT_EQ(fileManager.file(libOpenCOR::UNKNOWN_FILE), nullptr);
}
