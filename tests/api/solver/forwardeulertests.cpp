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

#ifdef BUILDING_USING_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdouble-promotion"
#    pragma clang diagnostic ignored "-Wmissing-prototypes"
#    pragma clang diagnostic ignored "-Wold-style-cast"
#    pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#    pragma clang diagnostic ignored "-Wunused-parameter"
#    pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include "../../res/hh52/c/model.h" // NOLINT
#include "../../res/hh52/c/model.c" // NOLINT

#ifdef BUILDING_USING_CLANG
#    pragma clang diagnostic pop
#endif

namespace {
std::tuple<double *, double *, double *, libOpenCOR::SolverOdePtr> createAndInitialiseArraysAndCreateSolver()
{
    double *states = createStatesArray();
    double *rates = createStatesArray();
    double *variables = createVariablesArray();

    initialiseVariables(states, rates, variables);
    computeComputedConstants(variables);
    computeRates(0.0, states, rates, variables);
    computeVariables(0.0, states, rates, variables);

    auto solver = std::static_pointer_cast<libOpenCOR::SolverOde>(libOpenCOR::Solver::create("Forward Euler"));

    return std::make_tuple(states, rates, variables, solver);
}

void deleteArrays(double *pStates, double *pRates, double *pVariables)
{
    deleteArray(pStates);
    deleteArray(pRates);
    deleteArray(pVariables);
}
} // namespace

TEST(ForwardEulerSolverTest, nonFloatingPointStepValue)
{
    // Create and initialise our various arrays.

    const auto [states, rates, variables, solver] = createAndInitialiseArraysAndCreateSolver();

    // Customise, and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The 'Step' property has an invalid value ('abc'). It must be a floating point number greater than zero."},
    };

    solver->setProperty("Step", "abc");

    EXPECT_FALSE(solver->initialise(states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(solver, expectedIssues);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(ForwardEulerSolverTest, invalidStepValue)
{
    // Create and initialise our various arrays.

    const auto [states, rates, variables, solver] = createAndInitialiseArraysAndCreateSolver();

    // Customise, and initialise our solver using an invalid step value.

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The 'Step' property has an invalid value ('0.0'). It must be a floating point number greater than zero."},
    };

    solver->setProperty("Step", "0.0");

    EXPECT_FALSE(solver->initialise(states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(solver, expectedIssues);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(ForwardEulerSolverTest, main)
{
    // Create and initialise our various arrays.

    const auto [states, rates, variables, solver] = createAndInitialiseArraysAndCreateSolver();

    // Customise, and initialise our solver.

    solver->setProperty("Step", "0.01");

    EXPECT_TRUE(solver->initialise(states, rates, variables, computeRates));

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}
