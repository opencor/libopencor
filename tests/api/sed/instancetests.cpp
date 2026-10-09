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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <span>
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

    // Make sure that the issues are still present after "running" the instance.

    EXPECT_DOUBLE_EQ(instance->run(), 0.0);
    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
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

TEST(InstanceSedTest, cellmlFileWithUnitsPrefixOutOfRange)
{
    // Note: libCellML handles such a units prefix by catching the std::out_of_range exception thrown by std::stoi(), so
    //       this checks that exceptions can be caught everywhere in libOpenCOR, including in our third-party libraries.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is invalid."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: prefix '92233720368547758077876856757465433' of a unit referencing 'second' in units 'my_units' is out of the integer range."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/units_prefix_out_of_range.cellml"))};
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

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
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
    static const auto SIMULATION_PROPERTY {1000000};
    static const auto WAIT_ITERATIONS = 60000;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
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

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);
}

TEST(InstanceSedTest, stopRunWhenNotRunningDoesNotAffectNextRun)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->stopRun();
    instance->run();

    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());

    instance->stopRun();

    EXPECT_TRUE(instance->startRun());
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, stopRunRightAfterStartRun)
{
    static const auto SIMULATION_PROPERTY {1000000};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    instance->stopRun();
    instance->waitForRun();

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, pauseRunRightAfterStartRun)
{
    static const auto SIMULATION_PROPERTY {1000000};
    static const auto PAUSE_SLEEP = 50;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());

    instance->pauseRun();

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_SLEEP));

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::PAUSED);

    instance->stopRun();
    instance->waitForRun();

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, stopRunResultsHaveNans)
{
    static const auto SIMULATION_PROPERTY {1000000};
    static const auto WAIT_ITERATIONS = 60000;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());

    const auto &instanceTask {instance->tasks()[0]};
    const auto &voi {instanceTask->voi()};
    const auto &state0 {instanceTask->state(0)};

    EXPECT_EQ(voi.size(), state0.size());
    EXPECT_EQ(voi.size(), SIMULATION_PROPERTY + 1);

    EXPECT_FALSE(std::isnan(voi[0]));
    EXPECT_FALSE(std::isnan(state0[0]));

    size_t nanIndex {voi.size()};

    for (size_t i {1}; i < voi.size(); ++i) {
        if (std::isnan(state0[i])) {
            nanIndex = i;

            break;
        }
    }

    EXPECT_LT(nanIndex, voi.size());
    EXPECT_TRUE(std::isnan(voi[nanIndex]));
    EXPECT_LT(nanIndex, voi.size() - 1);
}

TEST(InstanceSedTest, stopRunBeforeOutputStartTime)
{
    // Note: our output start time is such that it would take our simulation a very long time to reach it, so we are
    //       guaranteed to stop our run before it gets reached.

    static const auto OUTPUT_START_TIME {1.0e9};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto instance {document->instantiate()};

    // Run our instance so that we have some results.

    EXPECT_GT(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());

    const auto &instanceTask {instance->tasks()[0]};
    const auto voi {instanceTask->voi()};

    EXPECT_FALSE(std::isnan(voi[0]));

    // Start our run and stop it before it reaches the output start time, which means that we have no results to
    // report. Our results should therefore not have been reallocated (since their size has not changed), but they
    // should all be NaN values.

    simulation->setOutputStartTime(OUTPUT_START_TIME);
    simulation->setOutputEndTime(OUTPUT_START_TIME + static_cast<double>(simulation->numberOfSteps()));

    EXPECT_TRUE(instance->startRun());

    instance->stopRun();
    instance->waitForRun();

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_EQ(instance->progress(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
    EXPECT_EQ(instanceTask->voi().data(), voi.data());

    auto allNan = [](std::span<const double> pValues) {
        return !pValues.empty() && std::ranges::all_of(pValues, [](double pValue) {
            return std::isnan(pValue);
        });
    };

    EXPECT_TRUE(allNan(instanceTask->voi()));

    for (size_t i {0}; i < instanceTask->stateCount(); ++i) {
        EXPECT_TRUE(allNan(instanceTask->state(i)));
        EXPECT_TRUE(allNan(instanceTask->rate(i)));
    }

    for (size_t i {0}; i < instanceTask->constantCount(); ++i) {
        EXPECT_TRUE(allNan(instanceTask->constant(i)));
    }

    for (size_t i {0}; i < instanceTask->computedConstantCount(); ++i) {
        EXPECT_TRUE(allNan(instanceTask->computedConstant(i)));
    }

    for (size_t i {0}; i < instanceTask->algebraicVariableCount(); ++i) {
        EXPECT_TRUE(allNan(instanceTask->algebraicVariable(i)));
    }
}

TEST(InstanceSedTest, pauseRunAndResumeRun)
{
    static const auto SIMULATION_PROPERTY {1000000};
    static const auto WAIT_ITERATIONS = 60000;
    static const auto PAUSE_SLEEP = 50;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
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

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_DOUBLE_EQ(instance->progress(), 0.0);
}

TEST(InstanceSedTest, pauseRunThenStopRun)
{
    static const auto SIMULATION_PROPERTY {1000000};
    static const auto WAIT_ITERATIONS = 60000;
    static const auto PAUSE_SLEEP = 50;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, deletePausedInstance)
{
    static const auto SIMULATION_PROPERTY {1000000};
    static const auto WAIT_ITERATIONS = 60000;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

    auto instance {document->instantiate()};
    const auto instanceTask {instance->tasks()[0]};

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (instance->progress() > 0.0) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    instance->pauseRun();

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::PAUSED);

    // Delete our paused instance, something that would hang if our run was not stopped first.

    instance.reset();

    EXPECT_LT(instanceTask->progress(), 1.0);
}

TEST(InstanceSedTest, pauseRunAndResumeRunWithNaturalCompletion)
{
    static const auto MODERATE_STEP_COUNT {50000};
    static const auto WAIT_ITERATIONS = 60000;
    static const auto PAUSE_SLEEP = 50;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, startRunWhileAlreadyRunning)
{
    static const auto SIMULATION_PROPERTY {1000000};
    static const auto WAIT_ITERATIONS = 60000;

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_LT(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, startRunRightAfterStatusIsIdle)
{
    // Note: a run is flagged as not running anymore just before it completes, so make sure that a new run can be
    //       started as soon as our instance is reported as idle.

    static const auto NUMBER_OF_STEPS {10};
    static const auto NUMBER_OF_RUNS {1000};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(NUMBER_OF_STEPS);
    simulation->setOutputEndTime(static_cast<double>(NUMBER_OF_STEPS));

    auto instance {document->instantiate()};

    for (auto i {0}; i < NUMBER_OF_RUNS; ++i) {
        ASSERT_TRUE(instance->startRun());

        while (instance->status() != libOpenCOR::SedInstance::Status::IDLE) {
        }
    }

    EXPECT_GT(instance->waitForRun(), 0.0);
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
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);

    EXPECT_TRUE(instance->startRun());

    for (size_t i {0}; i < WAIT_ITERATIONS; ++i) {
        if (instance->status() == libOpenCOR::SedInstance::Status::IDLE) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, resultsAllocatedWhenStartingRun)
{
    // Note: the results of a task must be (re)allocated before startRun() returns, so that they can be safely retrieved
    //       while the task is being run (e.g., to plot them progressively).

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto instance {document->instantiate()};

    // Run our instance so that our results get allocated.

    EXPECT_GT(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());

    const auto &instanceTask {instance->tasks()[0]};
    const auto numberOfSteps {static_cast<size_t>(simulation->numberOfSteps())};

    EXPECT_EQ(instanceTask->voi().size(), numberOfSteps + 1);

    // Change the size of our results and start running our instance, which means that our results must have been
    // reallocated by the time startRun() returns and that they must remain valid for the whole run.

    simulation->setNumberOfSteps(static_cast<int>(2 * numberOfSteps));

    EXPECT_TRUE(instance->startRun());

    const auto voi {instanceTask->voi()};
    const auto state {instanceTask->state(0)};

    EXPECT_EQ(voi.size(), (2 * numberOfSteps) + 1);
    EXPECT_EQ(state.size(), (2 * numberOfSteps) + 1);

    // Retrieve our results while our instance is running. They should always be the same arrays and they should never
    // contain any NaN values (our results are either not yet computed, i.e. zeros, or computed).

    auto isNan = [](double pValue) {
        return std::isnan(pValue);
    };

    while (instance->status() != libOpenCOR::SedInstance::Status::IDLE) {
        EXPECT_EQ(instanceTask->voi().data(), voi.data());
        EXPECT_EQ(instanceTask->state(0).data(), state.data());
        EXPECT_FALSE(std::ranges::any_of(voi, isNan));
        EXPECT_FALSE(std::ranges::any_of(state, isNan));
    }

    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
    EXPECT_EQ(instanceTask->voi().data(), voi.data());
    EXPECT_EQ(instanceTask->state(0).data(), state.data());
    EXPECT_EQ(voi[voi.size() - 1], simulation->outputEndTime());
    EXPECT_FALSE(std::isnan(state[state.size() - 1]));
}

TEST(InstanceSedTest, simulationSettingsUsedWhenStartingRun)
{
    // Note: the simulation settings used by a run are those in effect when the run is started, even if they get changed
    //       while the run is in progress.

    static const auto FACTOR {2};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto instance {document->instantiate()};
    const auto outputEndTime {simulation->outputEndTime()};
    const auto numberOfSteps {simulation->numberOfSteps()};

    EXPECT_TRUE(instance->startRun());

    simulation->setOutputEndTime(FACTOR * outputEndTime);
    simulation->setNumberOfSteps(FACTOR * numberOfSteps);

    EXPECT_GT(instance->waitForRun(), 0.0);
    EXPECT_FALSE(instance->hasIssues());

    const auto &instanceTask {instance->tasks()[0]};
    const auto voi {instanceTask->voi()};

    EXPECT_EQ(voi.size(), static_cast<size_t>(numberOfSteps) + 1);
    EXPECT_EQ(voi[voi.size() - 1], outputEndTime);

    // Running our instance again should use our new simulation settings.

    EXPECT_GT(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());

    EXPECT_EQ(instanceTask->voi().size(), static_cast<size_t>(FACTOR * numberOfSteps) + 1);
    EXPECT_EQ(instanceTask->voi()[instanceTask->voi().size() - 1], FACTOR * outputEndTime);
}

TEST(InstanceSedTest, runWhileAsynchronousRunInProgress)
{
    // Note: running an instance while it is already being run asynchronously should wait for the asynchronous run to
    //       complete before running the instance again.

    static const auto MODERATE_STEP_COUNT {10000};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(MODERATE_STEP_COUNT);
    simulation->setOutputEndTime(static_cast<double>(MODERATE_STEP_COUNT));

    auto instance {document->instantiate()};

    EXPECT_TRUE(instance->startRun());
    EXPECT_GT(instance->run(), 0.0);
    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
    EXPECT_FALSE(instance->hasIssues());

    const auto &instanceTask {instance->tasks()[0]};
    const auto voi {instanceTask->voi()};

    EXPECT_EQ(voi.size(), MODERATE_STEP_COUNT + 1);
    EXPECT_EQ(voi[voi.size() - 1], static_cast<double>(MODERATE_STEP_COUNT));
    EXPECT_FALSE(std::isnan(instanceTask->state(0)[voi.size() - 1]));
}

TEST(InstanceSedTest, odeModel)
{
    const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | CVODE: at t = 0.00140013827899996, mxstep steps taken before reaching tout."},
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

TEST(InstanceSedTest, odeModelWithNonUniformTimeCourseSimulation)
{
    static const libOpenCOR::ExpectedIssues ONE_STEP_EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which (currently) requires a uniform time course simulation."},
    }};
    static const libOpenCOR::ExpectedIssues STEADY_STATE_EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation2' is to be used with model 'model1' which (currently) requires a uniform time course simulation."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create()};
    auto model {libOpenCOR::SedModel::create(document, file)};
    auto oneStep {libOpenCOR::SedOneStep::create(document)};
    auto steadyState {libOpenCOR::SedSteadyState::create(document)};
    auto task {libOpenCOR::SedTask::create(document, model, oneStep)};

    oneStep->setOdeSolver(libOpenCOR::SolverCvode::create());
    steadyState->setOdeSolver(libOpenCOR::SolverCvode::create());

    document->addModel(model);
    document->addSimulation(oneStep);
    document->addSimulation(steadyState);
    document->addTask(task);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, ONE_STEP_EXPECTED_ISSUES);

    task->setSimulation(steadyState);

    instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, STEADY_STATE_EXPECTED_ISSUES);
}

TEST(InstanceSedTest, odeModelWithInvalidNumberOfSteps)
{
    static const libOpenCOR::ExpectedIssues ZERO_STEPS_EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but 0 is provided."},
    }};
    static const libOpenCOR::ExpectedIssues NEGATIVE_STEPS_EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but -100 is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(0);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, ZERO_STEPS_EXPECTED_ISSUES);

    simulation->setNumberOfSteps(-100); // NOLINT

    instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, NEGATIVE_STEPS_EXPECTED_ISSUES);

    // Running the instance should not do anything, but it should still leave it idle.

    EXPECT_TRUE(instance->startRun());
    EXPECT_EQ(instance->waitForRun(), 0.0);
    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_EQ_ISSUES(instance, NEGATIVE_STEPS_EXPECTED_ISSUES);

    // Make sure that the number of steps is not reported as invalid anymore once it has been fixed.

    simulation->setNumberOfSteps(1000); // NOLINT

    instance = document->instantiate();

    EXPECT_FALSE(instance->hasIssues());
    EXPECT_GT(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());

    // Make sure that an invalid number of steps is reported when running an instance even if it was valid when the
    // instance was created.

    const auto &instanceTask {instance->tasks()[0]};

    simulation->setNumberOfSteps(0);

    EXPECT_EQ(instance->run(), 0.0);
    EXPECT_EQ_ISSUES(instance, ZERO_STEPS_EXPECTED_ISSUES);
    EXPECT_EQ(instanceTask->voi().size(), 1001);

    simulation->setNumberOfSteps(-100); // NOLINT

    EXPECT_TRUE(instance->startRun());
    EXPECT_EQ(instance->waitForRun(), 0.0);
    EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
    EXPECT_EQ_ISSUES(instance, NEGATIVE_STEPS_EXPECTED_ISSUES);
    EXPECT_EQ(instanceTask->voi().size(), 1001);

    // Make sure that the instance can be run again once the number of steps has been fixed.

    simulation->setNumberOfSteps(500); // NOLINT

    EXPECT_GT(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
    EXPECT_EQ(instanceTask->voi().size(), 501);
}

TEST(InstanceSedTest, odeModelWithInvalidTimes)
{
    static const libOpenCOR::ExpectedIssues INITIAL_TIME_EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 5, 0, and 50 are provided."},
    }};
    static const libOpenCOR::ExpectedIssues OUTPUT_END_TIME_EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 0, 5, and 5 are provided."},
    }};
    static const libOpenCOR::ExpectedIssues TIMES_AND_STEPS_EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 0, 5, and 5 are provided."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but 0 is provided."},
    }};

    static const auto FIVE {5.0};
    static const auto FIFTY {50.0};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    // Initial time after the output start time.

    simulation->setInitialTime(FIVE);
    simulation->setOutputStartTime(0.0);
    simulation->setOutputEndTime(FIFTY);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, INITIAL_TIME_EXPECTED_ISSUES);

    // Output start time equal to the output end time.

    simulation->setInitialTime(0.0);
    simulation->setOutputStartTime(FIVE);
    simulation->setOutputEndTime(FIVE);

    instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, OUTPUT_END_TIME_EXPECTED_ISSUES);

    // Both invalid times and an invalid number of steps.

    simulation->setNumberOfSteps(0);

    instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, TIMES_AND_STEPS_EXPECTED_ISSUES);

    // Non-finite times.

    simulation->setOutputEndTime(std::numeric_limits<double>::quiet_NaN());
    simulation->setNumberOfSteps(10); // NOLINT

    instance = document->instantiate();

    EXPECT_EQ(instance->errorCount(), 1);

    simulation->setOutputEndTime(std::numeric_limits<double>::infinity());

    instance = document->instantiate();

    EXPECT_EQ(instance->errorCount(), 1);

    simulation->setInitialTime(-std::numeric_limits<double>::infinity());
    simulation->setOutputStartTime(0.0);
    simulation->setOutputEndTime(FIFTY);

    instance = document->instantiate();

    EXPECT_EQ(instance->errorCount(), 1);

    simulation->setInitialTime(0.0);
    simulation->setOutputStartTime(std::numeric_limits<double>::quiet_NaN());

    instance = document->instantiate();

    EXPECT_EQ(instance->errorCount(), 1);

    // Valid times, but then made invalid after instantiation.

    simulation->setOutputStartTime(0.0);
    simulation->setOutputEndTime(FIFTY);
    simulation->setNumberOfSteps(static_cast<int>(FIFTY));

    instance = document->instantiate();

    EXPECT_FALSE(instance->hasIssues());
    EXPECT_GT(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);

    simulation->setOutputStartTime(FIVE);
    simulation->setOutputEndTime(FIVE);

    EXPECT_EQ(instance->run(), 0.0);
    EXPECT_EQ_ISSUES(instance, OUTPUT_END_TIME_EXPECTED_ISSUES);
    EXPECT_EQ(instance->progress(), 0.0);

    // Valid times again.

    simulation->setOutputStartTime(0.0);
    simulation->setOutputEndTime(FIFTY);

    EXPECT_GT(instance->run(), 0.0);
    EXPECT_FALSE(instance->hasIssues());
    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
}

TEST(InstanceSedTest, odeModelResultsReusedWhenRunAgain)
{
    // Note: our Python bindings return zero-copy NumPy arrays, so make sure that our results are not reallocated when
    //       running an instance again with the same number of steps.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    const auto &instanceTask {instance->tasks()[0]};
    const auto voi {instanceTask->voi()};
    const auto state {instanceTask->state(0)};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
    EXPECT_EQ(instanceTask->voi().data(), voi.data());
    EXPECT_EQ(instanceTask->state(0).data(), state.data());
    EXPECT_EQ(voi[voi.size() - 1], instanceTask->voi()[voi.size() - 1]);
    EXPECT_EQ(state[state.size() - 1], instanceTask->state(0)[state.size() - 1]);
}

TEST(InstanceSedTest, odeModelWithRoundingErrorOnOutputEndTime)
{
    // Note: with an output start time of -1, an output end time of 0, and 49 steps, the time of the last step is computed
    //       as -1 + 49 * (1 / 49), which is not exactly 0 due to rounding errors. This used to result in an extra step
    //       being taken and its results being written past the end of our results arrays.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    static const auto INITIAL_TIME {-1.0};
    static const auto OUTPUT_END_TIME {0.0};
    static const auto NUMBER_OF_STEPS {49};

    simulation->setInitialTime(INITIAL_TIME);
    simulation->setOutputStartTime(INITIAL_TIME);
    simulation->setOutputEndTime(OUTPUT_END_TIME);
    simulation->setNumberOfSteps(NUMBER_OF_STEPS);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);

    const auto &voi {instance->tasks()[0]->voi()};

    EXPECT_EQ(voi.size(), NUMBER_OF_STEPS + 1);
    EXPECT_EQ(voi[0], INITIAL_TIME);
    EXPECT_EQ(voi[voi.size() - 1], OUTPUT_END_TIME);
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

TEST(InstanceSedTest, daeModelWithNonUniformTimeCourseSimulation)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which (currently) requires a uniform time course simulation."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"))};
    auto document {libOpenCOR::SedDocument::create()};
    auto model {libOpenCOR::SedModel::create(document, file)};
    auto oneStep {libOpenCOR::SedOneStep::create(document)};
    auto task {libOpenCOR::SedTask::create(document, model, oneStep)};

    document->addModel(model);
    document->addSimulation(oneStep);
    document->addTask(task);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, daeModelWithFailingOdeSolver)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | CVODE: at t = 1.08537561647883e-09, mxstep steps taken before reaching tout."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    const auto &cvode {std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver())};

    static const auto NOK_MAXIMUM_NUMBER_OF_STEPS {1};

    cvode->setMaximumNumberOfSteps(NOK_MAXIMUM_NUMBER_OF_STEPS);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

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
    EXPECT_DOUBLE_EQ(instance->progress(), 1.0);

    static const auto VOI_SIZE {50001U};
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
    EXPECT_EQ(instance->run(), 0.0);
    EXPECT_TRUE(instance->hasIssues());

    // Our simulation failed before reaching its output start time, so we have no results to report, i.e. our results
    // should all be NaN values.

    const auto &instanceTask {instance->tasks()[0]};
    const auto voi {instanceTask->voi()};
    const auto state {instanceTask->state(0)};

    EXPECT_FALSE(voi.empty());
    EXPECT_TRUE(std::ranges::all_of(voi, [](double pValue) {
        return std::isnan(pValue);
    }));
    EXPECT_TRUE(std::ranges::all_of(state, [](double pValue) {
        return std::isnan(pValue);
    }));
}

TEST(InstanceSedTest, changesToVariablesUsedToInitialiseOtherVariables)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/variables_initialised_using_variables.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};
    const auto &model {document->model(0)};
    const auto &instanceTask {instance->tasks()[0]};

    auto initialValue = [&instanceTask](const std::string &pName) -> double {
        for (size_t i {0}; i < instanceTask->stateCount(); ++i) {
            if (instanceTask->stateName(i) == pName) {
                return instanceTask->state(i)[0];
            }
        }

        for (size_t i {0}; i < instanceTask->constantCount(); ++i) {
            if (instanceTask->constantName(i) == pName) {
                return instanceTask->constant(i)[0];
            }
        }

        for (size_t i {0}; i < instanceTask->computedConstantCount(); ++i) {
            if (instanceTask->computedConstantName(i) == pName) {
                return instanceTask->computedConstant(i)[0];
            }
        }

        return std::numeric_limits<double>::quiet_NaN();
    };

    auto checkInitialValues = [&](const std::vector<std::pair<std::string, double>> &pExpectedInitialValues) {
        instance->run();

        EXPECT_FALSE(instance->hasIssues());

        for (const auto &[name, expectedInitialValue] : pExpectedInitialValues) {
            EXPECT_DOUBLE_EQ(initialValue(name), expectedInitialValue) << name;
        }
    };

    // No changes.

    checkInitialValues({{"main/x", 3.0}, {"main/y", 3.0}, {"main/k2", 3.0}, {"main/k3", 3.0}, {"main/cc", 6.0}, {"main/z", 6.0}, {"main/w", 6.0}, {"main/v", 3.0}, {"main/u", 0.005}, {"main/q", 0.005}, {"main/r", 2.0}}); // NOLINT

    // Change a constant that is used (directly or indirectly) to initialise some variables.

    model->addChange(libOpenCOR::SedChangeAttribute::create("main", "k", "5.0"));

    checkInitialValues({{"main/x", 5.0}, {"main/y", 5.0}, {"main/k2", 5.0}, {"main/k3", 5.0}, {"main/cc", 10.0}, {"main/z", 10.0}, {"main/w", 10.0}, {"main/v", 5.0}, {"main/u", 0.005}, {"main/q", 0.005}, {"main/r", 2.0}}); // NOLINT

    // Change a state that is used (directly or indirectly) to initialise some variables.

    model->addChange(libOpenCOR::SedChangeAttribute::create("main", "x", "7.0"));

    checkInitialValues({{"main/x", 7.0}, {"main/y", 7.0}, {"main/k2", 5.0}, {"main/k3", 7.0}, {"main/cc", 14.0}, {"main/z", 14.0}, {"main/w", 14.0}, {"main/v", 5.0}, {"main/u", 0.005}, {"main/q", 0.005}, {"main/r", 2.0}}); // NOLINT

    // Change a state that is initialised using a computed constant.

    model->addChange(libOpenCOR::SedChangeAttribute::create("main", "z", "1.0"));

    checkInitialValues({{"main/x", 7.0}, {"main/y", 7.0}, {"main/k2", 5.0}, {"main/k3", 7.0}, {"main/cc", 14.0}, {"main/z", 1.0}, {"main/w", 1.0}, {"main/v", 5.0}, {"main/u", 0.005}, {"main/q", 0.005}, {"main/r", 2.0}}); // NOLINT

    // Change a constant that is initialised using a state.

    model->addChange(libOpenCOR::SedChangeAttribute::create("main", "k3", "2.0"));

    checkInitialValues({{"main/x", 7.0}, {"main/y", 7.0}, {"main/k2", 5.0}, {"main/k3", 2.0}, {"main/cc", 4.0}, {"main/z", 1.0}, {"main/w", 1.0}, {"main/v", 5.0}, {"main/u", 0.005}, {"main/q", 0.005}, {"main/r", 2.0}}); // NOLINT

    // Change some constants that are used to initialise some variables in another component and with different units.

    model->addChange(libOpenCOR::SedChangeAttribute::create("initialisation", "u_init", "2.0"));
    model->addChange(libOpenCOR::SedChangeAttribute::create("initialisation", "p", "4.0"));

    checkInitialValues({{"main/x", 7.0}, {"main/y", 7.0}, {"main/k2", 5.0}, {"main/k3", 2.0}, {"main/cc", 4.0}, {"main/z", 1.0}, {"main/w", 1.0}, {"main/v", 5.0}, {"main/u", 0.002}, {"main/q", 0.004}, {"main/r", 2.0}}); // NOLINT

    // Change a constant that is used to initialise a variable in the same component but with different units.

    model->addChange(libOpenCOR::SedChangeAttribute::create("main", "s", "4.0"));

    checkInitialValues({{"main/x", 7.0}, {"main/y", 7.0}, {"main/k2", 5.0}, {"main/k3", 2.0}, {"main/cc", 4.0}, {"main/z", 1.0}, {"main/w", 1.0}, {"main/v", 5.0}, {"main/u", 0.002}, {"main/q", 0.004}, {"main/r", 4.0}}); // NOLINT

    // Remove all our changes.

    model->removeAllChanges();

    checkInitialValues({{"main/x", 3.0}, {"main/y", 3.0}, {"main/k2", 3.0}, {"main/k3", 3.0}, {"main/cc", 6.0}, {"main/z", 6.0}, {"main/w", 6.0}, {"main/v", 3.0}, {"main/u", 0.005}, {"main/q", 0.005}, {"main/r", 2.0}}); // NOLINT
}
