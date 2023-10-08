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

#include "solvers.h"

#include "gtest/gtest.h"

#include <libopencor>

TEST(BasicSolverTest, solversInfo)
{
    auto solversInfo = libOpenCOR::Solver::solversInfo();

    EXPECT_EQ(solversInfo.size(), 5);

    checkCvodeSolver(solversInfo[0]);
    checkForwardEulerSolver(solversInfo[1]);
    checkFourthOrderRungeKuttaSolver(solversInfo[2]);
    checkHeunSolver(solversInfo[3]);
    checkSecondOrderRungeKuttaSolver(solversInfo[4]);
}

TEST(BasicSolverTest, unknownSolver)
{
    auto solver = libOpenCOR::Solver::create("Unknown");

    EXPECT_FALSE(solver->isValid());
}

TEST(BasicSolverTest, cvodeById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000019");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, cvodeByName)
{
    auto solver = libOpenCOR::Solver::create("CVODE");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, forwardEulerById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000030");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, forwardEulerByName)
{
    auto solver = libOpenCOR::Solver::create("Forward Euler");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, fourthOrderRungeKuttaById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000032");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, fourthOrderRungeKuttaByName)
{
    auto solver = libOpenCOR::Solver::create("Fourth-order Runge-Kutta");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, heunById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000301");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, heunByName)
{
    auto solver = libOpenCOR::Solver::create("Heun");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, secondOrderRungeKuttaById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000381");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, secondOrderRungeKuttaByName)
{
    auto solver = libOpenCOR::Solver::create("Second-order Runge-Kutta");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, properties)
{
    auto solver = libOpenCOR::Solver::create("Forward Euler");
    auto properties = solver->properties();

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1");
    EXPECT_EQ(solver->property("KISAO:0000483"), "1");

    solver->setProperty("Step", "1.23");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("KISAO:0000483"), "1.23");

    solver->setProperty("KISAO:0000483", "7.89");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "7.89");

    solver->setProperty("Unknown", "1.23");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "7.89");
    EXPECT_EQ(solver->property("Unknown"), "");

    properties["Step"] = "1.23";

    EXPECT_EQ(properties.size(), 2);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1");

    properties["Unknown"] = "1.23";

    EXPECT_EQ(properties.size(), 3);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1");

    properties["KISAO:0000483"] = "1.23";

    EXPECT_EQ(properties.size(), 3);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1.23");

    properties.erase("KISAO:0000483");

    EXPECT_EQ(properties.size(), 2);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1.23");
}
