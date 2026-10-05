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

TEST(ForwardEulerSolverTest, stepValueWithInvalidNumber)
{
    static const auto STEP {0.0};
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task instance | Forward Euler: the step cannot be equal to 0. It must be greater than 0."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto solver {libOpenCOR::SolverForwardEuler::create()};

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(ForwardEulerSolverTest, solve)
{
    static const auto STEP {0.0123};
    static const auto STATE_VALUES {std::vector<double>({-63.787727, 0.134748, 0.984255, 0.741178})};
    static const auto STATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto RATE_VALUES {std::vector<double>({49.723292, -0.127811, -0.051436, 0.09812})};
    static const auto RATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto CONSTANT_VALUES {std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0})};
    static const auto CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0})};
    static const auto COMPUTED_CONSTANT_VALUES {std::vector<double>({-10.613, -115.0, 12.0})};
    static const auto COMPUTED_CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0})};
    static const auto ALGEBRAIC_VALUES {std::vector<double>({0.0, -15.952418, -823.361177, 789.590304, 3.960664, 0.115617, 0.002884, 0.967035, 0.54037, 0.056315})};
    static const auto ALGEBRAIC_ABS_TOLS {std::vector<double>({0.0, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.00001, 0.000001})};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto solver {libOpenCOR::SolverForwardEuler::create()};

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS);
}

TEST(ForwardEulerSolverTest, solveWithSeveralStepsPerOutputPoint)
{
    // Note: the output interval is 0.001, hence our step means that we have several steps per output point.

    static const auto STEP {0.00023};
    static const auto STATE_VALUES {std::vector<double>({-63.86285, 0.134946, 0.984315, 0.741021})};
    static const auto STATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto RATE_VALUES {std::vector<double>({49.719825, -0.128045, -0.051096, 0.098444})};
    static const auto RATE_ABS_TOLS {std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001})};
    static const auto CONSTANT_VALUES {std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0})};
    static const auto CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0})};
    static const auto COMPUTED_CONSTANT_VALUES {std::vector<double>({-10.613, -115.0, 12.0})};
    static const auto COMPUTED_CONSTANT_ABS_TOLS {std::vector<double>({0.0, 0.0, 0.0})};
    static const auto ALGEBRAIC_VALUES {std::vector<double>({0.0, -15.974955, -823.479865, 789.734995, 3.967708, 0.115136, 0.002873, 0.967273, 0.541106, 0.056262})};
    static const auto ALGEBRAIC_ABS_TOLS {std::vector<double>({0.0, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.00001, 0.000001})};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto solver {libOpenCOR::SolverForwardEuler::create()};

    solver->setStep(STEP);

    simulation->setOdeSolver(solver);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS);
}
