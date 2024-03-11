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

#include "tests/utils.h"

#include <libopencor>

TEST(RuntimeCellmlTest, validRuntime)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto cellmlFile = libOpenCOR::CellmlFile::create(file);
    auto cellmlFileRuntime = cellmlFile->runtime();

    EXPECT_FALSE(cellmlFileRuntime->hasIssues());
}

TEST(RuntimeCellmlTest, invalidRuntimeBecauseOfAnalysisIssues)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::WARNING, "The units in 'x = 1.0' in component 'my_component' are not equivalent. 'x' is in 'volt' (i.e. 'ampere^-1 x kilogram x metre^2 x second^-3') while '1.0' is 'dimensionless'."},
        {libOpenCOR::Issue::Type::ERROR, "Variable 'x' in component 'my_component' is computed more than once."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("support/cellml/model_with_analysis_issues.cellml"));
    auto cellmlFile = libOpenCOR::CellmlFile::create(file);
    auto cellmlFileRuntime = cellmlFile->runtime();

    EXPECT_EQ_ISSUES(cellmlFileRuntime, expectedIssues);
}
