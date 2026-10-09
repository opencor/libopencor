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

#include <atomic>
#include <filesystem>
#include <fstream>
#include <libopencor>
#include <set>
#include <thread>

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

TEST(BasicFileTest, nonExistingRelativeLocalFileWithLeadingParentDirectories)
{
    auto file {libOpenCOR::File::create("../models/./lorenz.cellml")};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
#ifdef BUILDING_ON_WINDOWS
    EXPECT_EQ(file->fileName(), R"(..\models\lorenz.cellml)");
#else
    EXPECT_EQ(file->fileName(), "../models/lorenz.cellml");
#endif
    EXPECT_EQ(file->url(), "");
#ifdef BUILDING_ON_WINDOWS
    EXPECT_EQ(file->path(), R"(..\models\lorenz.cellml)");
#else
    EXPECT_EQ(file->path(), "../models/lorenz.cellml");
#endif
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNonExistingFileIssues());
}

TEST(BasicFileTest, tooLongLocalFileName)
{
    // A file name that is too long for the file system must not result in an exception being thrown.

    static constexpr auto TOO_LONG_NAME_LENGTH {5000};

    auto file {libOpenCOR::File::create("/" + std::string(TOO_LONG_NAME_LENGTH, 'a'))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNonExistingFileIssues());
}

TEST(BasicFileTest, localDirectory)
{
    // A directory is not a file, but it must not result in an exception being thrown.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_TRUE(file->contents().empty());
    EXPECT_EQ_ISSUES(file, expectedNoIssues());
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

TEST(BasicFileTest, remoteFileLocalCopy)
{
    // The local copy of a remote file must exist for as long as the remote file exists, and be deleted with it.

    std::string fileName;

    {
        auto file {libOpenCOR::File::create(libOpenCOR::REMOTE_FILE)};

        fileName = file->fileName();

        EXPECT_TRUE(std::filesystem::exists(fileName));
    }

    EXPECT_FALSE(std::filesystem::exists(fileName));
}

TEST(BasicFileTest, localFileMatchingLocalCopyOfRemoteFile)
{
    // A local file must never be confused with the local copy of a remote file, and deleting it must not delete that
    // local copy (since it is not a file that we downloaded).

    auto remoteFile {libOpenCOR::File::create(libOpenCOR::REMOTE_FILE)};
    const auto &fileName {remoteFile->fileName()};

    {
        auto localFile {libOpenCOR::File::create(fileName)};

        EXPECT_NE(localFile, remoteFile);
        EXPECT_EQ(localFile->fileName(), libOpenCOR::canonicalFileName(fileName));
        EXPECT_EQ(localFile->url(), "");
        EXPECT_EQ(localFile->contents(), remoteFile->contents());
        EXPECT_EQ(libOpenCOR::FileManager::instance().fileCount(), 2U);
    }

    EXPECT_TRUE(std::filesystem::exists(fileName));
    EXPECT_EQ(libOpenCOR::FileManager::instance().file(fileName), nullptr);
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

TEST(BasicFileTest, remoteFileWithDotSegments)
{
    auto file {libOpenCOR::File::create("https://example.com/a/./b/../../c/model.cellml", false)};

    EXPECT_EQ(file->url(), "https://example.com/c/model.cellml");
    EXPECT_EQ(file->path(), "https://example.com/c/model.cellml");
}

TEST(BasicFileTest, remoteFileMatchingLocalFile)
{
    // The host and path of a URL must never be resolved as a local file, even if such a local file exists.

    auto origDir {std::filesystem::current_path()};
    auto tempDir {std::filesystem::temp_directory_path() / "libopencor_remote_file_matching_local_file"};

    std::filesystem::create_directories(tempDir / "example.com");
    std::ofstream(tempDir / "example.com" / "model.cellml").close();

    std::filesystem::current_path(tempDir);

    auto url {libOpenCOR::File::create("https://example.com/model.cellml", false)->url()};

    std::filesystem::current_path(origDir);
    std::filesystem::remove_all(tempDir);

    EXPECT_EQ(url, "https://example.com/model.cellml");
}

TEST(BasicFileTest, concurrentFileCreations)
{
    // Creating a file must never change the current working directory since it is shared by all the threads of the
    // process. Also, the file manager must cope with files that get looked up while they are being destroyed by another
    // thread.

    static constexpr auto FILE_CREATION_COUNT {1000};

    auto origDir {std::filesystem::current_path()};
    std::atomic<bool> done {false};
    std::atomic<bool> workingDirectoryChanged {false};
    std::thread checker([&] {
        auto &fileManager {libOpenCOR::FileManager::instance()};

        while (!done) {
            if (std::filesystem::current_path() != origDir) {
                workingDirectoryChanged = true;
            }

            fileManager.file("https://example.com/model.cellml");
            fileManager.files();
        }
    });
    auto createFiles {[] {
        for (int i {0}; i < FILE_CREATION_COUNT; ++i) {
            libOpenCOR::File::create("https://example.com/model.cellml", false);
            libOpenCOR::File::create("non_existing_dir/../non_existing_file.txt", false);
        }
    }};
    std::thread thread1(createFiles);
    std::thread thread2(createFiles);

    thread1.join();
    thread2.join();

    done = true;

    checker.join();

    EXPECT_FALSE(workingDirectoryChanged);
    EXPECT_EQ(std::filesystem::current_path(), origDir);
}

TEST(BasicFileTest, concurrentFileDownloads)
{
    // Remote files that are downloaded at the same time must each have their own local copy, so that deleting the local
    // copy of one of them doesn't delete the local copy of another one.

    static constexpr size_t THREAD_COUNT {8};

    std::vector<libOpenCOR::FilePtr> files(THREAD_COUNT);
    std::vector<std::thread> threads;

    threads.reserve(THREAD_COUNT);

    for (size_t i {0}; i < THREAD_COUNT; ++i) {
        threads.emplace_back([&files, i] {
            files[i] = libOpenCOR::File::create(std::string(libOpenCOR::REMOTE_FILE) + "?thread=" + std::to_string(i));
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    std::set<std::string> fileNames;

    for (const auto &file : files) {
        EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
        EXPECT_EQ(file->contents(), files[0]->contents());

        fileNames.insert(file->fileName());
    }

    EXPECT_EQ(fileNames.size(), THREAD_COUNT);

    files.clear();

    for (const auto &fileName : fileNames) {
        EXPECT_FALSE(std::filesystem::exists(fileName));
    }
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
    EXPECT_EQ(file->fileName(), "");
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

TEST(BasicFileTest, remoteVirtualFileMatchingLocalFile)
{
    // A remote file that has no local copy has an empty file name, so it must never be confused with a local file that
    // has an empty file name.

    auto remoteFile {libOpenCOR::File::create(libOpenCOR::REMOTE_FILE, false)};
    auto localFile {libOpenCOR::File::create("", false)};

    EXPECT_NE(localFile, remoteFile);
    EXPECT_EQ(remoteFile->fileName(), "");
    EXPECT_EQ(localFile->fileName(), "");
    EXPECT_EQ(localFile->url(), "");
    EXPECT_EQ(libOpenCOR::FileManager::instance().file(""), localFile);
    EXPECT_EQ(libOpenCOR::FileManager::instance().file(libOpenCOR::REMOTE_FILE), remoteFile);
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
