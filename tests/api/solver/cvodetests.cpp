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

TEST(CvodeSolverTest, maximumStepValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum step cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setMaximumStep(RELATIVE_TOLERANCE);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, maximumNumberOfStepsValueWithInvalidNumber)
{
    static const auto MAXIMUM_NUMBER_OF_STEPS = 0;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum number of steps cannot be equal to 0. It must be greater than 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setMaximumNumberOfSteps(MAXIMUM_NUMBER_OF_STEPS);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    static const auto UPPER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    static const auto UPPER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    static const auto LOWER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    static const auto LOWER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, relativeToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setRelativeTolerance(RELATIVE_TOLERANCE);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, absoluteToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setAbsoluteTolerance(RELATIVE_TOLERANCE);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

void cvodeSolve(bool pCompiled)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto instance = sed->createInstance(pCompiled);

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, compiledSolve)
{
    cvodeSolve(true);
}

TEST(CvodeSolverTest, interpretedSolve)
{
    cvodeSolve(false);
}

TEST(CvodeSolverTest, solveWithoutInterpolateSolution)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setInterpolateSolution(false);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithAdamsMoultonIntegrationMethod)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIntegrationMethod(libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithFunctionalIterationType)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIterationType(libOpenCOR::SolverCvode::IterationType::FUNCTIONAL);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithDiagonalLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::DIAGONAL);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithGmresLinearSolverAndNoPreconditioner)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolverAndNoPreconditioner)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolverAndNoPreconditioner)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    auto instance = sed->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}
