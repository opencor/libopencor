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

#include "gtest/gtest.h"

#include <libopencor>

TEST(CoverageSedTest, initialise)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\"/>\n";

    auto document = libOpenCOR::SedDocument::create();

    EXPECT_EQ(document->serialise(), expectedSerialisation);
}

TEST(CoverageSedTest, models)
{
    auto document = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(document->hasModels());
    EXPECT_FALSE(document->addModel(nullptr));

    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto model = libOpenCOR::SedModel::create(document, file);

    EXPECT_TRUE(document->addModel(model));

    EXPECT_EQ(document->models().size(), 1);
    EXPECT_EQ(document->models()[0], model);

    EXPECT_FALSE(document->addModel(model));
    EXPECT_TRUE(document->removeModel(model));

    EXPECT_FALSE(document->hasModels());

    EXPECT_FALSE(document->removeModel(nullptr));
}

TEST(CoverageSedTest, simulations)
{
    auto document = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(document->hasSimulations());
    EXPECT_FALSE(document->addSimulation(nullptr));

    auto uniformTimeCourse = libOpenCOR::SedUniformTimeCourse::create(document);
    auto oneStep = libOpenCOR::SedOneStep::create(document);
    auto steadyState = libOpenCOR::SedSteadyState::create(document);
    auto analysis = libOpenCOR::SedAnalysis::create(document);

    EXPECT_TRUE(document->addSimulation(uniformTimeCourse));
    EXPECT_TRUE(document->addSimulation(oneStep));
    EXPECT_TRUE(document->addSimulation(steadyState));
    EXPECT_TRUE(document->addSimulation(analysis));

    EXPECT_EQ(document->simulations().size(), 4);
    EXPECT_EQ(document->simulations()[0], uniformTimeCourse);
    EXPECT_EQ(document->simulations()[1], oneStep);
    EXPECT_EQ(document->simulations()[2], steadyState);
    EXPECT_EQ(document->simulations()[3], analysis);

    EXPECT_FALSE(document->addSimulation(uniformTimeCourse));
    EXPECT_TRUE(document->removeSimulation(uniformTimeCourse));

    EXPECT_FALSE(document->addSimulation(oneStep));
    EXPECT_TRUE(document->removeSimulation(oneStep));

    EXPECT_FALSE(document->addSimulation(steadyState));
    EXPECT_TRUE(document->removeSimulation(steadyState));

    EXPECT_FALSE(document->addSimulation(analysis));
    EXPECT_TRUE(document->removeSimulation(analysis));

    EXPECT_FALSE(document->hasSimulations());

    EXPECT_FALSE(document->removeSimulation(nullptr));
}

namespace {

std::string sedTaskExpectedSerialisation(bool pWithProperties)
{
    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
           "  <listOfTasks>\n"
           "    <task id=\"task1\""
           + std::string(pWithProperties ? R"( modelReference="model1" simulationReference="simulation1")" : "")
           + "/>\n"
             "  </listOfTasks>\n"
             "</sedML>\n";
}

} // namespace

TEST(CoverageSedTest, tasks)
{
    auto document = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(document->hasTasks());
    EXPECT_FALSE(document->addTask(nullptr));

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto model = libOpenCOR::SedModel::create(document, file);
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(document);
    auto task = libOpenCOR::SedTask::create(document, model, simulation);

    EXPECT_NE(task->model(), nullptr);
    EXPECT_NE(task->simulation(), nullptr);

    EXPECT_TRUE(document->addTask(task));

    EXPECT_EQ(document->tasks().size(), 1);
    EXPECT_EQ(document->tasks()[0], task);

    EXPECT_EQ(document->serialise(), sedTaskExpectedSerialisation(true));

    task->setModel(nullptr);
    task->setSimulation(nullptr);

    EXPECT_EQ(task->model(), nullptr);
    EXPECT_EQ(task->simulation(), nullptr);

    EXPECT_EQ(document->serialise(), sedTaskExpectedSerialisation(false));

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Task 'task1' requires a model."},
        {libOpenCOR::Issue::Type::ERROR, "Task 'task1' requires a simulation."},
    };

    auto instance = document->instantiate();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    EXPECT_FALSE(document->addTask(task));
    EXPECT_TRUE(document->removeTask(task));

    EXPECT_FALSE(document->hasTasks());

    EXPECT_FALSE(document->removeTask(nullptr));
}

TEST(CoverageSedTest, odeSolver)
{
    auto document = libOpenCOR::SedDocument::create();
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(document);

    EXPECT_EQ(simulation->odeSolver(), nullptr);

    auto solver = libOpenCOR::SolverCvode::create();

    simulation->setOdeSolver(solver);

    EXPECT_EQ(simulation->odeSolver(), solver);

    simulation->setOdeSolver(nullptr);

    EXPECT_EQ(simulation->odeSolver(), nullptr);
}

TEST(CoverageSedTest, nlaSolver)
{
    auto document = libOpenCOR::SedDocument::create();
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(document);

    EXPECT_EQ(simulation->nlaSolver(), nullptr);

    auto solver = libOpenCOR::SolverKinsol::create();

    simulation->setNlaSolver(solver);

    EXPECT_EQ(simulation->nlaSolver(), solver);

    simulation->setNlaSolver(nullptr);

    EXPECT_EQ(simulation->nlaSolver(), nullptr);
}

TEST(CoverageSedTest, sedOneStep)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = libOpenCOR::SedOneStep::create(document);

    static const auto STEP = 1.23;

    EXPECT_EQ(simulation->step(), 1.0);

    simulation->setStep(STEP);

    EXPECT_EQ(simulation->step(), STEP);
}

TEST(CoverageSedTest, sedUniformTimeCourse)
{
    static const auto INITIAL_TIME = 1.23;
    static const auto OUTPUT_START_TIME = 4.56;
    static const auto OUTPUT_END_TIME = 7.89;
    static const auto NUMBER_OF_STEPS = 10;

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(document);

    EXPECT_EQ(simulation->initialTime(), 0.0);
    EXPECT_EQ(simulation->outputStartTime(), 0.0);
    EXPECT_EQ(simulation->outputEndTime(), 1000.0);
    EXPECT_EQ(simulation->numberOfSteps(), 1000);

    simulation->setInitialTime(INITIAL_TIME);
    simulation->setOutputStartTime(OUTPUT_START_TIME);
    simulation->setOutputEndTime(OUTPUT_END_TIME);
    simulation->setNumberOfSteps(NUMBER_OF_STEPS);

    EXPECT_EQ(simulation->initialTime(), INITIAL_TIME);
    EXPECT_EQ(simulation->outputStartTime(), OUTPUT_START_TIME);
    EXPECT_EQ(simulation->outputEndTime(), OUTPUT_END_TIME);
    EXPECT_EQ(simulation->numberOfSteps(), NUMBER_OF_STEPS);
}

TEST(CoverageSedTest, sedInstanceAndSedInstanceTask)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 2."},
    };
    static const auto UPPER_HALF_BANDWIDTH = -1;

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(document->simulations()[0]->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = document->instantiate();
    auto instanceTask = instance->tasks()[0];

    static const auto NoDoubles = std::vector<double> {};

    EXPECT_EQ(instanceTask->voi(), NoDoubles);
    EXPECT_EQ(instanceTask->voiName(), "main.t");
    EXPECT_EQ(instanceTask->voiUnit(), "dimensionless");

    EXPECT_EQ(instanceTask->stateCount(), 3);
    EXPECT_EQ(instanceTask->state(0), NoDoubles);
    EXPECT_EQ(instanceTask->state(3), NoDoubles);
    EXPECT_EQ(instanceTask->stateName(0), "main.x");
    EXPECT_EQ(instanceTask->stateName(3), "");
    EXPECT_EQ(instanceTask->stateUnit(0), "dimensionless");
    EXPECT_EQ(instanceTask->stateUnit(3), "");

    EXPECT_EQ(instanceTask->rateCount(), 3);
    EXPECT_EQ(instanceTask->rate(0), NoDoubles);
    EXPECT_EQ(instanceTask->rate(3), NoDoubles);
    EXPECT_EQ(instanceTask->rateName(0), "main.x'");
    EXPECT_EQ(instanceTask->rateName(3), "");
    EXPECT_EQ(instanceTask->rateUnit(0), "dimensionless/dimensionless");
    EXPECT_EQ(instanceTask->rateUnit(3), "");

    EXPECT_EQ(instanceTask->variableCount(), 3);
    EXPECT_EQ(instanceTask->variable(0), NoDoubles);
    EXPECT_EQ(instanceTask->variable(3), NoDoubles);
    EXPECT_EQ(instanceTask->variableName(0), "main.sigma");
    EXPECT_EQ(instanceTask->variableName(3), "");
    EXPECT_EQ(instanceTask->variableUnit(0), "dimensionless");
    EXPECT_EQ(instanceTask->variableUnit(3), "");

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CoverageSedTest, solver)
{
    // Get the duplicate() method of different solvers to be covered.

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    document->simulations()[0]->setOdeSolver(libOpenCOR::SolverForwardEuler::create());

    auto instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    document->simulations()[0]->setOdeSolver(libOpenCOR::SolverFourthOrderRungeKutta::create());

    instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    document->simulations()[0]->setOdeSolver(libOpenCOR::SolverHeun::create());

    instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    document->simulations()[0]->setOdeSolver(libOpenCOR::SolverSecondOrderRungeKutta::create());

    instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}
