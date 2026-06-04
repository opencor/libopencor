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

#include <libopencor>

namespace {

libOpenCOR::ExpectedIssues expectedUnknownFileIssues()
{
    return {{
        {libOpenCOR::Issue::Type::ERROR, "The file is not a CellML file, a SED-ML file, or a COMBINE archive."},
    }};
}

libOpenCOR::ExpectedIssues expectedIrretrievableFileIssues()
{
    return {{
        {libOpenCOR::Issue::Type::ERROR, "The file does not exist."},
    }};
}

} // namespace

TEST(TypeFileTest, irretrievableFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("irretrievable_file.txt"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ_ISSUES(file, expectedIrretrievableFileIssues());
}

TEST(TypeFileTest, unknownFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("unknown_file.txt"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(TypeFileTest, sbmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("sbml.sbml"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(TypeFileTest, errorSedmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("error.sedml"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, cellml1xFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_1_x.cellml"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedml1xFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_1_x.sedml"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combine1xArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_1_x.omex"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(TypeFileTest, cellml2File)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedml2File)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.sedml"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combine2Archive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.omex"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(TypeFileTest, unknownVirtualFile)
{
    auto filePath {libOpenCOR::resourcePath("unknown_file.txt")};
    auto file {libOpenCOR::File::create(filePath, false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::fileContents(filePath));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(TypeFileTest, cellmlVirtualFile)
{
    auto filePath {libOpenCOR::resourcePath("cellml_2.cellml")};
    auto file {libOpenCOR::File::create(filePath, false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::fileContents(filePath));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedmlVirtualFile)
{
    auto filePath {libOpenCOR::resourcePath("cellml_2.sedml")};
    auto file {libOpenCOR::File::create(filePath, false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::fileContents(filePath));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combineVirtualArchive)
{
    auto filePath {libOpenCOR::resourcePath("cellml_2.omex")};
    auto file {libOpenCOR::File::create(filePath, false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::fileContents(filePath));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
