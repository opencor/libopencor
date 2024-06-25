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

import libOpenCOR from "./libopencor.js";
import * as utils from "./utils.js";
import { expectIssues } from "./utils.js";

const libopencor = await libOpenCOR();

describe("Sed coverage tests", () => {
  let someCellmlContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
  });

  function sedTaskExpectedSerialisation(withProperties) {
    return (
      `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfTasks>
    <task id="task1"` +
      (withProperties
        ? ` modelReference="model1" simulationReference="simulation1"`
        : ``) +
      `/>
  </listOfTasks>
</sedML>
`
    );
  }

  test("Initialise", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
`;
    const document = new libopencor.SedDocument();

    expect(document.serialise()).toBe(expectedSerialisation);

    document.delete();
  });

  test("Models", () => {
    const document = new libopencor.SedDocument();

    expect(document.hasModels()).toBe(false);
    expect(document.modelCount()).toBe(0);
    expect(document.models().size()).toBe(0);
    expect(document.addModel(null)).toBe(false);

    const file = new libopencor.File(utils.SEDML_FILE);
    const model = new libopencor.SedModel(document, file);

    expect(document.addModel(model)).toBe(true);

    expect(document.hasModels()).toBe(true);
    expect(document.modelCount()).toBe(1);
    expect(document.models().size()).toBe(1);
    expect(document.models().get(0)).toStrictEqual(model);

    expect(document.addModel(model)).toBe(false);
    expect(document.removeModel(model)).toBe(true);

    expect(document.hasModels()).toBe(false);
    expect(document.modelCount()).toBe(0);
    expect(document.models().size()).toBe(0);

    expect(document.removeModel(null)).toBe(false);

    model.delete();
    file.delete();
    document.delete();
  });

  test("Simulations", () => {
    const document = new libopencor.SedDocument();

    expect(document.hasSimulations()).toBe(false);
    expect(document.simulationCount()).toBe(0);
    expect(document.simulations().size()).toBe(0);
    expect(document.addSimulation(null)).toBe(false);

    const uniform_time_course = new libopencor.SedUniformTimeCourse(document);
    const one_step = new libopencor.SedOneStep(document);
    const steady_state = new libopencor.SedSteadyState(document);
    const analysis = new libopencor.SedAnalysis(document);

    expect(document.addSimulation(uniform_time_course)).toBe(true);
    expect(document.addSimulation(one_step)).toBe(true);
    expect(document.addSimulation(steady_state)).toBe(true);
    expect(document.addSimulation(analysis)).toBe(true);

    expect(document.hasSimulations()).toBe(true);
    expect(document.simulationCount()).toBe(4);
    expect(document.simulations().size()).toBe(4);
    expect(document.simulations().get(0)).toStrictEqual(uniform_time_course);
    expect(document.simulations().get(1)).toStrictEqual(one_step);
    expect(document.simulations().get(2)).toStrictEqual(steady_state);
    expect(document.simulations().get(3)).toStrictEqual(analysis);

    expect(document.addSimulation(uniform_time_course)).toBe(false);
    expect(document.removeSimulation(uniform_time_course)).toBe(true);

    expect(document.addSimulation(one_step)).toBe(false);
    expect(document.removeSimulation(one_step)).toBe(true);

    expect(document.addSimulation(steady_state)).toBe(false);
    expect(document.removeSimulation(steady_state)).toBe(true);

    expect(document.addSimulation(analysis)).toBe(false);
    expect(document.removeSimulation(analysis)).toBe(true);

    expect(document.hasSimulations()).toBe(false);
    expect(document.simulationCount()).toBe(0);
    expect(document.simulations().size()).toBe(0);

    expect(document.removeSimulation(null)).toBe(false);

    steady_state.delete();
    uniform_time_course.delete();
    document.delete();
  });

  test("Tasks", () => {
    const document = new libopencor.SedDocument();

    expect(document.hasTasks()).toBe(false);
    expect(document.taskCount()).toBe(0);
    expect(document.tasks().size()).toBe(0);
    expect(document.addTask(null)).toBe(false);

    const file = new libopencor.File(utils.SEDML_FILE);
    const model = new libopencor.SedModel(document, file);
    const simulation = new libopencor.SedUniformTimeCourse(document);
    const task = new libopencor.SedTask(document, model, simulation);

    expect(task.model()).not.toBe(null);
    expect(task.simulation()).not.toBe(null);

    expect(document.addTask(task)).toBe(true);

    expect(document.hasTasks()).toBe(true);
    expect(document.taskCount()).toBe(1);
    expect(document.tasks().size()).toBe(1);
    expect(document.tasks().get(0)).toStrictEqual(task);

    expect(document.serialise()).toBe(sedTaskExpectedSerialisation(true));

    task.setModel(null);
    task.setSimulation(null);

    expect(task.model()).toBe(null);
    expect(task.simulation()).toBe(null);

    expect(document.serialise()).toBe(sedTaskExpectedSerialisation(false));

    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [libopencor.Issue.Type.ERROR, "Task 'task1' requires a model."],
      [libopencor.Issue.Type.ERROR, "Task 'task1' requires a simulation."],
    ]);

    expect(document.addTask(task)).toBe(false);
    expect(document.removeTask(task)).toBe(true);

    expect(document.hasTasks()).toBe(false);
    expect(document.taskCount()).toBe(0);
    expect(document.tasks().size()).toBe(0);

    expect(document.removeTask(null)).toBe(false);

    task.delete();
    simulation.delete();
    model.delete();
    file.delete();
    document.delete();
  });

  test("ODE solver", () => {
    const document = new libopencor.SedDocument();
    const simulation = new libopencor.SedUniformTimeCourse(document);

    expect(simulation.odeSolver()).toBe(null);

    const solver = new libopencor.SolverCvode();

    simulation.setOdeSolver(solver);

    expect(simulation.odeSolver()).toStrictEqual(solver);

    simulation.setOdeSolver(null);

    expect(simulation.odeSolver()).toBe(null);

    solver.delete();
    simulation.delete();
    document.delete();
  });

  test("NLA solver", () => {
    const document = new libopencor.SedDocument();
    const simulation = new libopencor.SedUniformTimeCourse(document);

    expect(simulation.nlaSolver()).toBe(null);

    const solver = new libopencor.SolverKinsol();

    simulation.setNlaSolver(solver);

    expect(simulation.nlaSolver()).toStrictEqual(solver);

    simulation.setNlaSolver(null);

    expect(simulation.nlaSolver()).toBe(null);

    solver.delete();
    simulation.delete();
    document.delete();
  });

  test("SedOneStep", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));
    const document = new libopencor.SedDocument(file);
    const simulation = new libopencor.SedOneStep(document);

    expect(simulation.step()).toBe(1.0);

    simulation.setStep(1.23);

    expect(simulation.step()).toBe(1.23);

    simulation.delete();
    document.delete();
    file.delete();
  });

  test("SedUniformTimeCourse", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));
    const document = new libopencor.SedDocument(file);
    const simulation = new libopencor.SedUniformTimeCourse(document);

    expect(simulation.initialTime()).toBe(0.0);
    expect(simulation.outputStartTime()).toBe(0.0);
    expect(simulation.outputEndTime()).toBe(1000.0);
    expect(simulation.numberOfSteps()).toBe(1000);

    simulation.setInitialTime(1.23);
    simulation.setOutputStartTime(4.56);
    simulation.setOutputEndTime(7.89);
    simulation.setNumberOfSteps(10);

    expect(simulation.initialTime()).toBe(1.23);
    expect(simulation.outputStartTime()).toBe(4.56);
    expect(simulation.outputEndTime()).toBe(7.89);
    expect(simulation.numberOfSteps()).toBe(10);

    simulation.delete();
    document.delete();
    file.delete();
  });

  test("SedInstanceAndSedInstanceTask", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const solver = document.simulations().get(0).odeSolver();

    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.BANDED);
    solver.setUpperHalfBandwidth(-1);

    const instance = document.instantiate();
    const instanceTask = instance.tasks().get(0);

    expect(instanceTask.voi().size()).toBe(0);
    expect(instanceTask.voiAsArray()).toStrictEqual([]);
    expect(instanceTask.voiName()).toBe("main.t");
    expect(instanceTask.voiUnit()).toBe("dimensionless");

    expect(instanceTask.stateCount()).toBe(3);
    expect(instanceTask.state(0).size()).toBe(0);
    expect(instanceTask.stateAsArray(0)).toStrictEqual([]);
    expect(instanceTask.state(3).size()).toBe(0);
    expect(instanceTask.stateAsArray(3)).toStrictEqual([]);
    expect(instanceTask.stateName(0)).toBe("main.x");
    expect(instanceTask.stateName(3)).toBe("");
    expect(instanceTask.stateUnit(0)).toBe("dimensionless");
    expect(instanceTask.stateUnit(3)).toBe("");

    expect(instanceTask.rateCount()).toBe(3);
    expect(instanceTask.rate(0).size()).toBe(0);
    expect(instanceTask.rateAsArray(0)).toStrictEqual([]);
    expect(instanceTask.rate(3).size()).toBe(0);
    expect(instanceTask.rateAsArray(3)).toStrictEqual([]);
    expect(instanceTask.rateName(0)).toBe("main.x'");
    expect(instanceTask.rateName(3)).toBe("");
    expect(instanceTask.rateUnit(0)).toBe("dimensionless/dimensionless");
    expect(instanceTask.rateUnit(3)).toBe("");

    expect(instanceTask.variableCount()).toBe(3);
    expect(instanceTask.variable(0).size()).toBe(0);
    expect(instanceTask.variableAsArray(0)).toStrictEqual([]);
    expect(instanceTask.variable(3).size()).toBe(0);
    expect(instanceTask.variableAsArray(3)).toStrictEqual([]);
    expect(instanceTask.variableName(0)).toBe("main.sigma");
    expect(instanceTask.variableName(3)).toBe("");
    expect(instanceTask.variableUnit(0)).toBe("dimensionless");
    expect(instanceTask.variableUnit(3)).toBe("");

    instance.run();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 2.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Solver", () => {
    // Get the duplicate() method of different solvers to be covered.

    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    let solver = new libopencor.SolverForwardEuler();

    simulation.setOdeSolver(solver);

    let instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    solver.delete();

    solver = new libopencor.SolverFourthOrderRungeKutta();

    simulation.setOdeSolver(solver);

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    solver.delete();

    solver = new libopencor.SolverHeun();

    simulation.setOdeSolver(solver);

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    solver.delete();

    solver = new libopencor.SolverSecondOrderRungeKutta();

    simulation.setOdeSolver(solver);

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    solver.delete();
    document.delete();
    file.delete();
  });
});
