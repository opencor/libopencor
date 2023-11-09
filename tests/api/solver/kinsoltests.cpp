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

#include "nlamodel.h"

#include "tests/utils.h"

#include <libopencor>

TEST(KinsolSolverTest, maximumNumberOfIterationsValueWithInvalidNumber)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum number of iterations cannot be equal to 0. It must be greater than 0."},
    };

    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model2::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setMaximumNumberOfIterations(0);

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model2::VARIABLE_COUNT_2));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 2."},
    };

    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model2::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(-1);

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model2::VARIABLE_COUNT_2));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to 3. It must be between 0 and 2."},
    };

    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model2::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(3);

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model2::VARIABLE_COUNT_2));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 1."},
    };

    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model2::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(-1);

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model1::VARIABLE_COUNT_1));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to 2. It must be between 0 and 1."},
    };

    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model2::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(2);

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model1::VARIABLE_COUNT_1));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, solve)
{
    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model1::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    NlaModel::Model1::compute(variables);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, solveWithBandedLinearSolver)
{
    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model1::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(2);
    solver->setLowerHalfBandwidth(2);

    NlaModel::Model2::compute(variables);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, solveWithGmresLinearSolver)
{
    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model1::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::GMRES);

    NlaModel::Model1::compute(variables);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, solveWithBicgstabLinearSolver)
{
    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model1::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB);

    NlaModel::Model2::compute(variables);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, solveWithTfqmrLinearSolver)
{
    auto solver = libOpenCOR::SolverKinsol::create();
    auto *variables = NlaModel::Model1::initialise(static_pointer_cast<libOpenCOR::SolverNla>(solver).get());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::TFQMR);

    NlaModel::Model1::compute(variables);

    NlaModel::Model1::finalise(variables);
}
