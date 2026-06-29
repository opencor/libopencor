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

#include "tests/utils.h"

#include <libopencor>

#include <chrono>
#include <thread>

TEST(InstanceSedTest, noFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "The simulation experiment description does not contain any tasks to run."},
    }};

    auto document {libOpenCOR::SedDocument::create()};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);
}

TEST(InstanceSedTest, invalidCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is invalid."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS)."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("error.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, overconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is overconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: variable 'x' in component 'my_component' is overconstrained."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/overconstrained.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, underconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is underconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/underconstrained.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, unsuitablyConstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is unsuitably constrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: variable 'y' in component 'my_component' is overconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unsuitably_constrained.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, algebraicModel)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, asynchronousRunWithoutActiveRun)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->isRunning());
    EXPECT_EQ(instance->waitForRun(), 0.0);
}

TEST(InstanceSedTest, asynchronousRunLifecycle)
{
    static const auto WAIT_ITERATIONS = 200;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->isRunning());
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, asynchronousRunCanBeRestarted)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());

    EXPECT_TRUE(instance->startRun());
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, progressBeforeAnyRun)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);
    EXPECT_DOUBLE_EQ(instance->tasks()[0]->progress(), 0.0);
}

TEST(InstanceSedTest, progressOfAlgebraicModel)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);

    instance->run();

    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
    EXPECT_DOUBLE_EQ(instance->tasks()[0]->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, progressOfOdeModel)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);

    instance->run();

    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
    EXPECT_DOUBLE_EQ(instance->tasks()[0]->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, stopRun)
{
    static const auto LARGE_STEP_COUNT {1000000};
    static const auto WAIT_ITERATIONS = 60000;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(LARGE_STEP_COUNT);
    simulation->setOutputEndTime(static_cast<double>(LARGE_STEP_COUNT));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (instance->progress() > 0.0) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    instance->stopRun();

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, stopRunWhenNotRunning)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->stopRun();

    EXPECT_FALSE(instance->isRunning());
    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);
}

TEST(InstanceSedTest, pauseRunAndResumeRun)
{
    static const auto LARGE_STEP_COUNT {1000000};
    static const auto WAIT_ITERATIONS = 60000;
    static const auto PAUSE_SLEEP = 50;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(LARGE_STEP_COUNT);
    simulation->setOutputEndTime(static_cast<double>(LARGE_STEP_COUNT));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (instance->progress() > 0.0) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    instance->pauseRun();

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_SLEEP));

    instance->resumeRun();
    instance->stopRun();

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->isRunning());
    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, pauseRunAndResumeRunWhenNotRunning)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->pauseRun();
    instance->resumeRun();

    EXPECT_FALSE(instance->isRunning());
    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);
}

TEST(InstanceSedTest, pauseRunThenStopRun)
{
    static const auto LARGE_STEP_COUNT {1000000};
    static const auto WAIT_ITERATIONS = 60000;
    static const auto PAUSE_SLEEP = 50;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(LARGE_STEP_COUNT);
    simulation->setOutputEndTime(static_cast<double>(LARGE_STEP_COUNT));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (instance->progress() > 0.0) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    instance->pauseRun();

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_SLEEP));

    instance->stopRun();

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->isRunning());
    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, pauseRunAndResumeRunWithNaturalCompletion)
{
    static const auto MODERATE_STEP_COUNT {50000};
    static const auto WAIT_ITERATIONS = 60000;
    static const auto PAUSE_SLEEP = 50;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(MODERATE_STEP_COUNT);
    simulation->setOutputEndTime(static_cast<double>(MODERATE_STEP_COUNT));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (instance->progress() > 0.0) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    instance->pauseRun();

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_SLEEP));

    instance->resumeRun();

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->isRunning());
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, startRunWhileAlreadyRunning)
{
    static const auto LARGE_STEP_COUNT {1000000};
    static const auto WAIT_ITERATIONS = 60000;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(LARGE_STEP_COUNT);
    simulation->setOutputEndTime(static_cast<double>(LARGE_STEP_COUNT));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (instance->progress() > 0.0) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->startRun());

    instance->stopRun();

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->isRunning());
    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, startRunAfterPreviousRunCompleted)
{
    static const auto WAIT_ITERATIONS = 60000;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->isRunning());

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (!instance->isRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_FALSE(instance->isRunning());
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, odeModel)
{
    const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
#ifdef BUILDING_ON_INTEL
        {libOpenCOR::Issue::Type::ERROR, "Task | CVODE: at t = 0.00140013827899996, mxstep steps taken before reaching tout."},
#else
        {libOpenCOR::Issue::Type::ERROR,
#    ifdef BUILDING_ON_WINDOWS
         "Task | CVODE: at t = 0.00140013827899821, mxstep steps taken before reaching tout."
#    else
         "Task | CVODE: at t = 0.00140013827899707, mxstep steps taken before reaching tout."
#    endif
        },
#endif
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    static const auto NOK_MAXIMUM_NUMBER_OF_STEPS {10};

    const auto &cvode {std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver())};

    cvode->setMaximumNumberOfSteps(NOK_MAXIMUM_NUMBER_OF_STEPS);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    static const auto OK_MAXIMUM_NUMBER_OF_STEPS {500};

    cvode->setMaximumNumberOfSteps(OK_MAXIMUM_NUMBER_OF_STEPS);

    instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, odeModelWithNoOdeSolver)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    document->simulations()[0]->setOdeSolver(nullptr);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, nlaModel)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0])};
    const auto &kinsol {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver())};

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = document->instantiate();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, nlaModelWithNoNlaSolver)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    document->simulations()[0]->setNlaSolver(nullptr);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, daeModel)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    const auto &kinsol {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver())};

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelWithNoOdeOrNlaSolver)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {document->simulations()[0]};

    simulation->setOdeSolver(nullptr);
    simulation->setNlaSolver(nullptr);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, combineArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, combineArchiveWithCellmlFileAsMasterFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/cellml_file_as_master_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelFromCellmlFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae/model.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelFromSedmlFile)
{
    auto cellmlFile {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae/model.cellml"))};
    auto sedmlFile {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae/model.sedml"))};
    auto document {libOpenCOR::SedDocument::create(sedmlFile)};

    EXPECT_FALSE(document->hasIssues());

    auto nlaSolver {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(document->simulations()[0]->nlaSolver())};

    EXPECT_EQ(nlaSolver->linearSolver(), libOpenCOR::SolverKinsol::LinearSolver::GMRES);
    EXPECT_EQ(nlaSolver->maximumNumberOfIterations(), 123);
    EXPECT_EQ(nlaSolver->upperHalfBandwidth(), 1);
    EXPECT_EQ(nlaSolver->lowerHalfBandwidth(), 1);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelFromCombineArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae/model.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());

    auto nlaSolver {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(document->simulations()[0]->nlaSolver())};

    EXPECT_EQ(nlaSolver->linearSolver(), libOpenCOR::SolverKinsol::LinearSolver::GMRES);
    EXPECT_EQ(nlaSolver->maximumNumberOfIterations(), 123);
    EXPECT_EQ(nlaSolver->upperHalfBandwidth(), 1);
    EXPECT_EQ(nlaSolver->lowerHalfBandwidth(), 1);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelFromLegacySedmlFile)
{
    auto cellmlFile {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae/model.cellml"))};
    auto sedmlFile {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae/model_legacy.sedml"))};
    auto document {libOpenCOR::SedDocument::create(sedmlFile)};

    EXPECT_FALSE(document->hasIssues());

    auto nlaSolver {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(document->simulations()[0]->nlaSolver())};

    EXPECT_EQ(nlaSolver->linearSolver(), libOpenCOR::SolverKinsol::LinearSolver::GMRES);
    EXPECT_EQ(nlaSolver->maximumNumberOfIterations(), 123);
    EXPECT_EQ(nlaSolver->upperHalfBandwidth(), 1);
    EXPECT_EQ(nlaSolver->lowerHalfBandwidth(), 1);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelFromLegacyCombineArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae/model_legacy.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    EXPECT_FALSE(document->hasIssues());

    auto nlaSolver {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(document->simulations()[0]->nlaSolver())};

    EXPECT_EQ(nlaSolver->linearSolver(), libOpenCOR::SolverKinsol::LinearSolver::GMRES);
    EXPECT_EQ(nlaSolver->maximumNumberOfIterations(), 123);
    EXPECT_EQ(nlaSolver->upperHalfBandwidth(), 1);
    EXPECT_EQ(nlaSolver->lowerHalfBandwidth(), 1);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, simulationWithInitialTime)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/simulation_with_initial_time.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    static const auto VOI_SIZE {50001};
    static const auto VOI_START {0.0};
    static const auto VOI_END {50.0};

    const auto &instanceTask {instance->tasks()[0]};
    const auto &voi {instanceTask->voi()};

    EXPECT_EQ(voi.size(), VOI_SIZE);
    EXPECT_EQ(voi[0], VOI_START);
    EXPECT_EQ(voi[voi.size() - 1], VOI_END);

    static const auto INITIAL_VALUE {1.0};

    const auto &x {instanceTask->state(0)};
    const auto &y {instanceTask->state(1)};
    const auto &z {instanceTask->state(2)};

    EXPECT_EQ(x.size(), VOI_SIZE);
    EXPECT_EQ(y.size(), VOI_SIZE);
    EXPECT_EQ(z.size(), VOI_SIZE);

    EXPECT_NE(x[0], INITIAL_VALUE);
    EXPECT_NE(y[0], INITIAL_VALUE);
    EXPECT_NE(z[0], INITIAL_VALUE);
}

TEST(InstanceSedTest, simulationWithInitialTimeFailing)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/simulation_with_initial_time_failing.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_TRUE(instance->hasIssues());
}
