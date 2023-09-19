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

#include "utils.h"

#include "gtest/gtest.h"

#include "tests/utils.h"

#include <libopencor>

#if defined(BUILDING_USING_MSVC)
#    pragma warning(push)
#    pragma warning(disable: 4100)
#elif defined(BUILDING_USING_GNU)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdouble-promotion"
#    pragma clang diagnostic ignored "-Wmissing-prototypes"
#    pragma clang diagnostic ignored "-Wold-style-cast"
#    pragma clang diagnostic ignored "-Wunused-parameter"
#    pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include "../../res/hh52/c/model.h" // NOLINT
#include "../../res/hh52/c/model.c" // NOLINT

#if defined(BUILDING_USING_MSVC)
#    pragma warning(pop)
#elif defined(BUILDING_USING_GNU)
#    pragma GCC diagnostic pop
#else
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

    auto odeSolver = libOpenCOR::Solver::createOde("Forward Euler");

    return std::make_tuple(states, rates, variables, odeSolver);
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
    // Create and initialise our various arrays and create our ODE solver.

    const auto [states, rates, variables, odeSolver] = createAndInitialiseArraysAndCreateSolver();

    // Customise, and initialise our ODE solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("abc"). It must be a floating point number greater than zero.)"},
    };

    odeSolver->setProperty("Step", "abc");

    EXPECT_FALSE(odeSolver->initialise(STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(odeSolver, expectedIssues);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(ForwardEulerSolverTest, invalidStepValue)
{
    // Create and initialise our various arrays and create our ODE solver.

    const auto [states, rates, variables, odeSolver] = createAndInitialiseArraysAndCreateSolver();

    // Customise, and initialise our ODE solver using an invalid step value.

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("0.0"). It must be a floating point number greater than zero.)"},
    };

    odeSolver->setProperty("Step", "0.0");

    EXPECT_FALSE(odeSolver->initialise(STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(odeSolver, expectedIssues);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(ForwardEulerSolverTest, main)
{
    // Create and initialise our various arrays and create our ODE solver.

    const auto [states, rates, variables, odeSolver] = createAndInitialiseArraysAndCreateSolver();

    // Customise, and initialise our ODE solver.

    static const libOpenCOR::Doubles initialStates = {0.0, 0.6, 0.05, 0.325};
    static const libOpenCOR::Doubles finalStates = {-0.015329449762310435, 0.59604909855484645, 0.053034873006546725, 0.31777429461290835};

    odeSolver->setProperty("Step", "0.0123");

    EXPECT_TRUE(odeSolver->initialise(STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_DOUBLES(states, initialStates);

    // Solve our model.

    static const double voiStart = 0.0;
    static const double voiEnd = 50.0;
    static const double voiInterval = 0.1;

    double voi = 0.0;
    size_t voiCounter = 0;

    while (!libOpenCOR::fuzzyCompare(voi, voiEnd)) {
        odeSolver->solve(voi, std::min(voiStart + static_cast<double>(++voiCounter) * voiInterval, voiEnd));

        computeVariables(voi, states, rates, variables);
    }

    EXPECT_EQ_DOUBLES(states, finalStates);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}
