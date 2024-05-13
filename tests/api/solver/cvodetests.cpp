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

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setMaximumStep(RELATIVE_TOLERANCE);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, maximumNumberOfStepsValueWithInvalidNumber)
{
    static const auto MAXIMUM_NUMBER_OF_STEPS = 0;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum number of steps cannot be equal to 0. It must be greater than 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setMaximumNumberOfSteps(MAXIMUM_NUMBER_OF_STEPS);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    static const auto UPPER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    static const auto UPPER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    static const auto LOWER_HALF_BANDWIDTH = -1;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    static const auto LOWER_HALF_BANDWIDTH = 4;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, relativeToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setRelativeTolerance(RELATIVE_TOLERANCE);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CvodeSolverTest, absoluteToleranceValueWithInvalidNumber)
{
    static const auto RELATIVE_TOLERANCE = -1.234;
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setAbsoluteTolerance(RELATIVE_TOLERANCE);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

namespace {

void cvodeSolve(const libOpenCOR::Doubles &pStateValues, const libOpenCOR::Doubles &pRateValues,
                const libOpenCOR::Doubles &pVariableValues, bool pCompiled)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    OdeModel::run(document, pStateValues, pRateValues, pVariableValues, pCompiled);
}

} // namespace

TEST(CvodeSolverTest, compiledSolve)
{
    static const auto STATE_VALUES = std::vector<double>({-63.885782, 0.135007, 0.984333, 0.740973});
    static const auto RATE_VALUES = std::vector<double>({49.725595, -0.128192, -0.050905, 0.098648});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.981835, -823.516207, 789.778615, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.969859, 0.114989, 0.00287, 0.967346, 12.0, 36.0, 0.541331, 0.056246});

    cvodeSolve(STATE_VALUES, RATE_VALUES, VARIABLE_VALUES, true);
}

TEST(CvodeSolverTest, interpretedSolve)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886003, 0.135008, 0.984333, 0.740972});
    static const auto RATE_VALUES = std::vector<double>({49.725651, -0.128193, -0.050904, 0.098649});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.981901, -823.516588, 789.779007, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.96988, 0.114988, 0.00287, 0.967346, 12.0, 36.0, 0.541333, 0.056246});

    cvodeSolve(STATE_VALUES, RATE_VALUES, VARIABLE_VALUES, false);
}

TEST(CvodeSolverTest, solveWithoutInterpolateSolution)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886395, 0.135008, 0.984334, 0.740972});
    static const auto RATE_VALUES = std::vector<double>({49.725709, -0.128194, -0.050903, 0.098651});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.982019, -823.51695, 789.77946, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.969916, 0.114985, 0.00287, 0.967348, 12.0, 36.0, 0.541337, 0.056246});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setInterpolateSolution(false);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithAdamsMoultonIntegrationMethod)
{
    static const auto STATE_VALUES = std::vector<double>({-63.887314, 0.135011, 0.984335, 0.740970});
    static const auto RATE_VALUES = std::vector<double>({49.726135, -0.128196, -0.0509, 0.098655});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.982294, -823.518644, 789.78127, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.970003, 0.11498, 0.002869, 0.967351, 12.0, 36.0, 0.541346, 0.056245});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIntegrationMethod(libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithFunctionalIterationType)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886535, 0.135009, 0.984334, 0.740971});
    static const auto RATE_VALUES = std::vector<double>({49.725827, -0.128194, -0.050903, 0.098651});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.98206, -823.517642, 789.780112, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.969929, 0.114985, 0.00287, 0.967348, 12.0, 36.0, 0.541338, 0.056246});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIterationType(libOpenCOR::SolverCvode::IterationType::FUNCTIONAL);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithBandedLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({-54.958164, 0.114717, 0.971366, 0.756967});
    static const auto RATE_VALUES = std::vector<double>({47.196374, -0.102082, -0.092975, 0.062978});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -13.303549, -791.433508, 757.546624, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.153477, 0.188825, 0.004484, 0.923848, 12.0, 36.0, 0.454654, 0.062887});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithDiagonalLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886547, 0.135008, 0.984334, 0.740970});
    static const auto RATE_VALUES = std::vector<double>({49.725024, -0.128193, -0.050902, 0.098652});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.982064, -823.513555, 789.774976, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.969931, 0.114984, 0.00287, 0.967348, 12.0, 36.0, 0.541338, 0.056246});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::DIAGONAL);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({9.518468, 0.367366, 0.01612, 0.491145});
    static const auto RATE_VALUES = std::vector<double>({-0.864067, 0.064354, 0.001718, -0.052716});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, 6.03944, -5.198281, 0.022996, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 0.112958, 6.787605, 0.112665, 0.018857, 12.0, 36.0, 0.032306, 0.140794});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({9.518406, 0.36737, 0.016121, 0.491141});
    static const auto RATE_VALUES = std::vector<double>({-0.864079, 0.064354, 0.001718, -0.052715});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, 6.039422, -5.198258, 0.022996, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 0.112959, 6.787581, 0.112665, 0.018857, 12.0, 36.0, 0.032306, 0.140793});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolver)
{
    static const auto STATE_VALUES = std::vector<double>({9.5181, 0.367396, 0.016121, 0.491122});
    static const auto RATE_VALUES = std::vector<double>({-0.864144, 0.064349, 0.001718, -0.052712});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, 6.03933, -5.1981, 0.023001, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 0.112961, 6.787466, 0.112663, 0.018857, 12.0, 36.0, 0.032307, 0.140793});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolverAndNoPreconditioner)
{
    static const auto STATE_VALUES = std::vector<double>({-63.886553, 0.135009, 0.984334, 0.740971});
    static const auto RATE_VALUES = std::vector<double>({49.725971, -0.128194, -0.050901, 0.098651});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.982066, -823.517289, 789.779577, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.969931, 0.114984, 0.00287, 0.967348, 12.0, 36.0, 0.541338, 0.056246});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolverAndNoPreconditioner)
{
    static const auto STATE_VALUES = std::vector<double>({-63.885919, 0.135007, 0.984333, 0.740972});
    static const auto RATE_VALUES = std::vector<double>({49.725375, -0.128192, -0.050904, 0.098649});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.981876, -823.515974, 789.778576, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.969872, 0.114988, 0.00287, 0.967346, 12.0, 36.0, 0.541332, 0.056246});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolverAndNoPreconditioner)
{
    static const auto STATE_VALUES = std::vector<double>({-63.885721, 0.135007, 0.984333, 0.740973});
    static const auto RATE_VALUES = std::vector<double>({49.725598, -0.128192, -0.050904, 0.098648});
    static const auto VARIABLE_VALUES = std::vector<double>({0.0, -15.981816, -823.515538, 789.778026, 1.0, 0.0, -10.613, 0.3, -115.0, 120.0, 3.969853, 0.11499, 0.00287, 0.967345, 12.0, 36.0, 0.54133, 0.056246});

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode/model.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);
    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    OdeModel::run(document, STATE_VALUES, RATE_VALUES, VARIABLE_VALUES);
}
