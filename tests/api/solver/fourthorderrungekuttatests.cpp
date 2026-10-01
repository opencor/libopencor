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

TEST(FourthOrderRungeKuttaSolverTest, stepValueWithInvalidNumber)
{
    static const auto STEP {0.0};
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task instance | Fourth-order Runge-Kutta: the step cannot be equal to 0. It must be greater than 0."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto solver {libOpenCOR::SolverFourthOrderRungeKutta::create()};

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(FourthOrderRungeKuttaSolverTest, solve)
{
    static const auto STEP {0.0123};
    static const auto STATE_VALUES {std::vector<double>({-63.821233, 0.134844, 0.984267, 0.741105})};
    static const auto STATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto RATE_VALUES {std::vector<double>({49.702732, -0.127922, -0.051225, 0.098266})};
    static const auto RATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto CONSTANT_VALUES {std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0})};
    static const auto CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0})};
    static const auto COMPUTED_CONSTANT_VALUES {std::vector<double>({-10.613, -115.0, 12.0})};
    static const auto COMPUTED_CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0})};
    static const auto ALGEBRAIC_VALUES {std::vector<double>({0.0, -15.96247, -823.402257, 789.661995, 3.963806, 0.115402, 0.002879, 0.967141, 0.540698, 0.056292})};
    static const auto ALGEBRAIC_ABS_TOLS {std::vector<double>({0.0, 0.00001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001})};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto solver {libOpenCOR::SolverFourthOrderRungeKutta::create()};

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS);
}

TEST(FourthOrderRungeKuttaSolverTest, solveWithSeveralStepsPerOutputPoint)
{
    // Note: the output interval is 0.001, hence our step means that we have several steps per output point.

    static const auto STEP {0.00023};
    static const auto STATE_VALUES {std::vector<double>({-63.877006, 0.134985, 0.984324, 0.740991})};
    static const auto STATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto RATE_VALUES {std::vector<double>({49.717088, -0.12809, -0.051025, 0.098505})};
    static const auto RATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto CONSTANT_VALUES {std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0})};
    static const auto CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0})};
    static const auto COMPUTED_CONSTANT_VALUES {std::vector<double>({-10.613, -115.0, 12.0})};
    static const auto COMPUTED_CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0})};
    static const auto ALGEBRAIC_VALUES {std::vector<double>({0.0, -15.979202, -823.500731, 789.762845, 3.969036, 0.115045, 0.002871, 0.967318, 0.541245, 0.056253})};
    static const auto ALGEBRAIC_ABS_TOLS {std::vector<double>({0.0, 0.00001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001})};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto solver {libOpenCOR::SolverFourthOrderRungeKutta::create()};

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS);
}
