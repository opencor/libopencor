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

void checkCvodeSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    EXPECT_EQ(pSolverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(pSolverInfo->id(), "KISAO:0000019");
    EXPECT_EQ(pSolverInfo->name(), "CVODE");

    auto properties = pSolverInfo->properties();

    EXPECT_EQ(properties.size(), 11);

    auto property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
    EXPECT_EQ(property->id(), "KISAO:0000467");
    EXPECT_EQ(property->name(), "Maximum step");
    EXPECT_EQ(property->defaultValue(), "0");
    EXPECT_TRUE(property->hasVoiUnit());

    auto listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[1];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGt0);
    EXPECT_EQ(property->id(), "KISAO:0000415");
    EXPECT_EQ(property->name(), "Maximum number of steps");
    EXPECT_EQ(property->defaultValue(), "500");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[2];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::List);
    EXPECT_EQ(property->id(), "KISAO:0000475");
    EXPECT_EQ(property->name(), "Integration method");
    EXPECT_EQ(property->defaultValue(), "BDF");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 2);
    EXPECT_EQ(listValues[0], "Adams-Moulton");
    EXPECT_EQ(listValues[1], "BDF");

    property = properties[3];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::List);
    EXPECT_EQ(property->id(), "KISAO:0000476");
    EXPECT_EQ(property->name(), "Iteration type");
    EXPECT_EQ(property->defaultValue(), "Newton");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 2);
    EXPECT_EQ(listValues[0], "Functional");
    EXPECT_EQ(listValues[1], "Newton");

    property = properties[4];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::List);
    EXPECT_EQ(property->id(), "KISAO:0000477");
    EXPECT_EQ(property->name(), "Linear solver");
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
    EXPECT_EQ(property->id(), "KISAO:0000478");
    EXPECT_EQ(property->name(), "Preconditioner");
    EXPECT_EQ(property->defaultValue(), "Banded");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 2);
    EXPECT_EQ(listValues[0], "None");
    EXPECT_EQ(listValues[1], "Banded");

    property = properties[6]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGe0);
    EXPECT_EQ(property->id(), "KISAO:0000479");
    EXPECT_EQ(property->name(), "Upper half-bandwidth");
    EXPECT_EQ(property->defaultValue(), "0");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[7]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGe0);
    EXPECT_EQ(property->id(), "KISAO:0000480");
    EXPECT_EQ(property->name(), "Lower half-bandwidth");
    EXPECT_EQ(property->defaultValue(), "0");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[8]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
    EXPECT_EQ(property->id(), "KISAO:0000209");
    EXPECT_EQ(property->name(), "Relative tolerance");
    EXPECT_EQ(property->defaultValue(), "1e-07");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[9]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
    EXPECT_EQ(property->id(), "KISAO:0000211");
    EXPECT_EQ(property->name(), "Absolute tolerance");
    EXPECT_EQ(property->defaultValue(), "1e-07");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);

    property = properties[10]; // NOLINT

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::Boolean);
    EXPECT_EQ(property->id(), "KISAO:0000481");
    EXPECT_EQ(property->name(), "Interpolate solution");
    EXPECT_EQ(property->defaultValue(), "True");
    EXPECT_FALSE(property->hasVoiUnit());

    listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);
}

void checkForwardEulerSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    EXPECT_EQ(pSolverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(pSolverInfo->id(), "KISAO:0000030");
    EXPECT_EQ(pSolverInfo->name(), "Forward Euler");

    auto properties = pSolverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    auto property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->id(), "KISAO:0000483");
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    auto listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);
}

void checkFourthOrderRungeKuttaSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    EXPECT_EQ(pSolverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(pSolverInfo->id(), "KISAO:0000032");
    EXPECT_EQ(pSolverInfo->name(), "Fourth-order Runge-Kutta");

    auto properties = pSolverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    auto property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->id(), "KISAO:0000483");
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    auto listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);
}

void checkHeunSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    EXPECT_EQ(pSolverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(pSolverInfo->id(), "KISAO:0000301");
    EXPECT_EQ(pSolverInfo->name(), "Heun");

    auto properties = pSolverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    auto property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->id(), "KISAO:0000483");
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    auto listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);
}

void checkSecondOrderRungeKuttaSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    EXPECT_EQ(pSolverInfo->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(pSolverInfo->id(), "KISAO:0000381");
    EXPECT_EQ(pSolverInfo->name(), "Second-order Runge-Kutta");

    auto properties = pSolverInfo->properties();

    EXPECT_EQ(properties.size(), 1);

    auto property = properties[0];

    EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGt0);
    EXPECT_EQ(property->id(), "KISAO:0000483");
    EXPECT_EQ(property->name(), "Step");
    EXPECT_EQ(property->defaultValue(), "1");
    EXPECT_TRUE(property->hasVoiUnit());

    auto listValues = property->listValues();

    EXPECT_EQ(listValues.size(), 0);
}
