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

#include <libopencor>

TEST(CoverageSedDocumentTest, initialise)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\"/>\n";

    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_EQ(sed->serialise(), expectedSerialisation);
}

TEST(CoverageSedDocumentTest, sedDocumentSimulations)
{
    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_TRUE(sed->simulations().empty());
    EXPECT_FALSE(sed->addSimulation(nullptr));

    auto simulation = libOpenCOR::SedSimulationUniformTimeCourse::create(sed);

    EXPECT_TRUE(sed->addSimulation(simulation));

    EXPECT_EQ(sed->simulations().size(), 1);
    EXPECT_EQ(sed->simulations()[0], simulation);

    EXPECT_FALSE(sed->addSimulation(simulation));
    EXPECT_TRUE(sed->removeSimulation(simulation));

    EXPECT_TRUE(sed->simulations().empty());

    EXPECT_FALSE(sed->removeSimulation(nullptr));
}
