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

TEST(CoverageLoggerTest, issue)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_EQ(nullptr, simulation->issue(0));
}

TEST(CoverageLoggerTest, hasErrors)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_FALSE(simulation->hasErrors());
}

TEST(CoverageLoggerTest, errorCount)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_EQ(0, simulation->errorCount());
}

TEST(CoverageLoggerTest, error)
{
    // Has an error.

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    EXPECT_NE(nullptr, simulation->error(0));

    // Doesn't have an error.

    file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    simulation = libOpenCOR::Simulation::create(file);

    EXPECT_EQ(nullptr, simulation->error(0));
}