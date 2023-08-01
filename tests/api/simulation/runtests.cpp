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

TEST(RunSimulationTest, runUnknownFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    simulation->run();
}

TEST(RunSimulationTest, runCellmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    simulation->run();
}

TEST(RunSimulationTest, runSedmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    simulation->run();
}

TEST(RunSimulationTest, runCombineArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));
    auto simulation = libOpenCOR::Simulation::create(file);

    simulation->run();
}

TEST(RunSimulationTest, runIrretrievableFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::IRRETRIEVABLE_FILE));
    auto simulation = libOpenCOR::Simulation::create(file);

    simulation->run();
}
