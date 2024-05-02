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

TEST(FourthOrderRungeKuttaSolverTest, stepValueWithInvalidNumber)
{
    static const auto STEP = 0.0;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The step cannot be equal to 0. It must be greater than 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = libOpenCOR::SolverFourthOrderRungeKutta::create();

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

namespace {

void fourthOrderRungeKuttaSolve(bool pCompiled)
{
    static const auto STEP = 0.0123;

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(sed->simulations()[0]);
    auto solver = libOpenCOR::SolverFourthOrderRungeKutta::create();

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    auto instance = sed->createInstance(pCompiled);

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

} // namespace

TEST(FourthOrderRungeKuttaSolverTest, compiledSolve)
{
    fourthOrderRungeKuttaSolve(true);
}

TEST(FourthOrderRungeKuttaSolverTest, interpretedSolve)
{
    fourthOrderRungeKuttaSolve(false);
}
