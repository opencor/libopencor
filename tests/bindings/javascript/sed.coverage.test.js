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
    const sed = new libopencor.SedDocument();

    expect(sed.serialise()).toBe(expectedSerialisation);
  });

  test("Model", () => {
    const sed = new libopencor.SedDocument();

    expect(sed.hasModels()).toBe(false);
    expect(sed.addModel(null)).toBe(false);

    const file = new libopencor.File(utils.LOCAL_FILE);
    const model = new libopencor.SedModel(sed, file);

    expect(sed.addModel(model)).toBe(true);

    expect(sed.models().size()).toBe(1);
    expect(sed.models().get(0)).toStrictEqual(model);

    expect(sed.addModel(model)).toBe(false);
    expect(sed.removeModel(model)).toBe(true);

    expect(sed.hasModels()).toBe(false);

    expect(sed.removeModel(null)).toBe(false);
  });

  test("Simulations", () => {
    const sed = new libopencor.SedDocument();

    expect(sed.hasSimulations()).toBe(false);
    expect(sed.addSimulation(null)).toBe(false);

    const simulation = new libopencor.SedUniformTimeCourse(sed);

    expect(sed.addSimulation(simulation)).toBe(true);

    expect(sed.simulations().size()).toBe(1);
    expect(sed.simulations().get(0)).toStrictEqual(simulation);

    expect(sed.addSimulation(simulation)).toBe(false);
    expect(sed.removeSimulation(simulation)).toBe(true);

    expect(sed.hasSimulations()).toBe(false);

    expect(sed.removeSimulation(null)).toBe(false);
  });

  test("Tasks", () => {
    const sed = new libopencor.SedDocument();

    expect(sed.hasTasks()).toBe(false);
    expect(sed.addTask(null)).toBe(false);

    const file = new libopencor.File(utils.LOCAL_FILE);
    const model = new libopencor.SedModel(sed, file);
    const simulation = new libopencor.SedUniformTimeCourse(sed);
    const task = new libopencor.SedTask(sed, model, simulation);

    expect(task.model()).not.toBe(null);
    expect(task.simulation()).not.toBe(null);

    expect(sed.addTask(task)).toBe(true);

    expect(sed.tasks().size()).toBe(1);
    expect(sed.tasks().get(0)).toStrictEqual(task);

    expect(sed.serialise()).toBe(sedTaskExpectedSerialisation(true));

    task.setModel(null);
    task.setSimulation(null);

    expect(task.model()).toBe(null);
    expect(task.simulation()).toBe(null);

    expect(sed.serialise()).toBe(sedTaskExpectedSerialisation(false));

    const instance = sed.createInstance();

    expectIssues(instance, [
      [libopencor.Issue.Type.ERROR, "Task 'task1' requires a model."],
      [libopencor.Issue.Type.ERROR, "Task 'task1' requires a simulation."],
    ]);

    expect(sed.addTask(task)).toBe(false);
    expect(sed.removeTask(task)).toBe(true);

    expect(sed.hasTasks()).toBe(false);

    expect(sed.removeTask(null)).toBe(false);
  });

  test("ODE solver", () => {
    const sed = new libopencor.SedDocument();
    const simulation = new libopencor.SedUniformTimeCourse(sed);

    expect(simulation.odeSolver()).toBe(null);

    const solver = new libopencor.SolverCvode();

    simulation.setOdeSolver(solver);

    expect(simulation.odeSolver()).toStrictEqual(solver);

    simulation.setOdeSolver(null);

    expect(simulation.odeSolver()).toBe(null);
  });

  test("NLA solver", () => {
    const sed = new libopencor.SedDocument();
    const simulation = new libopencor.SedUniformTimeCourse(sed);

    expect(simulation.nlaSolver()).toBe(null);

    const solver = new libopencor.SolverKinsol();

    simulation.setNlaSolver(solver);

    expect(simulation.nlaSolver()).toStrictEqual(solver);

    simulation.setNlaSolver(null);

    expect(simulation.nlaSolver()).toBe(null);
  });

  test("SedOneStep", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = new libopencor.SedOneStep(sed);

    expect(simulation.step()).toBe(1.0);

    simulation.setStep(1.23);

    expect(simulation.step()).toBe(1.23);
  });

  test("SedUniformTimeCourse", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = new libopencor.SedUniformTimeCourse(sed);

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
  });

  test("Solver", () => {
    // Get the duplicate() method of different solvers to be covered.

    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    sed.simulations().get(0).setOdeSolver(new libopencor.SolverForwardEuler());

    let instance = sed.createInstance();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    sed
      .simulations()
      .get(0)
      .setOdeSolver(new libopencor.SolverFourthOrderRungeKutta());

    instance = sed.createInstance();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    sed.simulations().get(0).setOdeSolver(new libopencor.SolverHeun());

    instance = sed.createInstance();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    sed
      .simulations()
      .get(0)
      .setOdeSolver(new libopencor.SolverSecondOrderRungeKutta());

    instance = sed.createInstance();

    instance.run();

    expect(instance.hasIssues()).toBe(false);
  });

  test("SedInstanceTask", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const solver = sed.simulations().get(0).odeSolver();

    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.BANDED);
    solver.setUpperHalfBandwidth(-1);

    const instance = sed.createInstance();

    instance.run();

    /*---GRY--- TO BE RE-ENABLED ONCE WE CAN RUN A SIMULATION FROM JavaScript.
    expectIssues(instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 2.",
      ],
    ]);
    */
  });
});
