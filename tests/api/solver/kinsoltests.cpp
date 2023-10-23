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
#include "solverkinsol.h"
#include "solvers.h"
#include "utils.h"

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
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum number of iterations" property has an invalid value ("abc"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Maximum number of iterations", "abc");

    EXPECT_FALSE(solver->initialise(nullptr, variables, NlaModel::VARIABLE_COUNT));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, maximumNumberOfIterationsValueWithInvalidNumber)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum number of iterations" property has an invalid value ("0"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Maximum number of iterations", "0");

    EXPECT_FALSE(solver->initialise(nullptr, variables, NlaModel::VARIABLE_COUNT));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, linearSolverValueWithUnknownValue)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Linear solver" property has an invalid value ("Unknown linear solver"). It must be equal to either "Dense", "Banded", "GMRES", "BiCGStab", or "TFQMR".)"},
    };

    solver->setProperty("Linear solver", "Unknown linear solver");

    EXPECT_FALSE(solver->initialise(nullptr, variables, NlaModel::VARIABLE_COUNT));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNonNumber)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "abc");

    EXPECT_FALSE(solver->initialise(nullptr, variables, NlaModel::VARIABLE_COUNT));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNonNumber)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "abc");

    EXPECT_FALSE(solver->initialise(nullptr, variables, NlaModel::VARIABLE_COUNT));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    NlaModel::finalise(variables);
}

static const libOpenCOR::Doubles SOLUTIONS = {3.0, -2.0, 1.0};
static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0, 0.0, 0.0};

TEST(KinsolSolverTest, solve)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    NlaModel::compute(solver, variables, SOLUTIONS, ABSOLUTE_ERRORS);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, solveWithBandedLinearSolver)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    solver->setProperty("Linear solver", "Banded");

    NlaModel::compute(solver, variables, SOLUTIONS, ABSOLUTE_ERRORS);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, solveWithGmresLinearSolver)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    solver->setProperty("Linear solver", "GMRES");

    NlaModel::compute(solver, variables, SOLUTIONS, ABSOLUTE_ERRORS);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, solveWithBicgstabLinearSolver)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    solver->setProperty("Linear solver", "BiCGStab");

    NlaModel::compute(solver, variables, SOLUTIONS, ABSOLUTE_ERRORS);

    NlaModel::finalise(variables);
}

TEST(KinsolSolverTest, solveWithTfqmrLinearSolver)
{
    const auto [solver, variables] = NlaModel::initialise("KINSOL");

    solver->setProperty("Linear solver", "TFQMR");

    NlaModel::compute(solver, variables, SOLUTIONS, ABSOLUTE_ERRORS);

    NlaModel::finalise(variables);
}
