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
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    file->setContents({});

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(CoverageFileTest, fileWithNullCharacter)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    file->setContents({0});

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(CoverageFileTest, sedmlFileWithNoParent)
{
    auto file = libOpenCOR::File::create(libOpenCOR::SEDML_2_FILE);

    file->setContents(libOpenCOR::charArrayToUnsignedChars(libOpenCOR::SOME_SEDML_CONTENTS));
}

#ifdef GHA_NOT_WINDOWS_ON_ARM
TEST(CoverageFileTest, irretrievableRemoteFile)
{
    libOpenCOR::File::create(libOpenCOR::IRRETRIEVABLE_REMOTE_FILE);
}
#endif

TEST(CoverageFileTest, sameLocalFile)
{
    auto file1 = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto file2 = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    EXPECT_EQ(file1, file2);
}

#ifdef GHA_NOT_WINDOWS_ON_ARM
TEST(CoverageFileTest, sameRemoteFile)
{
    auto file1 = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto file2 = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);

    EXPECT_EQ(file1, file2);
}
#endif

TEST(CoverageFileTest, doNotRetrieveContents)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE, false);

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());
}

TEST(CoverageFileTest, unmanageFileWithChildren)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));
    auto fileManager = libOpenCOR::FileManager::instance();

    EXPECT_EQ(fileManager.fileCount(), 3);

    fileManager.unmanage(file);

    EXPECT_EQ(fileManager.fileCount(), 0);
}
