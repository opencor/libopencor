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

#include "utils.h"

#include "tests/utils.h"

#include <filesystem>
#include <libopencor>

namespace {

libOpenCOR::ExpectedIssues expectedNoIssues()
{
    return {};
}

libOpenCOR::ExpectedIssues expectedNonExistingFileIssues()
{
    return {{
        {libOpenCOR::Issue::Type::ERROR, "The file does not exist."},
    }};
}

libOpenCOR::ExpectedIssues expectedUnknownFileIssues()
{
    return {{
        {libOpenCOR::Issue::Type::ERROR, "The file is not a CellML file, a SED-ML file, or a COMBINE archive."},
    }};
}

} // namespace

TEST(BasicFileTest, localFile)
{
    auto filePath {libOpenCOR::resourcePath("file.txt")};
    auto file {libOpenCOR::File::create(filePath)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ(file->fileName(), filePath);
    EXPECT_EQ(file->url(), "");
    EXPECT_EQ(file->path(), filePath);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNonExistingFileIssues());
}

TEST(BasicSedTest, existingRelativeLocalFile)
{
    auto origDir {std::filesystem::current_path()};

    std::filesystem::current_path(libOpenCOR::resourcePath());

    auto file {libOpenCOR::File::create("cellml_2.cellml")};

    EXPECT_FALSE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNoIssues());

    std::filesystem::current_path(origDir);
}

TEST(BasicFileTest, nonExistingRelativeLocalFile)
{
#ifdef BUILDING_ON_WINDOWS
    auto file {libOpenCOR::File::create(R"(some\.\relative\..\..\path\.\..\dir\file.txt)")};
#else
    auto file {libOpenCOR::File::create("some/relative/../../path/../dir/file.txt")};
#endif

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
#ifdef BUILDING_ON_WINDOWS
    EXPECT_EQ(file->fileName(), R"(dir\file.txt)");
#else
    EXPECT_EQ(file->fileName(), "dir/file.txt");
#endif
    EXPECT_EQ(file->url(), "");
#ifdef BUILDING_ON_WINDOWS
    EXPECT_EQ(file->path(), R"(dir\file.txt)");
#else
    EXPECT_EQ(file->path(), "dir/file.txt");
#endif
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNonExistingFileIssues());
}

TEST(BasicFileTest, urlBasedLocalFile)
{
    auto filePath {libOpenCOR::resourcePath("file.txt")};
#ifdef BUILDING_ON_WINDOWS
    auto file {libOpenCOR::File::create(std::string("file:///") + libOpenCOR::forwardSlashPath(filePath))};
#else
    auto file {libOpenCOR::File::create(std::string("file://") + filePath)};
#endif

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ(file->fileName(), filePath);
    EXPECT_EQ(file->url(), "");
    EXPECT_EQ(file->path(), filePath);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNonExistingFileIssues());
}

TEST(BasicFileTest, remoteFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::REMOTE_FILE)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_NE(file->fileName(), "");
    EXPECT_EQ(file->url(), libOpenCOR::REMOTE_FILE);
    EXPECT_EQ(file->path(), libOpenCOR::REMOTE_FILE);
    EXPECT_FALSE(file->contents().empty());
}

TEST(BasicFileTest, encodedRemoteFile)
{
    auto file {libOpenCOR::File::create("https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO%20BG%20example%203.1.cellml")};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_NE(file->fileName(), "");
    EXPECT_EQ(file->url(), "https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml");
    EXPECT_EQ(file->path(), "https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml");
    EXPECT_FALSE(file->contents().empty());
}

TEST(BasicFileTest, localVirtualFile)
{
    auto filePath {libOpenCOR::resourcePath("unknown_file.txt")};
    auto file {libOpenCOR::File::create(filePath, false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->fileName(), filePath);
    EXPECT_EQ(file->url(), "");
    EXPECT_EQ(file->path(), filePath);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNoIssues());

    auto someUnknownContents {libOpenCOR::charArrayToUnsignedChars(libOpenCOR::textFileContents(filePath).c_str())};

    file->setContents(someUnknownContents);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), someUnknownContents);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(BasicFileTest, remoteVirtualFile)
{
    auto file {libOpenCOR::File::create("https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt", false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
#ifdef BUILDING_USING_MSVC
    EXPECT_EQ(file->fileName(), "\\some\\path\\file");
#else
    EXPECT_EQ(file->fileName(), "/some/path/file");
#endif
    EXPECT_EQ(file->url(), "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt");
    EXPECT_EQ(file->path(), "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt");
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNoIssues());

    auto filePath {libOpenCOR::resourcePath("unknown_file.txt")};
    auto someUnknownContents {libOpenCOR::charArrayToUnsignedChars(libOpenCOR::textFileContents(filePath).c_str())};

    file->setContents(someUnknownContents);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), someUnknownContents);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(BasicFileTest, fileManager)
{
    auto &fileManager {libOpenCOR::FileManager::instance()};
    auto filePath {libOpenCOR::resourcePath("file.txt")};

    EXPECT_FALSE(fileManager.hasFiles());
    EXPECT_EQ(fileManager.fileCount(), 0U);
    EXPECT_TRUE(fileManager.files().empty());
    EXPECT_EQ(fileManager.file(0), nullptr);
    EXPECT_EQ(fileManager.file(filePath), nullptr);

    auto localFile {libOpenCOR::File::create(filePath)};
    auto &sameFileManager {libOpenCOR::FileManager::instance()};

    EXPECT_TRUE(sameFileManager.hasFiles());
    EXPECT_EQ(sameFileManager.fileCount(), 1U);
    EXPECT_EQ(sameFileManager.files().size(), 1U);
    EXPECT_EQ(fileManager.file(0), localFile);
    EXPECT_EQ(sameFileManager.file(filePath), localFile);

    auto remoteFile {libOpenCOR::File::create(libOpenCOR::REMOTE_FILE)};

    EXPECT_TRUE(fileManager.hasFiles());
    EXPECT_EQ(fileManager.fileCount(), 2U);
    EXPECT_EQ(fileManager.files().size(), 2U);
    EXPECT_EQ(fileManager.file(1), remoteFile);
    EXPECT_EQ(fileManager.file(libOpenCOR::REMOTE_FILE), remoteFile);

    sameFileManager.unmanage(localFile);

    EXPECT_TRUE(sameFileManager.hasFiles());
    EXPECT_EQ(sameFileManager.fileCount(), 1U);
    EXPECT_EQ(sameFileManager.files().size(), 1U);
    EXPECT_EQ(fileManager.file(1), nullptr);
    EXPECT_EQ(sameFileManager.file(filePath), nullptr);

    sameFileManager.manage(localFile);

    EXPECT_TRUE(sameFileManager.hasFiles());
    EXPECT_EQ(sameFileManager.fileCount(), 2U);
    EXPECT_EQ(sameFileManager.files().size(), 2U);
    EXPECT_EQ(fileManager.file(1), localFile);
    EXPECT_EQ(sameFileManager.file(filePath), localFile);

    fileManager.reset();

    EXPECT_FALSE(fileManager.hasFiles());
    EXPECT_EQ(fileManager.fileCount(), 0U);
    EXPECT_EQ(fileManager.files().size(), 0U);
    EXPECT_EQ(fileManager.file(0), nullptr);
    EXPECT_EQ(fileManager.file(1), nullptr);
    EXPECT_EQ(fileManager.file(libOpenCOR::REMOTE_FILE), nullptr);
    EXPECT_EQ(fileManager.file(libOpenCOR::resourcePath("unknown_file.txt")), nullptr);
}
