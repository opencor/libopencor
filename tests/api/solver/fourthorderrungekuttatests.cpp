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

TEST(FourthOrderRungeKuttaSolverTest, basic)
{
    auto solver = libOpenCOR::SolverFourthOrderRungeKutta::create();

    EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solver->id(), "KISAO:0000032");
    EXPECT_EQ(solver->name(), "Fourth-order Runge-Kutta");
}

TEST(FourthOrderRungeKuttaSolverTest, stepValueWithInvalidNumber)
{
    static const auto STEP = 0.0;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The step cannot be equal to 0. It must be greater than 0."},
    };

    auto solver = libOpenCOR::SolverFourthOrderRungeKutta::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setStep(STEP);

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(FourthOrderRungeKuttaSolverTest, solve)
{
    static const auto STEP = 0.0123;
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01538470269835, 0.596055526314677, 0.053034933968590, 0.31777081396617829};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.00000000000001, 0.000000000000001, 0.000000000000001, 0.00000000000000001};

    auto solver = libOpenCOR::SolverFourthOrderRungeKutta::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setStep(STEP);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}
