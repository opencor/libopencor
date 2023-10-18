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

#include "model.h"
#include "solverfourthorderrungekutta.h"
#include "solvers.h"
#include "utils.h"

#include "tests/utils.h"

#include <libopencor>

TEST(FourthOrderRungeKuttaSolverTest, basic)
{
    auto solver = std::static_pointer_cast<libOpenCOR::SolverOde>(libOpenCOR::Solver::create("Fourth-order Runge-Kutta"));

    checkFourthOrderRungeKuttaSolver(solver->info());
}

TEST(FourthOrderRungeKuttaSolverTest, stepValueWithNonNumber)
{
    // Create and initialise our various arrays and create our solver.

    const auto [solver, states, rates, variables] = createAndInitialiseArraysAndCreateSolver("Fourth-order Runge-Kutta");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("abc"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Step", "abc");

    EXPECT_FALSE(solver->initialise(0.0, STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(FourthOrderRungeKuttaSolverTest, stepValueWithInvalidNumber)
{
    // Create and initialise our various arrays and create our solver.

    const auto [solver, states, rates, variables] = createAndInitialiseArraysAndCreateSolver("Fourth-order Runge-Kutta");

    // Customise and initialise our solver using an invalid step value.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("0.0"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Step", "0.0");

    EXPECT_FALSE(solver->initialise(0.0, STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(FourthOrderRungeKuttaSolverTest, solve)
{
    // Create and initialise our various arrays and create our solver.

    const auto [solver, states, rates, variables] = createAndInitialiseArraysAndCreateSolver("Fourth-order Runge-Kutta");

    // Customise our solver and compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.01538470269835, 0.596055526314677, 0.053034933968590, 0.31777081396617829};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.00000000000001, 0.000000000000001, 0.000000000000001, 0.00000000000000001};

    solver->setProperty("Step", "0.0123");

    computeModel(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}
