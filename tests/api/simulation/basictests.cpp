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

TEST(BasicSimulationTest, unknownFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation cannot be created using an unknown file."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_EQ_ISSUES(expectedIssues, simulation);
}

TEST(BasicSimulationTest, cellmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_FALSE(simulation->hasIssues());
}

TEST(BasicSimulationTest, sedmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation cannot currently be created using a SED-ML file."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_EQ_ISSUES(expectedIssues, simulation);
}

TEST(BasicSimulationTest, combineArchive)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation cannot currently be created using a COMBINE archive."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_EQ_ISSUES(expectedIssues, simulation);
}

TEST(BasicSimulationTest, irretrievableFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation cannot be created using an irretrievable file."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::IRRETRIEVABLE_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_EQ_ISSUES(expectedIssues, simulation);
}
