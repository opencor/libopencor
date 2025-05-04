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

TEST(CvodeSolverTest, maximumStepValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum step cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setMaximumStep(RELATIVE_TOLERANCE);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, maximumNumberOfStepsValueWithInvalidNumber)
{
    static const auto MAXIMUM_NUMBER_OF_STEPS = 0;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum number of steps cannot be equal to 0. It must be greater than 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setMaximumNumberOfSteps(MAXIMUM_NUMBER_OF_STEPS);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    static const auto UPPER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    static const auto UPPER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    static const auto LOWER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    static const auto LOWER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, relativeToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setRelativeTolerance(RELATIVE_TOLERANCE);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, absoluteToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setAbsoluteTolerance(RELATIVE_TOLERANCE);

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

namespace {

void cvodeSolve(const libOpenCOR::Doubles &pStateValues, const libOpenCOR::Doubles &pStateAbsTols,
                const libOpenCOR::Doubles &pRateValues, const libOpenCOR::Doubles &pRateAbsTols,
                const libOpenCOR::Doubles &pConstantValues, const libOpenCOR::Doubles &pConstantAbsTols,
                const libOpenCOR::Doubles &pComputedConstantValues, const libOpenCOR::Doubles &pComputedConstantAbsTols,
                const libOpenCOR::Doubles &pAlgebraicValues, const libOpenCOR::Doubles &pAlgebraicAbsTols,
                bool pCompiled)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    OdeModel::run(document,
                  pStateValues, pStateAbsTols,
                  pRateValues, pRateAbsTols,
                  pConstantValues, pConstantAbsTols,
                  pComputedConstantValues, pComputedConstantAbsTols,
                  pAlgebraicValues, pAlgebraicAbsTols,
                  pCompiled);
}

} // namespace

TEST(CvodeSolverTest, compiledSolve)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886, 0.135007, 0.984333, 0.740973});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.000001, 0.000001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({49.726, -0.128192, -0.05091, 0.098649});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.000001, 0.00001, 0.000001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.9819, -823.517, 789.779, 3.9699, 0.11499, 0.00287, 0.96735, 0.54133, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.0001, 0.001, 0.001, 0.0001, 0.00001, 0.00001, 0.00001, 0.00001, 0.000001});

    cvodeSolve(STATE_VALUES, STATE_ABS_TOLS,
               RATE_VALUES, RATE_ABS_TOLS,
               CONSTANT_VALUES, CONSTANT_ABS_TOLS,
               COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
               ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
               true);
}

TEST(CvodeSolverTest, interpretedSolve)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886, 0.135008, 0.984333, 0.740972});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.000001, 0.000001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({49.725, -0.128193, -0.05090, 0.09865});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.000001, 0.00001, 0.00001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.9819, -823.517, 789.779, 3.9699, 0.11499, 0.00287, 0.967347, 0.54133, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.0001, 0.001, 0.001, 0.0001, 0.00001, 0.00001, 0.000001, 0.00001, 0.000001});

    cvodeSolve(STATE_VALUES, STATE_ABS_TOLS,
               RATE_VALUES, RATE_ABS_TOLS,
               CONSTANT_VALUES, CONSTANT_ABS_TOLS,
               COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
               ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
               false);
}

TEST(CvodeSolverTest, solveWithoutInterpolateSolution)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886395, 0.135008, 0.984334, 0.740972});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({49.725709, -0.128194, -0.050903, 0.098651});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.982019, -823.51695, 789.77946, 3.969916, 0.114985, 0.00287, 0.967348, 0.541337, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.000001, 0.00001, 0.00001, 0.000001, 0.000001, 0.00001, 0.000001, 0.000001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setInterpolateSolution(false);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithAdamsMoultonIntegrationMethod)
{
    static const auto STATE_VALUES = std::vector<double>({-63.89, 0.13501, 0.98434, 0.74097});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.01, 0.00001, 0.00001, 0.00001});
    static const auto RATE_VALUES = std::vector<double>({49.726, -0.12820, -0.0509, 0.0987});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.0001, 0.0001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.98, -823.52, 789.78, 3.97, 0.1149, 0.002869, 0.96735, 0.5413, 0.05625});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.01, 0.01, 0.01, 0.01, 0.0001, 0.000001, 0.00001, 0.0001, 0.00001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIntegrationMethod(libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithFunctionalIterationType)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886, 0.13501, 0.984334, 0.74097});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.000001, 0.00001});
    static const auto RATE_VALUES = std::vector<double>({49.726, -0.12819, -0.05090, 0.09865});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.00001, 0.00001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.982, -823.52, 789.78, 3.9699, 0.11499, 0.00287, 0.96735, 0.54134, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.001, 0.01, 0.01, 0.0001, 0.00001, 0.00001, 0.00001, 0.00001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIterationType(libOpenCOR::SolverCvode::IterationType::FUNCTIONAL);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithBandedLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({-54.958, 0.11472, 0.97137, 0.756967});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.00001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({47.20, -0.10208, -0.09298, 0.06298});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.01, 0.00001, 0.00001, 0.00001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -13.304, -791.43, 757.5, 3.1534, 0.18883, 0.004484, 0.92385, 0.4547, 0.062887});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.001, 0.01, 0.1, 0.0001, 0.00001, 0.000001, 0.00001, 0.0001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithDiagonalLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({-63.887, 0.13501, 0.984334, 0.74097});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.000001, 0.00001});
    static const auto RATE_VALUES = std::vector<double>({49.725, -0.1282, -0.05090, 0.09865});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.0001, 0.00001, 0.00001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.98, -823.52, 789.78, 3.9699, 0.11498, 0.00287, 0.96735, 0.54134, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.01, 0.01, 0.01, 0.0001, 0.00001, 0.00001, 0.00001, 0.00001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::DIAGONAL);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({9.518468, 0.367366, 0.01612, 0.491145});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({-0.864067, 0.064354, 0.001718, -0.052716});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, 6.03944, -5.198281, 0.022996, 0.112958, 6.787605, 0.112665, 0.018857, 0.032306, 0.140794});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.00001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({9.518406, 0.36737, 0.016121, 0.491141});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({-0.864079, 0.064354, 0.001718, -0.052715});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.000001, 0.000001, 0.000001, 0.000001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, 6.039422, -5.198258, 0.022996, 0.112959, 6.787581, 0.112665, 0.018857, 0.032306, 0.140793});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.00001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({9.5181, 0.367396, 0.016121, 0.491122});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.0001, 0.000001, 0.000001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({-0.86414, 0.064349, 0.001718, -0.052712});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.00001, 0.000001, 0.000001, 0.000001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, 6.03933, -5.1981, 0.023001, 0.112961, 6.78747, 0.112663, 0.018857, 0.032307, 0.140793});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.00001, 0.0001, 0.000001, 0.000001, 0.00001, 0.000001, 0.000001, 0.000001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolverAndNoPreconditioner)
{
    static const auto STATE_VALUES = std::vector<double>({-63.887, 0.13501, 0.984334, 0.74097});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.000001, 0.00001});
    static const auto RATE_VALUES = std::vector<double>({49.726, -0.12819, -0.05090, 0.09865});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.00001, 0.00001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.982, -823.517, 789.779, 3.9699, 0.11498, 0.00287, 0.96734, 0.54134, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.001, 0.001, 0.001, 0.0001, 0.00001, 0.00001, 0.00001, 0.00001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolverAndNoPreconditioner)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886, 0.13501, 0.984333, 0.740972});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.000001, 0.000001});
    static const auto RATE_VALUES = std::vector<double>({49.725, -0.12819, -0.0509, 0.098649});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.0001, 0.000001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.9819, -823.52, 789.779, 3.9699, 0.11499, 0.00287, 0.96735, 0.54133, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.0001, 0.01, 0.001, 0.0001, 0.00001, 0.00001, 0.00001, 0.00001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolverAndNoPreconditioner)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886, 0.13501, 0.98433, 0.74097});
    static const auto STATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.00001, 0.00001});
    static const auto RATE_VALUES = std::vector<double>({49.726, -0.12819, -0.05090, 0.09865});
    static const auto RATE_ABS_TOLS = std::vector<double>({0.001, 0.00001, 0.00001, 0.00001});
    static const auto CONSTANT_VALUES = std::vector<double>({1.0, 0.0, 0.3, 120.0, 36.0});
    static const auto CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0, 0.0, 0.0});
    static const auto COMPUTED_CONSTANT_VALUES = std::vector<double>({-10.613, -115.0, 12.0});
    static const auto COMPUTED_CONSTANT_ABS_TOLS = std::vector<double>({0.0, 0.0, 0.0});
    static const auto ALGEBRAIC_VALUES = std::vector<double>({0.0, -15.982, -823.52, 789.78, 3.9699, 0.11499, 0.00287, 0.96735, 0.54133, 0.056246});
    static const auto ALGEBRAIC_ABS_TOLS = std::vector<double>({0.0, 0.001, 0.01, 0.01, 0.0001, 0.00001, 0.00001, 0.00001, 0.00001, 0.000001});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::run(document,
                  STATE_VALUES, STATE_ABS_TOLS,
                  RATE_VALUES, RATE_ABS_TOLS,
                  CONSTANT_VALUES, CONSTANT_ABS_TOLS,
                  COMPUTED_CONSTANT_VALUES, COMPUTED_CONSTANT_ABS_TOLS,
                  ALGEBRAIC_VALUES, ALGEBRAIC_ABS_TOLS,
                  true);
}
