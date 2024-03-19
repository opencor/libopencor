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

TEST(CoverageSedDocumentTest, initialise)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\"/>\n";

    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_EQ(sed->serialise(), expectedSerialisation);
}

TEST(CoverageSedDocumentTest, sedDocumentModels)
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

TEST(CoverageSedDocumentTest, sedDocumentSimulations)
{
    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(sed->hasSimulations());
    EXPECT_FALSE(sed->addSimulation(nullptr));

    auto simulation = libOpenCOR::SedUniformTimeCourse::create(sed);

    EXPECT_TRUE(sed->addSimulation(simulation));

    EXPECT_EQ(sed->simulations().size(), 1);
    EXPECT_EQ(sed->simulations()[0], simulation);

    EXPECT_FALSE(sed->addSimulation(simulation));
    EXPECT_TRUE(sed->removeSimulation(simulation));

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

TEST(CoverageSedDocumentTest, sedDocumentTasks)
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

    auto job = sed->run();

    EXPECT_EQ_ISSUES(job, expectedIssues);

    EXPECT_FALSE(sed->addTask(task));
    EXPECT_TRUE(sed->removeTask(task));

    EXPECT_FALSE(sed->hasTasks());

    EXPECT_FALSE(sed->removeTask(nullptr));
}

TEST(CoverageSedDocumentTest, sedSimulationOdeSolver)
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

TEST(CoverageSedDocumentTest, sedSimulationNlaSolver)
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

TEST(CoverageSedDocumentTest, sedOneStep)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = libOpenCOR::SedOneStep::create(sed);

    static const auto STEP = 1.23;

    EXPECT_EQ(simulation->step(), 1.0);

    simulation->setStep(STEP);

    EXPECT_EQ(simulation->step(), STEP);
}

TEST(CoverageSedDocumentTest, sedUniformTimeCourse)
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
