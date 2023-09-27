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

    EXPECT_EQ(solversInfo.size(), 5);

    // CVODE.

    auto solverInfo = solversInfo[0];

    EXPECT_EQ(solverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solverInfo->name(), "CVODE");
    EXPECT_EQ(solverInfo->kisaoId(), "KISAO:0000019");

    auto properties = solverInfo->properties();

    EXPECT_EQ(properties.size(), 11);

    auto property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
    EXPECT_EQ(property->name(), "Maximum step");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000467");
    EXPECT_EQ(property->defaultValue(), "0");
    EXPECT_TRUE(property->hasVoiUnit());

    auto listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[1];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGt0);
    EXPECT_EQ(property->name(), "Maximum number of steps");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000415");
    EXPECT_EQ(property->defaultValue(), "500");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[2];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::List);
    EXPECT_EQ(property->name(), "Integration method");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000475");
    EXPECT_EQ(property->defaultValue(), "BDF");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 2);
    EXPECT_EQ(listValues[0], "Adams-Moulton");
    EXPECT_EQ(listValues[1], "BDF");

    property = properties[3];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::List);
    EXPECT_EQ(property->name(), "Iteration type");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000476");
    EXPECT_EQ(property->defaultValue(), "Newton");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 2);
    EXPECT_EQ(listValues[0], "Functional");
    EXPECT_EQ(listValues[1], "Newton");

    property = properties[4];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::List);
    EXPECT_EQ(property->name(), "Linear solver");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000477");
    EXPECT_EQ(property->defaultValue(), "Dense");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 6);
    EXPECT_EQ(listValues[0], "Dense");
    EXPECT_EQ(listValues[1], "Banded");
    EXPECT_EQ(listValues[2], "Diagonal");
    EXPECT_EQ(listValues[3], "GMRES");
    EXPECT_EQ(listValues[4], "BiCGStab");
    EXPECT_EQ(listValues[5], "TFQMR");

    property = properties[5]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::List);
    EXPECT_EQ(property->name(), "Preconditioner");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000478");
    EXPECT_EQ(property->defaultValue(), "Banded");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 2);
    EXPECT_EQ(listValues[0], "None");
    EXPECT_EQ(listValues[1], "Banded");

    property = properties[6]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGe0);
    EXPECT_EQ(property->name(), "Upper half-bandwidth");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000479");
    EXPECT_EQ(property->defaultValue(), "0");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[7]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGe0);
    EXPECT_EQ(property->name(), "Lower half-bandwidth");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000480");
    EXPECT_EQ(property->defaultValue(), "0");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[8]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
    EXPECT_EQ(property->name(), "Relative tolerance");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000209");
    EXPECT_EQ(property->defaultValue(), "1e-07");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[9]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
    EXPECT_EQ(property->name(), "Absolute tolerance");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000211");
    EXPECT_EQ(property->defaultValue(), "1e-07");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[10]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::Boolean);
    EXPECT_EQ(property->name(), "Interpolate solution");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000481");
    EXPECT_EQ(property->defaultValue(), "True");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    // Forward Euler.

    solverInfo = solversInfo[1];

    EXPECT_EQ(solverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solverInfo->name(), "Forward Euler");
    EXPECT_EQ(solverInfo->kisaoId(), "KISAO:0000030");

    properties = solverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000483");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    // Fourth-order Runge-Kutta.

    solverInfo = solversInfo[2];

    EXPECT_EQ(solverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solverInfo->name(), "Fourth-order Runge-Kutta");
    EXPECT_EQ(solverInfo->kisaoId(), "KISAO:0000032");

    properties = solverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000483");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    // Heun.

    solverInfo = solversInfo[3];

    EXPECT_EQ(solverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solverInfo->name(), "Heun");
    EXPECT_EQ(solverInfo->kisaoId(), "KISAO:0000301");

    properties = solverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000483");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    // Second-order Runge-Kutta.

    solverInfo = solversInfo[4];

    EXPECT_EQ(solverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solverInfo->name(), "Second-order Runge-Kutta");
    EXPECT_EQ(solverInfo->kisaoId(), "KISAO:0000381");

    properties = solverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->kisaoId(), "KISAO:0000483");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);
}

TEST(BasicSolverTest, unknownSolver)
{
    auto solver = libOpenCOR::Solver::create("Unknown");

    EXPECT_FALSE(solver->isValid());
}

TEST(BasicSolverTest, cvodeByName)
{
    auto solver = libOpenCOR::Solver::create("CVODE");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, cvodeByKisaoId)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000019");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, forwardEulerByName)
{
    auto solver = libOpenCOR::Solver::create("Forward Euler");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, forwardEulerByKisaoId)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000030");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, fourthOrderRungeKuttaByName)
{
    auto solver = libOpenCOR::Solver::create("Fourth-order Runge-Kutta");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, fourthOrderRungeKuttaByKisaoId)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000032");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, heunByName)
{
    auto solver = libOpenCOR::Solver::create("Heun");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, heunByKisaoId)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000301");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, secondOrderRungeKuttaByName)
{
    auto solver = libOpenCOR::Solver::create("Second-order Runge-Kutta");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, secondOrderRungeKuttaByKisaoId)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000381");

    EXPECT_TRUE(solver->isValid());
}

TEST(BasicSolverTest, properties)
{
    auto solver = libOpenCOR::Solver::create("Forward Euler");
    auto properties = solver->properties();

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1");
    EXPECT_EQ(solver->property("KISAO:0000483"), "1");

    solver->setProperty("Step", "1.2345");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("KISAO:0000483"), "1.2345");

    solver->setProperty("KISAO:0000483", "7.89");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "7.89");

    solver->setProperty("Unknown property", "1.23");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "7.89");
    EXPECT_EQ(solver->property("Unknown property"), "");

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1");
}
