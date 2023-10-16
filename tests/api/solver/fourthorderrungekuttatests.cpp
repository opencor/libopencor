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
#include "solverfourthorderrungekutta.h"
#include "solvers.h"
#include "utils.h"

#include "gtest/gtest.h"

#include "tests/utils.h"

#include <libopencor>

TEST(FourthOrderRungeKuttaSolverTest, basic)
{
    auto solver = std::static_pointer_cast<libOpenCOR::SolverOde>(libOpenCOR::Solver::create("Fourth-order Runge-Kutta"));

    checkFourthOrderRungeKuttaSolver(solver->info());
}

TEST(FourthOrderRungeKuttaSolverTest, stepValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("Fourth-order Runge-Kutta");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("abc"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Step", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(FourthOrderRungeKuttaSolverTest, stepValueWithInvalidNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("Fourth-order Runge-Kutta");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Step" property has an invalid value ("0.0"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Step", "0.0");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(FourthOrderRungeKuttaSolverTest, solve)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("Fourth-order Runge-Kutta");

    // Customise our solver and compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015384702698352787, 0.59605552631467773, 0.05303493396859079, 0.31777081396617829};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015384702698352785, 0.59605552631467773, 0.05303493396859079, 0.31777081396617829};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015384702698352775, 0.59605552631467773, 0.05303493396859079, 0.31777081396617829};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01538470269835253, 0.59605552631467773, 0.05303493396859079, 0.31777081396617829};
#endif

    solver->setProperty("Step", "0.0123");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);

    OdeModel::finalise(states, rates, variables);
}
