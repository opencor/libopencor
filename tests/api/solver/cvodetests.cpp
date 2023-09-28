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
#include "solvercvode.h"
#include "utils.h"

#include "gtest/gtest.h"

#include "tests/utils.h"

#include <libopencor>

/*---GRY---
TEST(CvodeSolverTest, stepValueWithString)
{
    // Create and initialise our various arrays and create our solver.

    const auto [solver, states, rates, variables] = createAndInitialiseArraysAndCreateSolver("CVODE");

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
*/

/*---GRY---
TEST(CvodeSolverTest, stepValueWithInvalidNumber)
{
    // Create and initialise our various arrays and create our solver.

    const auto [solver, states, rates, variables] = createAndInitialiseArraysAndCreateSolver("CVODE");

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
*/

TEST(CvodeSolverTest, main)
{
    // Create and initialise our various arrays and create our solver.

    const auto [solver, states, rates, variables] = createAndInitialiseArraysAndCreateSolver("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015422246861901662, 0.59605559394022689, 0.053035160895529773, 0.31777058539774627};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01542143979535311, 0.59605559223195803, 0.053035155728368123, 0.31777058661072155};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015423275579583704, 0.5960556037980016, 0.053035167225244692, 0.31777058077104786};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015420735373646405, 0.59605558367032696, 0.05303515120051977, 0.31777058753168086};
#endif

    computeModel(solver, states, rates, variables, FINAL_STATES);

    // Clean up after ourselves.

    deleteArrays(states, rates, variables);
}
