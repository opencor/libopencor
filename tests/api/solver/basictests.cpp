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

TEST(BasicSolverTest, solversInfo)
{
    auto solversInfo = libOpenCOR::Solver::solversInfo();

    EXPECT_EQ(solversInfo.size(), 1);

    auto solverInfo = solversInfo[0];

    EXPECT_EQ(solverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solverInfo->name(), "Forward Euler");
    EXPECT_EQ(solverInfo->kisaoId(), "KISAO:0000030");

    auto properties = solverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    auto property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000483");
    EXPECT_EQ(property->defaultValue(), "1.000000");
    EXPECT_TRUE(property->hasVoiUnit());

    auto listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);
}

TEST(BasicSolverTest, unknownOdeSolver)
{
    auto odeSolver = libOpenCOR::SolverOde::create("Unknown");

    EXPECT_FALSE(odeSolver->isValid());
}

TEST(BasicSolverTest, forwardEulerByName)
{
    auto odeSolver = libOpenCOR::SolverOde::create("Forward Euler");

    EXPECT_TRUE(odeSolver->isValid());
}

TEST(BasicSolverTest, forwardEulerByKisaoId)
{
    auto odeSolver = libOpenCOR::SolverOde::create("KISAO:0000030");

    EXPECT_TRUE(odeSolver->isValid());
}

TEST(BasicSolverTest, properties)
{
    auto odeSolver = libOpenCOR::SolverOde::create("Forward Euler");
    auto properties = odeSolver->properties();

    EXPECT_EQ(odeSolver->properties().size(), 1);
    EXPECT_EQ(odeSolver->property("Step"), "1.000000");
    EXPECT_EQ(odeSolver->property("KISAO:0000483"), "1.000000");

    odeSolver->setProperty("Step", "1.2345");

    EXPECT_EQ(odeSolver->properties().size(), 1);
    EXPECT_EQ(odeSolver->property("KISAO:0000483"), "1.2345");

    odeSolver->setProperty("KISAO:0000483", "7.89");

    EXPECT_EQ(odeSolver->properties().size(), 1);
    EXPECT_EQ(odeSolver->property("Step"), "7.89");

    odeSolver->setProperty("Unknown property", "1.23");

    EXPECT_EQ(odeSolver->properties().size(), 1);
    EXPECT_EQ(odeSolver->property("Step"), "7.89");
    EXPECT_EQ(odeSolver->property("Unknown property"), "");

    odeSolver->setProperties(properties);

    EXPECT_EQ(odeSolver->properties().size(), 1);
    EXPECT_EQ(odeSolver->property("Step"), "1.000000");
}
