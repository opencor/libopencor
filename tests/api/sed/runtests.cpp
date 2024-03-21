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

TEST(RunSedDocumentTest, noFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The simulation experiment description does not contain any tasks to run."},
    };

    auto sed = libOpenCOR::SedDocument::create();
    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(RunSedDocumentTest, invalidCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is invalid."},
        {libOpenCOR::Issue::Type::ERROR, "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS)."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_CELLML_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(RunSedDocumentTest, overconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is overconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Variable 'x' in component 'my_component' is computed more than once."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/overconstrained.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(RunSedDocumentTest, underconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is underconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "The type of variable 'x' in component 'my_component' is unknown."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/underconstrained.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(RunSedDocumentTest, unsuitablyConstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is unsuitably constrained."},
        {libOpenCOR::Issue::Type::ERROR, "Variable 'y' in component 'my_component' is computed more than once."},
        {libOpenCOR::Issue::Type::ERROR, "The type of variable 'x' in component 'my_component' is unknown."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unsuitably_constrained.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(RunSedDocumentTest, algebraicModel)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto instance = sed->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(RunSedDocumentTest, odeModel)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "At t = 0.00140014, mxstep steps taken before reaching tout."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);

    static const auto NOK_MAXIMUM_NUMBER_OF_STEPS = 10;

    auto cvode = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    cvode->setMaximumNumberOfSteps(NOK_MAXIMUM_NUMBER_OF_STEPS);

    auto instance = sed->createInstance();

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    static const auto OK_MAXIMUM_NUMBER_OF_STEPS = 500;

    cvode->setMaximumNumberOfSteps(OK_MAXIMUM_NUMBER_OF_STEPS);

    instance = sed->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(RunSedDocumentTest, odeModelWithNoOdeSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);

    sed->simulations()[0]->setOdeSolver(nullptr);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(RunSedDocumentTest, nlaModel)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(sed->simulations()[0]);
    auto kinsol = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = sed->createInstance();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(RunSedDocumentTest, nlaModelWithNoNlaSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);

    sed->simulations()[0]->setNlaSolver(nullptr);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(RunSedDocumentTest, daeModel)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto kinsol = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    instance->run();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = sed->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(RunSedDocumentTest, daeModelWithNoOdeOrNlaSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver."},
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];

    simulation->setOdeSolver(nullptr);
    simulation->setNlaSolver(nullptr);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}
