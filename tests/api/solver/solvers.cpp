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

#include "tests/utils.h"

static const std::map<std::string, std::string> NoProperties;
static const std::vector<std::string> NoHiddenProperties;

void checkCvodeSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    // Properties.

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

        EXPECT_TRUE(listValues.empty());

        property = properties[1];

        EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGt0);
        EXPECT_EQ(property->id(), "KISAO:0000415");
        EXPECT_EQ(property->name(), "Maximum number of steps");
        EXPECT_EQ(property->defaultValue(), "500");
        EXPECT_FALSE(property->hasVoiUnit());

        listValues = property->listValues();

        EXPECT_TRUE(listValues.empty());

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

        EXPECT_TRUE(listValues.empty());

        property = properties[7]; // NOLINT

        EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::IntegerGe0);
        EXPECT_EQ(property->id(), "KISAO:0000480");
        EXPECT_EQ(property->name(), "Lower half-bandwidth");
        EXPECT_EQ(property->defaultValue(), "0");
        EXPECT_FALSE(property->hasVoiUnit());

        listValues = property->listValues();

        EXPECT_TRUE(listValues.empty());

        property = properties[8]; // NOLINT

        EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
        EXPECT_EQ(property->id(), "KISAO:0000209");
        EXPECT_EQ(property->name(), "Relative tolerance");
        EXPECT_EQ(property->defaultValue(), "1e-07");
        EXPECT_FALSE(property->hasVoiUnit());

        listValues = property->listValues();

        EXPECT_TRUE(listValues.empty());

        property = properties[9]; // NOLINT

        EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::DoubleGe0);
        EXPECT_EQ(property->id(), "KISAO:0000211");
        EXPECT_EQ(property->name(), "Absolute tolerance");
        EXPECT_EQ(property->defaultValue(), "1e-07");
        EXPECT_FALSE(property->hasVoiUnit());

        listValues = property->listValues();

        EXPECT_TRUE(listValues.empty());

        property = properties[10]; // NOLINT

        EXPECT_EQ(property->type(), libOpenCOR::SolverProperty::Type::Boolean);
        EXPECT_EQ(property->id(), "KISAO:0000481");
        EXPECT_EQ(property->name(), "Interpolate solution");
        EXPECT_EQ(property->defaultValue(), "True");
        EXPECT_FALSE(property->hasVoiUnit());

        listValues = property->listValues();

        EXPECT_TRUE(listValues.empty());
    }

    // Hidden properties.

    {
        static const std::vector<std::string> HiddenPropertiesForNewton = NoHiddenProperties;
        static const std::vector<std::string> HiddenPropertiesForNewtonDense = {"KISAO:0000478", "KISAO:0000479", "KISAO:0000480"};
        static const std::vector<std::string> HiddenPropertiesForNewtonBanded = {"KISAO:0000478"};
        static const std::vector<std::string> HiddenPropertiesForNewtonGmres = NoHiddenProperties;
        static const std::vector<std::string> HiddenPropertiesForNewtonGmresNone = {"KISAO:0000479", "KISAO:0000480"};
        static const std::vector<std::string> HiddenPropertiesForNewtonGmresBanded = NoHiddenProperties;
        static const std::vector<std::string> HiddenPropertiesForNewtonBicgstab = NoHiddenProperties;
        static const std::vector<std::string> HiddenPropertiesForNewtonBicgstabNone = HiddenPropertiesForNewtonGmresNone;
        static const std::vector<std::string> HiddenPropertiesForNewtonBicgstabBanded = NoHiddenProperties;
        static const std::vector<std::string> HiddenPropertiesForNewtonTfqmr = NoHiddenProperties;
        static const std::vector<std::string> HiddenPropertiesForNewtonTfqmrNone = HiddenPropertiesForNewtonGmresNone;
        static const std::vector<std::string> HiddenPropertiesForNewtonTfqmrBanded = NoHiddenProperties;
        static const std::vector<std::string> HiddenPropertiesForNewtonDiagonal = HiddenPropertiesForNewtonDense;
        static const std::vector<std::string> HiddenPropertiesForFunctional = {"KISAO:0000477", "KISAO:0000478", "KISAO:0000479", "KISAO:0000480"};

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(NoProperties), NoHiddenProperties);

        std::map<std::string, std::string> properties;

        properties["Iteration type"] = "Newton";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewton);

        properties["KISAO:0000477"] = "Dense";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonDense);

        properties["Linear solver"] = "Banded";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonDense);

        properties["KISAO:0000477"] = "Banded";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonBanded);

        properties["KISAO:0000477"] = "Diagonal";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonDiagonal);

        properties["KISAO:0000477"] = "GMRES";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonGmres);

        properties["KISAO:0000477"] = "BiCGStab";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonBicgstab);

        properties["KISAO:0000477"] = "TFQMR";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonTfqmr);

        properties["KISAO:0000477"] = "GMRES";
        properties["Preconditioner"] = "None";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonGmresNone);

        properties["KISAO:0000477"] = "BiCGStab";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonBicgstabNone);

        properties["KISAO:0000477"] = "TFQMR";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonTfqmrNone);

        properties["KISAO:0000477"] = "GMRES";
        properties["KISAO:0000478"] = "Banded";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonGmresBanded);

        properties["KISAO:0000477"] = "BiCGStab";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonBicgstabBanded);

        properties["KISAO:0000477"] = "TFQMR";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForNewtonTfqmrBanded);

        properties["KISAO:0000476"] = "Functional";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForFunctional);

        properties["Iteration type"] = "Newton";

        EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(properties), HiddenPropertiesForFunctional);
    }
}

void checkForwardEulerSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    // Properties.

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

    EXPECT_TRUE(listValues.empty());

    // Hidden properties.

    EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(NoProperties), NoHiddenProperties);
}

void checkFourthOrderRungeKuttaSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    // Properties.

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

    EXPECT_TRUE(listValues.empty());

    // Hidden properties.

    EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(NoProperties), NoHiddenProperties);
}

void checkHeunSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    // Properties.

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

    EXPECT_TRUE(listValues.empty());

    // Hidden properties.

    EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(NoProperties), NoHiddenProperties);
}

void checkSecondOrderRungeKuttaSolver(const libOpenCOR::SolverInfoPtr &pSolverInfo)
{
    // Properties.

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

    EXPECT_TRUE(listValues.empty());

    // Hidden properties.

    EXPECT_EQ_HIDDEN_PROPERTIES(pSolverInfo->hiddenProperties(NoProperties), NoHiddenProperties);
}
