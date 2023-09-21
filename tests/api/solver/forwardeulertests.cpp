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

#include "solverforwardeuler.h"
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

#include "../../res/api/solver/model.h" // NOLINT
#include "../../res/api/solver/model.c" // NOLINT

#if defined(BUILDING_USING_MSVC)
#    pragma warning(pop)
#elif defined(BUILDING_USING_GNU)
#    pragma GCC diagnostic pop
#else
#    pragma clang diagnostic pop
#endif

namespace {
std::tuple<double *, double *, double *, std::shared_ptr<libOpenCOR::SolverForwardEuler>> createAndInitialiseArraysAndCreateSolver()
{
    double *states = createStatesArray();
    double *rates = createStatesArray();
    double *variables = createVariablesArray();

    initialiseVariables(states, rates, variables);
    computeComputedConstants(variables);
    computeRates(0.0, states, rates, variables);
    computeVariables(0.0, states, rates, variables);

    auto solver = std::static_pointer_cast<libOpenCOR::SolverForwardEuler>(libOpenCOR::Solver::create("Forward Euler"));

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
    // Create and initialise our various arrays and create our solver.

    const auto [states, rates, variables, solver] = createAndInitialiseArraysAndCreateSolver();

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("abc"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Step", "abc");

    EXPECT_FALSE(solver->initialise(STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(ForwardEulerSolverTest, invalidStepValue)
{
    // Create and initialise our various arrays and create our solver.

    const auto [states, rates, variables, solver] = createAndInitialiseArraysAndCreateSolver();

    // Customise and initialise our solver using an invalid step value.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("0.0"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Step", "0.0");

    EXPECT_FALSE(solver->initialise(STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}

TEST(ForwardEulerSolverTest, main)
{
    // Create and initialise our various arrays and create our solver.

    const auto [states, rates, variables, solver] = createAndInitialiseArraysAndCreateSolver();

    // Customise and initialise our solver.

    static const libOpenCOR::Doubles INITIAL_STATES = {0.0, 0.6, 0.05, 0.325};
#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015329449762314604, 0.59604909855484645, 0.053034873006546725, 0.31777429461290835};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015329449762310383, 0.59604909855484645, 0.053034873006546725, 0.31777429461290835};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015329449762310383, 0.59604909855484645, 0.053034873006546725, 0.31777429461290835};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015329449762310435, 0.59604909855484645, 0.053034873006546725, 0.31777429461290835};
#endif

    solver->setProperty("Step", "0.0123");

    EXPECT_TRUE(solver->initialise(STATE_COUNT, states, rates, variables, computeRates));
    EXPECT_EQ_DOUBLES(states, INITIAL_STATES);

    // Solve our model.

    static constexpr double VOI_START = 0.0;
    static constexpr double VOI_END = 50.0;
    static constexpr double VOI_INTERVAL = 0.1;

    double voi = VOI_START;
    size_t voiCounter = 0;

    while (!libOpenCOR::fuzzyCompare(voi, VOI_END)) {
        EXPECT_TRUE(solver->solve(voi, std::min(VOI_START + static_cast<double>(++voiCounter) * VOI_INTERVAL, VOI_END)));

        computeVariables(voi, states, rates, variables);
    }

    EXPECT_EQ_DOUBLES(states, FINAL_STATES);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}
