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
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::IRRETRIEVABLE_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::IRRETRIEVABLE_FILE);
    EXPECT_EQ_ISSUES(file, expectedIrretrievableFileIssues());
}

TEST(TypeFileTest, unknownFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(TypeFileTest, sbmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SBML_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(TypeFileTest, errorSedmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_SEDML_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, cellml1xFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_1_X_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedml1xFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_1_X_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combine1xArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_1_X_ARCHIVE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(TypeFileTest, cellml2File)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedml2File)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combine2Archive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(TypeFileTest, unknownVirtualFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE), false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::charArrayToUnsignedChars(libOpenCOR::UNKNOWN_CONTENTS));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ_ISSUES(file, expectedUnknownFileIssues());
}

TEST(TypeFileTest, cellmlVirtualFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE), false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::charArrayToUnsignedChars(libOpenCOR::CELLML_CONTENTS));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(TypeFileTest, sedmlVirtualFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE), false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::charArrayToUnsignedChars(libOpenCOR::SEDML_CONTENTS));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(TypeFileTest, combineVirtualArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE), false)};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
    EXPECT_EQ(file->contents(), libOpenCOR::UnsignedChars());

    file->setContents(libOpenCOR::base64Decode(libOpenCOR::BASE64_COMBINE_ARCHIVE_CONTENTS));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}
