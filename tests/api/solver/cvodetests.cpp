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

#include "odemodel.h"

#include "tests/utils.h"

#include <libopencor>

TEST(CvodeSolverTest, maximumStepValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum step cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setMaximumStep(RELATIVE_TOLERANCE);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, maximumNumberOfStepsValueWithInvalidNumber)
{
    static const auto MAXIMUM_NUMBER_OF_STEPS = 0;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum number of steps cannot be equal to 0. It must be greater than 0."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setMaximumNumberOfSteps(MAXIMUM_NUMBER_OF_STEPS);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    static const auto UPPER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    static const auto UPPER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    static const auto LOWER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    static const auto LOWER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, relativeToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setRelativeTolerance(RELATIVE_TOLERANCE);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, absoluteToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setAbsoluteTolerance(RELATIVE_TOLERANCE);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solve)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.0530351, 0.3177705}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.0000001, 0.0000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithoutInterpolateSolution)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419, 0.5960555, 0.053035143, 0.317770590}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.000001, 0.0000001, 0.000000001, 0.000000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setInterpolateSolution(false);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithAdamsMoultonIntegrationMethod)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419, 0.5960555, 0.0530351, 0.3177705}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.000001, 0.0000001, 0.0000001, 0.0000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setIntegrationMethod(libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithFunctionalIterationType)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01541, 0.5960555, 0.0530351, 0.317770}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.00001, 0.0000001, 0.0000001, 0.000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setIterationType(libOpenCOR::SolverCvode::IterationType::FUNCTIONAL);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBandedLinearSolver)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015, 0.596, 0.0530, 0.317}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.001, 0.001, 0.0001, 0.001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithDiagonalLinearSolver)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.053035, 0.317770}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.000001, 0.000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::DIAGONAL);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolver)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0150, 0.59605, 0.053032, 0.317773}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.00001, 0.000001, 0.000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolver)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0150, 0.59605, 0.0530327, 0.317773}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.00001, 0.0000001, 0.000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolver)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0150, 0.59605, 0.053032, 0.317773}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.00001, 0.000001, 0.000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolverAndNoPreconditioner)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.0530351, 0.3177705}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.0000001, 0.0000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolverAndNoPreconditioner)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.0530351, 0.3177705}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.0000001, 0.0000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolverAndNoPreconditioner)
{
    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.053035, 0.3177705}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.000001, 0.0000001};

    auto solver = libOpenCOR::SolverCvode::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}
