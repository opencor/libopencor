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

static constexpr const char *NON_RETRIEVABLE_LOCAL_FILE = "non_retrievable_file.txt";
static constexpr const char *CELLML_2_LOCAL_FILE = "cellml_2.cellml";
static constexpr const char *SEDML_2_LOCAL_FILE = "cellml_2.sedml";
static constexpr const char *COMBINE_2_LOCAL_ARCHIVE = "cellml_2.omex";

static constexpr const char *NON_RETRIEVABLE_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/non_retrievable_file.txt";
static constexpr const char *CELLML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml";
static constexpr const char *SEDML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml";
static constexpr const char *COMBINE_2_REMOTE_ARCHIVE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex";

TEST(InstantiateFileTest, instantiateNonRetrievableLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(NON_RETRIEVABLE_LOCAL_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_RETRIEVABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCellml2LocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(CELLML_2_LOCAL_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::OK);
}

TEST(InstantiateFileTest, instantiateSedml2LocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(SEDML_2_LOCAL_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCombine2LocalArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(COMBINE_2_LOCAL_ARCHIVE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateNonRetrievableRemoteFile)
{
    auto file = libOpenCOR::File::create(NON_RETRIEVABLE_REMOTE_FILE);

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_RETRIEVABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCellml2RemoteFile)
{
    auto file = libOpenCOR::File::create(CELLML_2_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::OK);
}

TEST(InstantiateFileTest, instantiateSedml2RemoteFile)
{
    auto file = libOpenCOR::File::create(SEDML_2_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCombine2RemoteArchive)
{
    auto file = libOpenCOR::File::create(COMBINE_2_REMOTE_ARCHIVE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}
