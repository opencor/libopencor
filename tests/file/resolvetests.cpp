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
static constexpr const char *UNKNOWN_LOCAL_FILE = "unknown_file.txt";
static constexpr const char *SBML_LOCAL_FILE = "sbml.sbml";
static constexpr const char *ERROR_SEDML_LOCAL_FILE = "error.sedml";
/*---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
static constexpr const char *CELLML_1_X_LOCAL_FILE = "cellml_1_x.cellml";
static constexpr const char *SEDML_1_X_LOCAL_FILE = "cellml_1_x.sedml";
static constexpr const char *COMBINE_1_X_LOCAL_ARCHIVE = "cellml_1_x.omex";
*/
static constexpr const char *CELLML_2_LOCAL_FILE = "cellml_2.cellml";
static constexpr const char *SEDML_2_LOCAL_FILE = "cellml_2.sedml";
static constexpr const char *COMBINE_2_LOCAL_ARCHIVE = "cellml_2.omex";

static constexpr const char *NON_RETRIEVABLE_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/non_retrievable_file.txt";
static constexpr const char *UNKNOWN_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt";
static constexpr const char *SBML_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/sbml.sbml";
static constexpr const char *ERROR_SEDML_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/error.sedml";
/*---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
static constexpr const char *CELLML_1_X_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_1_x.cellml";
static constexpr const char *SEDML_1_X_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_1_x.sedml";
static constexpr const char *COMBINE_1_X_REMOTE_ARCHIVE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_1_x.omex";
*/
static constexpr const char *CELLML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml";
static constexpr const char *SEDML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml";
static constexpr const char *COMBINE_2_REMOTE_ARCHIVE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex";

TEST(ResolveFileTest, resolveNonRetrievableLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(NON_RETRIEVABLE_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::NON_RETRIEVABLE_FILE);
}

TEST(ResolveFileTest, resolveUnknownLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(UNKNOWN_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(ResolveFileTest, resolveSbmlLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(SBML_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(ResolveFileTest, resolveErrorSedmlLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(ERROR_SEDML_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

/*---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
TEST(ResolveFileTest, resolveCellml1xLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(CELLML_1_X_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(ResolveFileTest, resolveSedml1xLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(SEDML_1_X_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(ResolveFileTest, resolveCombine1xLocalArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(COMBINE_1_X_LOCAL_ARCHIVE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
*/

TEST(ResolveFileTest, resolveCellml2LocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(CELLML_2_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(ResolveFileTest, resolveSedml2LocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(SEDML_2_LOCAL_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(ResolveFileTest, resolveCombine2LocalArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(COMBINE_2_LOCAL_ARCHIVE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(ResolveFileTest, resolveNonRetrievableRemoteFile)
{
    auto file = libOpenCOR::File::create(NON_RETRIEVABLE_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::NON_RETRIEVABLE_FILE);
}

TEST(ResolveFileTest, resolveUnknownRemoteFile)
{
    auto file = libOpenCOR::File::create(UNKNOWN_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(ResolveFileTest, resolveSbmlRemoteFile)
{
    auto file = libOpenCOR::File::create(SBML_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(ResolveFileTest, resolveErrorSedmlRemoteFile)
{
    auto file = libOpenCOR::File::create(ERROR_SEDML_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

/*---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
TEST(ResolveFileTest, resolveCellml1xRemoteFile)
{
    auto file = libOpenCOR::File::create(CELLML_1_X_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(ResolveFileTest, resolveSedml1xRemoteFile)
{
    auto file = libOpenCOR::File::create(SEDML_1_X_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(ResolveFileTest, resolveCombine1xRemoteArchive)
{
    auto file = libOpenCOR::File::create(COMBINE_1_X_REMOTE_ARCHIVE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
*/

TEST(ResolveFileTest, resolveCellml2RemoteFile)
{
    auto file = libOpenCOR::File::create(CELLML_2_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(ResolveFileTest, resolveSedml2RemoteFile)
{
    auto file = libOpenCOR::File::create(SEDML_2_REMOTE_FILE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(ResolveFileTest, resolveCombine2RemoteArchive)
{
    auto file = libOpenCOR::File::create(COMBINE_2_REMOTE_ARCHIVE);

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
