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

TEST(SecondOrderRungeKuttaSolverTest, stepValueWithInvalidNumber)
{
    static const auto STEP = 0.0;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The step cannot be equal to 0. It must be greater than 0.)"},
    };

    auto solver = libOpenCOR::SolverSecondOrderRungeKutta::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setStep(STEP);

    EXPECT_FALSE(solver->pimpl()->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(SecondOrderRungeKuttaSolverTest, solve)
{
    static const auto STEP = 0.0123;
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01541887962884, 0.59605549615736997, 0.053035139074127761, 0.31777058428249821}; // NOLINT
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.00000000000001, 0.00000000000000001, 0.000000000000000001, 0.00000000000000001};

    auto solver = libOpenCOR::SolverSecondOrderRungeKutta::create();
    auto [states, rates, variables] = OdeModel::initialise();

    solver->setStep(STEP);

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}
