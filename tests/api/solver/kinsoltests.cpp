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
#include "solvers.h"

#include "gtest/gtest.h"

#include "tests/utils.h"

#include <libopencor>

TEST(KinsolSolverTest, basic)
{
    auto solver = std::static_pointer_cast<libOpenCOR::SolverNla>(libOpenCOR::Solver::create("KINSOL"));

    checkKinsolSolver(solver->info());
}

TEST(KinsolSolverTest, maximumNumberOfIterationsValueWithNonNumber)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum number of iterations" property has an invalid value ("abc"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Maximum number of iterations", "abc");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model1::VARIABLE_COUNT_1));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, maximumNumberOfIterationsValueWithInvalidNumber)
{
    const auto [solver, variables] = NlaModel::Model2::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum number of iterations" property has an invalid value ("0"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Maximum number of iterations", "0");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model2::VARIABLE_COUNT_2));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, linearSolverValueWithUnknownValue)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Linear solver" property has an invalid value ("Unknown linear solver"). It must be equal to either "Dense", "Banded", "GMRES", "BiCGStab", or "TFQMR".)"},
    };

    solver->setProperty("Linear solver", "Unknown linear solver");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model1::VARIABLE_COUNT_1));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNonNumber)
{
    const auto [solver, variables] = NlaModel::Model2::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 2.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "abc");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model2::VARIABLE_COUNT_2));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    const auto [solver, variables] = NlaModel::Model2::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("-1"). It must be an integer number between 0 and 2.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "-1");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model2::VARIABLE_COUNT_2));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    const auto [solver, variables] = NlaModel::Model2::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("3"). It must be an integer number between 0 and 2.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "3");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model2::VARIABLE_COUNT_2));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNonNumber)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 1.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "abc");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model1::VARIABLE_COUNT_1));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("-1"). It must be an integer number between 0 and 1.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "-1");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model1::VARIABLE_COUNT_1));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("2"). It must be an integer number between 0 and 1.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "2");

    EXPECT_FALSE(solver->solve(nullptr, variables, NlaModel::Model1::VARIABLE_COUNT_1));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, solve)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    NlaModel::Model1::compute(variables);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, solveWithBandedLinearSolver)
{
    const auto [solver, variables] = NlaModel::Model2::initialise("KINSOL");

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "2");
    solver->setProperty("Lower half-bandwidth", "2");

    NlaModel::Model2::compute(variables);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, solveWithGmresLinearSolver)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    solver->setProperty("Linear solver", "GMRES");

    NlaModel::Model1::compute(variables);

    NlaModel::Model1::finalise(variables);
}

TEST(KinsolSolverTest, solveWithBicgstabLinearSolver)
{
    const auto [solver, variables] = NlaModel::Model2::initialise("KINSOL");

    solver->setProperty("Linear solver", "BiCGStab");

    NlaModel::Model2::compute(variables);

    NlaModel::Model2::finalise(variables);
}

TEST(KinsolSolverTest, solveWithTfqmrLinearSolver)
{
    const auto [solver, variables] = NlaModel::Model1::initialise("KINSOL");

    solver->setProperty("Linear solver", "TFQMR");

    NlaModel::Model1::compute(variables);

    NlaModel::Model1::finalise(variables);
}
