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

TEST(CoverageSedTest, initialise)
{
    static const std::string expectedSerialisation = R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
)";

    auto document = libOpenCOR::SedDocument::create();

    EXPECT_EQ(document->serialise(), expectedSerialisation);
}

TEST(CoverageSedTest, models)
{
    auto document = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(document->hasModels());
    EXPECT_EQ(document->modelCount(), 0);
    EXPECT_EQ(document->models().size(), 0);
    EXPECT_FALSE(document->addModel(nullptr));

    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto model = libOpenCOR::SedModel::create(document, file);

    EXPECT_EQ(model->file(), file);

    EXPECT_TRUE(document->addModel(model));

    EXPECT_TRUE(document->hasModels());
    EXPECT_EQ(document->modelCount(), 1);
    EXPECT_EQ(document->models().size(), 1);
    EXPECT_EQ(document->models()[0], model);
    EXPECT_EQ(document->model(0), model);
    EXPECT_EQ(document->model(1), nullptr);

    EXPECT_FALSE(document->addModel(model));
    EXPECT_TRUE(document->removeModel(model));

    EXPECT_FALSE(document->hasModels());
    EXPECT_EQ(document->modelCount(), 0);
    EXPECT_EQ(document->models().size(), 0);

    EXPECT_FALSE(document->removeModel(nullptr));
    EXPECT_FALSE(document->removeAllModels());

    EXPECT_TRUE(document->addModel(model));
    EXPECT_TRUE(document->removeAllModels());

    EXPECT_FALSE(model->hasChanges());
    EXPECT_EQ(model->changeCount(), 0);
    EXPECT_EQ(model->changes().size(), 0);
    EXPECT_FALSE(model->addChange(nullptr));
    EXPECT_FALSE(model->removeAllChanges());

    auto changeAttribute = libOpenCOR::SedChangeAttribute::create("component", "variable", "newValue");

    EXPECT_TRUE(model->addChange(changeAttribute));

    EXPECT_TRUE(model->hasChanges());
    EXPECT_EQ(model->changeCount(), 1);
    EXPECT_EQ(model->changes().size(), 1);
    EXPECT_EQ(model->changes()[0], changeAttribute);
    EXPECT_EQ(model->change(0), changeAttribute);
    EXPECT_EQ(model->change(1), nullptr);

    EXPECT_FALSE(model->addChange(changeAttribute));
    EXPECT_TRUE(model->removeChange(changeAttribute));

    EXPECT_TRUE(model->addChange(changeAttribute));
    EXPECT_TRUE(model->removeAllChanges());

    EXPECT_FALSE(model->hasChanges());
    EXPECT_EQ(model->changeCount(), 0);
    EXPECT_EQ(model->changes().size(), 0);

    EXPECT_FALSE(model->removeChange(nullptr));
}

namespace {

std::string sedChangeExpectedSerialisation(const std::string &pComponent, const std::string &pVariable,
                                           const std::string &pNewValue)
{
    return std::string(R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source=")"
                       +
#ifdef BUILDING_USING_MSVC
                       std::string("file:///P:/some/path/file.txt")
#else
                       std::string("file:///some/path/file.txt")
#endif
                       +
                       R"(">
      <listOfChanges>
        <changeAttribute target="/cellml:model/cellml:component[@name=')"
                       + pComponent + R"(']/cellml:variable[@name=')"
                       + pVariable + R"(']" newValue=")"
                       + pNewValue + R"("/>
      </listOfChanges>
    </model>
  </listOfModels>
</sedML>
)");
}

} // namespace

TEST(CoverageSedTest, changes)
{
    auto changeAttribute = libOpenCOR::SedChangeAttribute::create("component", "variable", "123.456789");

    EXPECT_EQ(changeAttribute->target(), "/cellml:model/cellml:component[@name='component']/cellml:variable[@name='variable']");
    EXPECT_EQ(changeAttribute->componentName(), "component");
    EXPECT_EQ(changeAttribute->variableName(), "variable");
    EXPECT_EQ(changeAttribute->newValue(), "123.456789");

    auto document = libOpenCOR::SedDocument::create();
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto model = libOpenCOR::SedModel::create(document, file);

    EXPECT_TRUE(model->addChange(changeAttribute));
    EXPECT_TRUE(document->addModel(model));

    EXPECT_EQ(document->serialise(), sedChangeExpectedSerialisation("component", "variable", "123.456789"));

    changeAttribute->setComponentName("new_component");
    changeAttribute->setVariableName("new_variable");
    changeAttribute->setNewValue("987.654321");

    EXPECT_EQ(changeAttribute->target(), "/cellml:model/cellml:component[@name='new_component']/cellml:variable[@name='new_variable']");
    EXPECT_EQ(changeAttribute->componentName(), "new_component");
    EXPECT_EQ(changeAttribute->variableName(), "new_variable");
    EXPECT_EQ(changeAttribute->newValue(), "987.654321");

    EXPECT_EQ(document->serialise(), sedChangeExpectedSerialisation("new_component", "new_variable", "987.654321"));
}

TEST(CoverageSedTest, simulations)
{
    auto document = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(document->hasSimulations());
    EXPECT_EQ(document->simulationCount(), 0);
    EXPECT_EQ(document->simulations().size(), 0);
    EXPECT_FALSE(document->addSimulation(nullptr));

    auto uniformTimeCourse = libOpenCOR::SedUniformTimeCourse::create(document);
    auto oneStep = libOpenCOR::SedOneStep::create(document);
    auto steadyState = libOpenCOR::SedSteadyState::create(document);
    auto analysis = libOpenCOR::SedAnalysis::create(document);

    EXPECT_TRUE(document->addSimulation(uniformTimeCourse));
    EXPECT_TRUE(document->addSimulation(oneStep));
    EXPECT_TRUE(document->addSimulation(steadyState));
    EXPECT_TRUE(document->addSimulation(analysis));

    EXPECT_TRUE(document->hasSimulations());
    EXPECT_EQ(document->simulationCount(), 4);
    EXPECT_EQ(document->simulations().size(), 4);
    EXPECT_EQ(document->simulations()[0], uniformTimeCourse);
    EXPECT_EQ(document->simulations()[1], oneStep);
    EXPECT_EQ(document->simulations()[2], steadyState);
    EXPECT_EQ(document->simulations()[3], analysis);
    EXPECT_EQ(document->simulation(0), uniformTimeCourse);
    EXPECT_EQ(document->simulation(1), oneStep);
    EXPECT_EQ(document->simulation(2), steadyState);
    EXPECT_EQ(document->simulation(3), analysis);
    EXPECT_EQ(document->simulation(4), nullptr);

    EXPECT_FALSE(document->addSimulation(uniformTimeCourse));
    EXPECT_TRUE(document->removeSimulation(uniformTimeCourse));

    EXPECT_FALSE(document->addSimulation(oneStep));
    EXPECT_TRUE(document->removeSimulation(oneStep));

    EXPECT_FALSE(document->addSimulation(steadyState));
    EXPECT_TRUE(document->removeSimulation(steadyState));

    EXPECT_FALSE(document->addSimulation(analysis));
    EXPECT_TRUE(document->removeSimulation(analysis));

    EXPECT_FALSE(document->hasSimulations());
    EXPECT_EQ(document->simulationCount(), 0);
    EXPECT_EQ(document->simulations().size(), 0);

    EXPECT_FALSE(document->removeSimulation(nullptr));
    EXPECT_FALSE(document->removeAllSimulations());

    EXPECT_TRUE(document->addSimulation(uniformTimeCourse));
    EXPECT_TRUE(document->removeAllSimulations());
}

namespace {

std::string sedTaskExpectedSerialisation(bool pWithProperties)
{
    return std::string(R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfTasks>
    <task id="task1")")
        .append(pWithProperties ? R"( modelReference="model1" simulationReference="simulation1")" : "")
        .append(R"(/>
  </listOfTasks>
</sedML>
)");
}

} // namespace

TEST(CoverageSedTest, tasks)
{
    auto document = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(document->hasTasks());
    EXPECT_EQ(document->taskCount(), 0);
    EXPECT_EQ(document->tasks().size(), 0);
    EXPECT_FALSE(document->addTask(nullptr));

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto model = libOpenCOR::SedModel::create(document, file);
    auto simulation = libOpenCOR::SedUniformTimeCourse::create(document);
    auto task = libOpenCOR::SedTask::create(document, model, simulation);

    EXPECT_NE(task->model(), nullptr);
    EXPECT_NE(task->simulation(), nullptr);

    EXPECT_TRUE(document->addTask(task));

    EXPECT_TRUE(document->hasTasks());
    EXPECT_EQ(document->taskCount(), 1);
    EXPECT_EQ(document->tasks().size(), 1);
    EXPECT_EQ(document->tasks()[0], task);
    EXPECT_EQ(document->task(0), task);
    EXPECT_EQ(document->task(1), nullptr);

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
    EXPECT_EQ(document->taskCount(), 0);
    EXPECT_EQ(document->tasks().size(), 0);

    EXPECT_FALSE(document->removeTask(nullptr));
    EXPECT_FALSE(document->removeAllTasks());

    EXPECT_TRUE(document->addTask(task));
    EXPECT_TRUE(document->removeAllTasks());
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

TEST(CoverageSedTest, sedBase)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = libOpenCOR::SedOneStep::create(document);

    EXPECT_EQ(simulation->id(), "simulation2");
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
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3."},
    };
    static const auto UPPER_HALF_BANDWIDTH = -1;

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(document->simulations()[0]->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);

    auto instance = document->instantiate();
    auto instanceTask = instance->tasks()[0];

    EXPECT_EQ(instance->hasTasks(), true);
    EXPECT_EQ(instance->taskCount(), 1);
    EXPECT_EQ(instance->task(0), instanceTask);
    EXPECT_EQ(instance->task(1), nullptr);

    static const auto NoDoubles = std::vector<double> {};

    EXPECT_EQ(instanceTask->voi(), NoDoubles);
    EXPECT_EQ(instanceTask->voiName(), "environment/time");
    EXPECT_EQ(instanceTask->voiUnit(), "millisecond");

    EXPECT_EQ(instanceTask->stateCount(), 4);
    EXPECT_EQ(instanceTask->state(0), NoDoubles);
    EXPECT_EQ(instanceTask->state(4), NoDoubles);
    EXPECT_EQ(instanceTask->stateName(0), "membrane/V");
    EXPECT_EQ(instanceTask->stateName(4), "");
    EXPECT_EQ(instanceTask->stateUnit(0), "millivolt");
    EXPECT_EQ(instanceTask->stateUnit(4), "");

    EXPECT_EQ(instanceTask->rateCount(), 4);
    EXPECT_EQ(instanceTask->rate(0), NoDoubles);
    EXPECT_EQ(instanceTask->rate(4), NoDoubles);
    EXPECT_EQ(instanceTask->rateName(0), "membrane/V'");
    EXPECT_EQ(instanceTask->rateName(4), "");
    EXPECT_EQ(instanceTask->rateUnit(0), "millivolt/millisecond");
    EXPECT_EQ(instanceTask->rateUnit(4), "");

    EXPECT_EQ(instanceTask->constantCount(), 5);
    EXPECT_EQ(instanceTask->constant(0), NoDoubles);
    EXPECT_EQ(instanceTask->constant(5), NoDoubles);
    EXPECT_EQ(instanceTask->constantName(0), "membrane/Cm");
    EXPECT_EQ(instanceTask->constantName(5), "");
    EXPECT_EQ(instanceTask->constantUnit(0), "microF_per_cm2");
    EXPECT_EQ(instanceTask->constantUnit(5), "");

    EXPECT_EQ(instanceTask->computedConstantCount(), 3);
    EXPECT_EQ(instanceTask->computedConstant(0), NoDoubles);
    EXPECT_EQ(instanceTask->computedConstant(3), NoDoubles);
    EXPECT_EQ(instanceTask->computedConstantName(0), "leakage_current/E_L");
    EXPECT_EQ(instanceTask->computedConstantName(3), "");
    EXPECT_EQ(instanceTask->computedConstantUnit(0), "millivolt");
    EXPECT_EQ(instanceTask->computedConstantUnit(3), "");

    EXPECT_EQ(instanceTask->algebraicCount(), 10);
    EXPECT_EQ(instanceTask->algebraic(0), NoDoubles);
    EXPECT_EQ(instanceTask->algebraic(10), NoDoubles);
    EXPECT_EQ(instanceTask->algebraicName(0), "membrane/i_Stim");
    EXPECT_EQ(instanceTask->algebraicName(10), "");
    EXPECT_EQ(instanceTask->algebraicUnit(0), "microA_per_cm2");
    EXPECT_EQ(instanceTask->algebraicUnit(10), "");

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CoverageSedTest, sedDocument)
{
    auto file = libOpenCOR::File::create(libOpenCOR::HTTP_REMOTE_CELLML_FILE);
    libOpenCOR::SedDocument::create(file);

    file = libOpenCOR::File::create(libOpenCOR::HTTP_REMOTE_SEDML_FILE);
    libOpenCOR::SedDocument::create(file);

    file = libOpenCOR::File::create(libOpenCOR::HTTP_REMOTE_COMBINE_ARCHIVE);
    libOpenCOR::SedDocument::create(file);
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
