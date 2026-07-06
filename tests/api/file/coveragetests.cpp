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

TEST(CoverageFileTest, emptyFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("file.txt"));

    file->setContents({});

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(CoverageFileTest, fileWithNullCharacter)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("file.txt"));

    file->setContents({0});

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(CoverageFileTest, sedmlFileWithNoParent)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.sedml"));

    file->setContents(libOpenCOR::charArrayToUnsignedChars(libOpenCOR::textFileContents(file->path()).c_str()));
}

TEST(CoverageFileTest, irretrievableVirtualFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("irretrievable_file.txt"), false);

    EXPECT_FALSE(file->hasIssues());
}

TEST(CoverageFileTest, irretrievableRemoteFile)
{
    libOpenCOR::File::create("https://some.domain.com/irretrievable_file.txt");
}

TEST(CoverageFileTest, sameLocalFile)
{
    auto filePath {libOpenCOR::resourcePath("file.txt")};
    auto file1 = libOpenCOR::File::create(filePath);
    auto file2 = libOpenCOR::File::create(filePath);

    EXPECT_EQ(file1, file2);
}

TEST(CoverageFileTest, sameRemoteFile)
{
    auto file1 = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto file2 = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);

    EXPECT_EQ(file1, file2);
}

TEST(CoverageFileTest, doNotRetrieveContents)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE, false);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());
}

TEST(CoverageFileTest, unmanageFileWithChildren)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.omex"));
    auto &fileManager = libOpenCOR::FileManager::instance();

    EXPECT_EQ(fileManager.fileCount(), 3u);

    fileManager.unmanage(file);

    EXPECT_EQ(fileManager.fileCount(), 0u);
}
