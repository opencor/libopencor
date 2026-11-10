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
