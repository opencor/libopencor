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

TEST(FailingSimulationTest, basic)
{
    // Default simulation.

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("misc/failablesimulation.omex"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->instantiate();

    EXPECT_NE(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());

    // Set the value of `membrane.test` to zero and rerun the simulation.

    auto model = document->model(0);

    model->addChange(libOpenCOR::SedChangeAttribute::create("membrane", "test", "0.0"));

    EXPECT_EQ(instance->run(), 0.0);
    EXPECT_TRUE(instance->hasIssues());

    // Remove all changes and rerun the simulation.

    model->removeAllChanges();

    EXPECT_NE(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
}
