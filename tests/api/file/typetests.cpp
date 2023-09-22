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

static const libOpenCOR::ExpectedIssues EXPECTED_UNKNOWN_FILE_ISSUES = {
    {libOpenCOR::Issue::Type::ERROR, "The file is not a CellML file, a SED-ML file, or a COMBINE archive."},
};

TEST(TypeFileTest, irretrievableFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The file does not exist."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::IRRETRIEVABLE_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ_ISSUES(file, EXPECTED_ISSUES);
}

TEST(TypeFileTest, unknownFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, EXPECTED_UNKNOWN_FILE_ISSUES);
}

TEST(TypeFileTest, sbmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SBML_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, EXPECTED_UNKNOWN_FILE_ISSUES);
}

TEST(TypeFileTest, errorSedmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_SEDML_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, cellml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_1_X_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_1_X_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combine1xArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_1_X_ARCHIVE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(TypeFileTest, cellml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combine2Archive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(TypeFileTest, unknownVirtualFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_UNKNOWN_CONTENTS));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, EXPECTED_UNKNOWN_FILE_ISSUES);
}

TEST(TypeFileTest, cellmlVirtualFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedmlVirtualFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_SEDML_CONTENTS));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}
