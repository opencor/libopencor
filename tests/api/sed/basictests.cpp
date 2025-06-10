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

TEST(BasicSedTest, noFile)
{
    auto document = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, unknownFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using an unknown file."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, cellmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, sedmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_TRUE(document->hasIssues());

    auto neededFile = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));

    document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, sedmlFileWithAbsoluteCellmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/absolute_cellml_file.sedml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_TRUE(document->hasIssues());

    auto neededFile = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, sedmlFileWithRemoteCellmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/remote_cellml_file.sedml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_TRUE(document->hasIssues());

    auto neededFile = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);

    document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, combineArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, combineArchiveWithNoManifestFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with no master file."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_manifest_file.omex"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithNoMasterFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with no master file."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_master_file.omex"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithSbmlFileAsMasterFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported)."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/sbml_file_as_master_file.omex"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, irretrievableFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using an irretrievable file."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::IRRETRIEVABLE_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}
