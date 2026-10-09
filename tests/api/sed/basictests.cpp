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

#include <filesystem>
#include <libopencor>

namespace {

std::string sedmlContents(const std::string &pModelSource)
{
    return R"(<?xml version='1.0' encoding='UTF-8'?>
<sedML level="1" version="3" xmlns="http://sed-ml.org/sed-ml/level1/version3">
    <listOfModels>
        <model id="model" language="urn:sedml:language:cellml" source=")"
           + pModelSource + R"("/>
    </listOfModels>
</sedML>
)";
}

} // namespace

TEST(BasicSedTest, noFile)
{
    auto document {libOpenCOR::SedDocument::create()};

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, unknownFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using an unknown file."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("unknown_file.txt"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, cellmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, sedmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.sedml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_TRUE(document->hasIssues());

    auto neededFile {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};

    document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, sedmlFileWithAbsoluteCellmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/absolute_cellml_file.sedml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_TRUE(document->hasIssues());

    auto neededFile {libOpenCOR::File::create(libOpenCOR::resourcePath("file.txt"))};

    document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, sedmlFileWithRelativeCellmlFile)
{
    // A relative model source is relative to the SED-ML file and its leading ".." components must be kept.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/relative_cellml_file.sedml"), false)};

    file->setContents(libOpenCOR::charArrayToUnsignedChars(sedmlContents("../../cellml_2.cellml").c_str()));

    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ(document->models().size(), 1U);
    EXPECT_EQ(document->models()[0]->file()->path(), libOpenCOR::resourcePath("cellml_2.cellml"));

    auto neededFile {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};

    document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
    EXPECT_EQ(document->models().size(), 1U);
    EXPECT_EQ(document->models()[0]->file(), neededFile);
}

TEST(BasicSedTest, sedmlFileWithRelativeCellmlFileInWorkingDirectory)
{
    // A relative model source is relative to the SED-ML file, not to the current working directory, even if the
    // current working directory contains a file with that name.

    auto origDir {std::filesystem::current_path()};

    std::filesystem::current_path(libOpenCOR::resourcePath());

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/relative_cellml_file.sedml"), false)};

    file->setContents(libOpenCOR::charArrayToUnsignedChars(sedmlContents("cellml_2.cellml").c_str()));

    auto document {libOpenCOR::SedDocument::create(file)};

    std::filesystem::current_path(origDir);

    EXPECT_EQ(document->models().size(), 1U);
    EXPECT_EQ(document->models()[0]->file()->path(), libOpenCOR::resourcePath("api/sed/cellml_2.cellml"));
}

TEST(BasicSedTest, remoteSedmlFileWithRelativeCellmlFile)
{
    // A relative model source is relative to the remote SED-ML file, whatever the platform and even if the query and/or
    // fragment of the SED-ML file's URL contain some forward slashes.

    auto file {libOpenCOR::File::create("https://example.com/simulations/simulation.sedml?a=b/c#d/e", false)};

    file->setContents(libOpenCOR::charArrayToUnsignedChars(sedmlContents("../models/./model.cellml").c_str()));

    auto neededFile {libOpenCOR::File::create("https://example.com/models/model.cellml", false)};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());
    EXPECT_EQ(document->models().size(), 1U);
    EXPECT_EQ(document->models()[0]->file(), neededFile);
}

TEST(BasicSedTest, remoteSedmlFileWithoutPathWithRelativeCellmlFile)
{
    // A relative model source is relative to the root of a remote SED-ML file which URL has no path.

    auto file {libOpenCOR::File::create("https://example.com?a=b/c", false)};

    file->setContents(libOpenCOR::charArrayToUnsignedChars(sedmlContents("model.cellml").c_str()));

    auto neededFile {libOpenCOR::File::create("https://example.com/model.cellml", false)};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());
    EXPECT_EQ(document->models().size(), 1U);
    EXPECT_EQ(document->models()[0]->file(), neededFile);
}

TEST(BasicSedTest, sedmlFileWithRemoteCellmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/remote_cellml_file.sedml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_TRUE(document->hasIssues());

    auto neededFile {libOpenCOR::File::create(libOpenCOR::REMOTE_FILE)};

    document = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, combineArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());
}

TEST(BasicSedTest, combineArchiveWithNoManifestFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with no master file."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_manifest_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithNoMasterFileAndOneCellmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_master_file_with_one_cellml_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());
    EXPECT_EQ(std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])->outputEndTime(), 1000.0);

    auto instance {document->instantiate()};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(BasicSedTest, combineArchiveWithNoMasterFileAndOneUnknownCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported)."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_master_file_with_one_unknown_cellml_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithNoMasterFileAndSeveralCellmlFiles)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with no master file, no SED-ML file, and several CellML files."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_master_file_with_several_cellml_files.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithNoMasterFileAndOneSedmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_master_file_with_one_sedml_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());
    EXPECT_EQ(std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])->outputEndTime(), 50.0);

    auto instance {document->instantiate()};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(BasicSedTest, combineArchiveWithNoMasterFileAndOneUnknownSedmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported)."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_master_file_with_one_unknown_sedml_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithNoMasterFileAndSeveralSedmlFiles)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with no master file and several SED-ML files."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/no_master_file_with_several_sedml_files.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithUnknownDirectCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported)."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unknown_direct_cellml_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithUnknownIndirectCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task: task 'task1' requires a model of CellML type."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unknown_indirect_cellml_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());

    auto instance {document->instantiate()};

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(BasicSedTest, combineArchiveWithUnknownSedmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported)."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unknown_sedml_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}

TEST(BasicSedTest, irretrievableFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description cannot be created using an irretrievable file."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("irretrievable_file.txt"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
}
