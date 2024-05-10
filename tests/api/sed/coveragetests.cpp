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

    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_EQ(sed->serialise(), expectedSerialisation);
}

TEST(CoverageSedTest, sedDocumentModels)
{
    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(sed->hasModels());
    EXPECT_FALSE(sed->addModel(nullptr));

    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto model = libOpenCOR::SedModel::create(sed, file);

    EXPECT_TRUE(sed->addModel(model));

    EXPECT_EQ(sed->models().size(), 1);
    EXPECT_EQ(sed->models()[0], model);

    EXPECT_FALSE(sed->addModel(model));
    EXPECT_TRUE(sed->removeModel(model));

    EXPECT_FALSE(sed->hasModels());

    EXPECT_FALSE(sed->removeModel(nullptr));
}

TEST(CoverageSedTest, sedDocumentSimulations)
{
    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(sed->hasSimulations());
    EXPECT_FALSE(sed->addSimulation(nullptr));

    auto uniformTimeCourse = libOpenCOR::SedUniformTimeCourse::create(sed);
    auto steadyState = libOpenCOR::SedSteadyState::create(sed);

    EXPECT_TRUE(sed->addSimulation(uniformTimeCourse));
    EXPECT_TRUE(sed->addSimulation(steadyState));

    EXPECT_EQ(sed->simulations().size(), 2);
    EXPECT_EQ(sed->simulations()[0], uniformTimeCourse);
    EXPECT_EQ(sed->simulations()[1], steadyState);

    EXPECT_FALSE(sed->addSimulation(uniformTimeCourse));
    EXPECT_TRUE(sed->removeSimulation(uniformTimeCourse));

    EXPECT_FALSE(sed->addSimulation(steadyState));
    EXPECT_TRUE(sed->removeSimulation(steadyState));

    EXPECT_FALSE(sed->hasSimulations());

    EXPECT_FALSE(sed->removeSimulation(nullptr));
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

TEST(CoverageSedTest, sedDocumentTasks)
{
    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(sed->hasTasks());
    EXPECT_FALSE(sed->addTask(nullptr));

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto model = libOpenCOR::SedModel::create(sed, file);
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(sed);
    auto task = libOpenCOR::SedTask::create(sed, model, simulation);

    EXPECT_NE(task->model(), nullptr);
    EXPECT_NE(task->simulation(), nullptr);

    EXPECT_TRUE(sed->addTask(task));

    EXPECT_EQ(sed->tasks().size(), 1);
    EXPECT_EQ(sed->tasks()[0], task);

    EXPECT_EQ(sed->serialise(), sedTaskExpectedSerialisation(true));

    task->setModel(nullptr);
    task->setSimulation(nullptr);

    EXPECT_EQ(task->model(), nullptr);
    EXPECT_EQ(task->simulation(), nullptr);

    EXPECT_EQ(sed->serialise(), sedTaskExpectedSerialisation(false));

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Task 'task1' requires a model."},
        {libOpenCOR::Issue::Type::ERROR, "Task 'task1' requires a simulation."},
    };

    auto instance = sed->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    EXPECT_FALSE(sed->addTask(task));
    EXPECT_TRUE(sed->removeTask(task));

    EXPECT_FALSE(sed->hasTasks());

    EXPECT_FALSE(sed->removeTask(nullptr));
}

TEST(CoverageSedTest, sedSimulationOdeSolver)
{
    auto sed = libOpenCOR::SedDocument::create();
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(sed);

    EXPECT_EQ(simulation->odeSolver(), nullptr);

    auto solver = libOpenCOR::SolverCvode::create();

    simulation->setOdeSolver(solver);

    EXPECT_EQ(simulation->odeSolver(), solver);

    simulation->setOdeSolver(nullptr);

    EXPECT_EQ(simulation->odeSolver(), nullptr);
}

TEST(CoverageSedTest, sedSimulationNlaSolver)
{
    auto sed = libOpenCOR::SedDocument::create();
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(sed);

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
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = libOpenCOR::SedOneStep::create(sed);

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
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(sed);

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
    auto sed = libOpenCOR::SedDocument::create(file);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(sed->simulations()[0]->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = sed->createInstance();
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
    auto sed = libOpenCOR::SedDocument::create(file);

    sed->simulations()[0]->setOdeSolver(libOpenCOR::SolverForwardEuler::create());

    auto instance = sed->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    sed->simulations()[0]->setOdeSolver(libOpenCOR::SolverFourthOrderRungeKutta::create());

    instance = sed->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    sed->simulations()[0]->setOdeSolver(libOpenCOR::SolverHeun::create());

    instance = sed->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());

    sed->simulations()[0]->setOdeSolver(libOpenCOR::SolverSecondOrderRungeKutta::create());

    instance = sed->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}
