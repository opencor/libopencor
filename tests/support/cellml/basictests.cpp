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

#include "cellmlfile.h"
#include "utils.h"

#include "tests/utils.h"

#include <libopencor>

TEST(BasicCellmlTest, cellml1xModel)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_1_x.cellml"));
    auto cellmlFile = libOpenCOR::CellmlFile::create(file);

    EXPECT_FALSE(cellmlFile->hasIssues());
}

TEST(BasicCellmlTest, cellml2Model)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"));
    auto cellmlFile = libOpenCOR::CellmlFile::create(file);

    EXPECT_FALSE(cellmlFile->hasIssues());
}

TEST(BasicCellmlTest, modelWithValidImports)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_valid_imports/model.cellml"));
    auto cellmlFile = libOpenCOR::CellmlFile::create(file);

    EXPECT_FALSE(cellmlFile->hasIssues());
    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::ODE);
}

TEST(BasicCellmlTest, modelWithValidRemoteImports)
{
    auto file {libOpenCOR::File::create(std::string(libOpenCOR::REMOTE_BASE_PATH) + "/support/cellml/model_with_valid_imports/model.cellml")};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    EXPECT_FALSE(cellmlFile->hasIssues());
    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::ODE);
}

TEST(BasicCellmlTest, modelWithNestedImports)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_nested_imports/model.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    EXPECT_FALSE(cellmlFile->hasIssues());
    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::ODE);
}

TEST(BasicCellmlTest, modelWithNestedImportsInCombineArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/combine/model_with_nested_imports.omex"))};

    EXPECT_FALSE(file->hasIssues());
    EXPECT_EQ(file->childFileCount(), 3U);

    auto cellmlFile {libOpenCOR::CellmlFile::create(file->childFile("model.cellml"))};

    EXPECT_FALSE(cellmlFile->hasIssues());
    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::ODE);
}

TEST(BasicCellmlTest, modelWithInMemoryImports)
{
    static const std::string BASE_URL {"https://example.com/model_with_nested_imports/"};
    static const std::vector<std::string> FILE_NAMES {"units.cellml", "components/component.cellml", "model.cellml"};

    libOpenCOR::FilePtrs files;

    for (const auto &fileName : FILE_NAMES) {
        auto file {libOpenCOR::File::create(BASE_URL + fileName, false)};

        file->setContents(libOpenCOR::fileContents(libOpenCOR::resourcePath("support/cellml/model_with_nested_imports/" + fileName)));

        files.push_back(file);
    }

    auto cellmlFile {libOpenCOR::CellmlFile::create(files.back())};

    EXPECT_FALSE(cellmlFile->hasIssues());
    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::ODE);
}

TEST(BasicCellmlTest, modelWithMissingInMemoryImports)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Importer: the attempt to resolve imports with the model at 'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/support/cellml/missing_imports/components/component.cellml' failed: the file could not be opened."},
        {libOpenCOR::Issue::Type::ERROR, "Analyser: the type of variable 'time' in component 'decay' is unknown."},
    }};

    auto file {libOpenCOR::File::create(std::string(libOpenCOR::REMOTE_BASE_PATH) + "/support/cellml/missing_imports/model.cellml", false)};

    file->setContents(libOpenCOR::fileContents(libOpenCOR::resourcePath("support/cellml/model_with_nested_imports/model.cellml")));

    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    EXPECT_EQ_ISSUES(cellmlFile, EXPECTED_ISSUES);
}

TEST(BasicCellmlTest, modelWithManagedImportsWithNoContents)
{
    static const std::vector<std::string> FILE_NAMES {"units.cellml", "components/component.cellml"};

    libOpenCOR::FilePtrs files;

    for (const auto &fileName : FILE_NAMES) {
        files.push_back(libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_nested_imports/" + fileName), false));
    }

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_nested_imports/model.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    EXPECT_FALSE(cellmlFile->hasIssues());
    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::ODE);
}

TEST(BasicCellmlTest, modelWithVariousImports)
{
    // A model that imports:
    //  - some units from a file in another directory, i.e. "../model_with_nested_imports/units.cellml", and this twice
    //    (using two different import elements);
    //  - a component from a file in another directory, which itself imports some units from the same file as above,
    //    albeit using a different URL, i.e. "../units.cellml";
    //  - a component from a file in the same directory, which itself imports some units from the same file as above,
    //    using the same URL as above; and
    //  - a component from a remote file, using an absolute URL.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_various_imports/model.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    EXPECT_FALSE(cellmlFile->hasIssues());
    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::ODE);
}

TEST(BasicCellmlTest, modelWithUnsupportedSchemeImport)
{
    // We cannot retrieve a file using a scheme other than "http", "https", or "file".

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_unsupported_scheme_import.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    GTEST_ASSERT_EQ(cellmlFile->issueCount(), 1U);
    EXPECT_EQ(cellmlFile->issue(0)->type(), libOpenCOR::Issue::Type::ERROR);
    EXPECT_TRUE(cellmlFile->issue(0)->description().ends_with("urn:miriam:biomodels.db:BIOMD0000000012' failed: the file could not be opened."));
}

TEST(BasicCellmlTest, modelWithInvalidXmlImport)
{
    // We don't add an imported model that is not valid XML to the library of the importer, so libCellML reports it.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_invalid_xml_import.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    GTEST_ASSERT_EQ(cellmlFile->issueCount(), 1U);
    EXPECT_EQ(cellmlFile->issue(0)->type(), libOpenCOR::Issue::Type::ERROR);
    EXPECT_TRUE(cellmlFile->issue(0)->description().ends_with("unknown_file.txt' failed: the file is not valid XML."));
}

TEST(BasicCellmlTest, modelWithInvalidCellmlImport)
{
    // An imported model that is valid XML, but not valid CellML (it has an unknown element), is added to the library of
    // the importer, so that its imports can be resolved, but its errors are reported.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_invalid_cellml_import/model.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    GTEST_ASSERT_EQ(cellmlFile->issueCount(), 1U);
    EXPECT_EQ(cellmlFile->issue(0)->type(), libOpenCOR::Issue::Type::ERROR);
    EXPECT_TRUE(cellmlFile->issue(0)->description().starts_with("Importer: the model at '"));
    EXPECT_TRUE(cellmlFile->issue(0)->description().ends_with("units.cellml' is invalid: Model 'units' has an invalid child element 'unknown_element'."));
}

TEST(BasicCellmlTest, modelWithNullCharacterImport)
{
    // A percent-encoded null character must not result in "units.cellml" being imported (see decodeUrl()).

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_null_character_import.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    GTEST_ASSERT_EQ(cellmlFile->issueCount(), 1U);
    EXPECT_EQ(cellmlFile->issue(0)->type(), libOpenCOR::Issue::Type::ERROR);
    EXPECT_TRUE(cellmlFile->issue(0)->description().ends_with("units.cellml%00.txt' failed: the file could not be opened."));
}

TEST(BasicCellmlTest, modelWithImportCycle)
{
    // We must not end up in an infinite loop when retrieving the imported models, and libCellML reports the cycle.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_import_cycle/a.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};

    GTEST_ASSERT_EQ(cellmlFile->issueCount(), 1U);
    EXPECT_EQ(cellmlFile->issue(0)->type(), libOpenCOR::Issue::Type::ERROR);
    EXPECT_TRUE(cellmlFile->issue(0)->description().starts_with("Importer: cyclic dependencies were found when attempting to resolve a component in the model 'a'."));
}

TEST(BasicCellmlTest, modelWithInvalidImports)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_invalid_imports.cellml"));
    auto cellmlFile = libOpenCOR::CellmlFile::create(file);

    EXPECT_EQ(cellmlFile->type(), libcellml::AnalyserModel::Type::UNKNOWN);
}

TEST(BasicCellmlTest, createCellmlFileWithNonCellmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("unknown_file.txt"));
    auto cellmlFile = libOpenCOR::CellmlFile::create(file);

    EXPECT_EQ(cellmlFile, nullptr);
}
