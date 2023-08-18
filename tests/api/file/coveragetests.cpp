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

TEST(CoverageFileTest, httpRemoteFile)
{
    libOpenCOR::File::create(libOpenCOR::HTTP_REMOTE_FILE);
}

TEST(CoverageFileTest, irretrievableRemoteFile)
{
    libOpenCOR::File::create(libOpenCOR::IRRETRIEVABLE_REMOTE_FILE);
}

TEST(CoverageFileTest, sameLocalFile)
{
    auto file1 = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto file2 = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    EXPECT_EQ(file1, file2);
}

TEST(CoverageFileTest, sameRemoteFile)
{
    auto file1 = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto file2 = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);

    EXPECT_EQ(file1, file2);
}
