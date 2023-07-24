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

TEST(CoverageFileTest, httpRemoteFile)
{
    libOpenCOR::File::create(libOpenCOR::HTTP_REMOTE_FILE);
}

TEST(CoverageFileTest, irretrievableRemoteFile)
{
    libOpenCOR::File::create(libOpenCOR::IRRETRIEVABLE_REMOTE_FILE);
}

TEST(CoverageFileTest, errorCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS)."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::UNIX_LOCAL_FILE,
                                         libOpenCOR::charArrayToVector(libOpenCOR::SOME_ERROR_CELLML_CONTENTS));

    EXPECT_EQ_ISSUES(expectedIssues, file);
}

TEST(CoverageFileTest, warningCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::WARNING, "The units in 'metre = 1.0' in component 'main' are not equivalent. 'metre' is in 'metre' while '1.0' is 'dimensionless'."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::UNIX_LOCAL_FILE,
                                         libOpenCOR::charArrayToVector(libOpenCOR::SOME_WARNING_CELLML_CONTENTS));

    EXPECT_EQ_ISSUES(expectedIssues, file);
}
