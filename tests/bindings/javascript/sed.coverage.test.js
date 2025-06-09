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

const loc = await libOpenCOR();

describe("Sed coverage tests", () => {
  let sedChangesContentsPtr;
  let invalidSedChangesContentsPtr;
  let unsupportedSedChangesContentsPtr;
  let someSolverOdeContentsPtr;
  let someSolverNla1ContentsPtr;

  beforeAll(() => {
    sedChangesContentsPtr = utils.allocateMemory(
      loc,
      utils.SED_CHANGES_CONTENTS,
    );
    invalidSedChangesContentsPtr = utils.allocateMemory(
      loc,
      utils.INVALID_SED_CHANGES_CONTENTS,
    );
    unsupportedSedChangesContentsPtr = utils.allocateMemory(
      loc,
      utils.UNSUPPORTED_SED_CHANGES_CONTENTS,
    );
    someSolverOdeContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_SOLVER_ODE_CONTENTS,
    );
    someSolverNla1ContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_SOLVER_NLA1_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(loc, sedChangesContentsPtr);
    utils.freeMemory(loc, invalidSedChangesContentsPtr);
    utils.freeMemory(loc, unsupportedSedChangesContentsPtr);
    utils.freeMemory(loc, someSolverOdeContentsPtr);
    utils.freeMemory(loc, someSolverNla1ContentsPtr);
  });

  function sedTaskExpectedSerialisation(withProperties) {
    return `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfTasks>
    <task id="task1"${withProperties ? ` modelReference="model1" simulationReference="simulation1"` : ``}/>
  </listOfTasks>
</sedML>
`;
  }

  test("Initialise", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
`;
    const document = new loc.SedDocument();

    expect(document.serialise()).toBe(expectedSerialisation);

    document.delete();
  });

  test("Models", () => {
    const document = new loc.SedDocument();

    expect(document.hasModels).toBe(false);
    expect(document.modelCount).toBe(0);
    expect(document.models.size()).toBe(0);
    expect(document.addModel(null)).toBe(false);

    const file = new loc.File(utils.SEDML_FILE);
    const model = new loc.SedModel(document, file);

    expect(model.file).toStrictEqual(file);

    expect(document.addModel(model)).toBe(true);

    expect(document.hasModels).toBe(true);
    expect(document.modelCount).toBe(1);
    expect(document.models.size()).toBe(1);
    expect(document.models.get(0)).toStrictEqual(model);
    expect(document.model(0)).toStrictEqual(model);
    expect(document.model(1)).toBeNull();

    expect(document.addModel(model)).toBe(false);
    expect(document.removeModel(model)).toBe(true);

    expect(document.hasModels).toBe(false);
    expect(document.modelCount).toBe(0);
    expect(document.models.size()).toBe(0);

    expect(document.removeModel(null)).toBe(false);
    expect(document.removeAllModels()).toBe(false);

    expect(document.addModel(model)).toBe(true);
    expect(document.removeAllModels()).toBe(true);

    expect(model.hasChanges).toBe(false);
    expect(model.changeCount).toBe(0);
    expect(model.changes.size()).toBe(0);
    expect(model.addChange(null)).toBe(false);
    expect(model.removeAllChanges()).toBe(false);

    const changeAttribute = new loc.SedChangeAttribute(
      "component",
      "variable",
      "newValue",
    );

    expect(model.addChange(changeAttribute)).toBe(true);

    expect(model.hasChanges).toBe(true);
    expect(model.changeCount).toBe(1);
    expect(model.changes.size()).toBe(1);
    expect(model.changes.get(0)).toStrictEqual(changeAttribute);
    expect(model.change(0)).toStrictEqual(changeAttribute);
    expect(model.change(1)).toBeNull();

    expect(model.addChange(changeAttribute)).toBe(false);
    expect(model.removeChange(changeAttribute)).toBe(true);

    expect(model.addChange(changeAttribute)).toBe(true);
    expect(model.removeAllChanges()).toBe(true);

    expect(model.hasChanges).toBe(false);
    expect(model.changeCount).toBe(0);
    expect(model.changes.size()).toBe(0);

    expect(model.removeChange(null)).toBe(false);

    model.delete();
    file.delete();
    document.delete();
  });

  test("Changes", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(sedChangesContentsPtr, utils.SED_CHANGES_CONTENTS.length);

    let document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(false);

    file.setContents(
      invalidSedChangesContentsPtr,
      utils.INVALID_SED_CHANGES_CONTENTS.length,
    );

    document = new loc.SedDocument(file);

    expectIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "The component and variable names could not be retrieved for the change of type 'changeAttribute' and of target 'invalidTarget'.",
      ],
      [
        loc.Issue.Type.ERROR,
        "The new value 'invalidNewValue' for the change of type 'changeAttribute' is not a valid double value.",
      ],
      [
        loc.Issue.Type.ERROR,
        "The component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name=''.",
      ],
      [
        loc.Issue.Type.ERROR,
        "The component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName'.",
      ],
      [
        loc.Issue.Type.ERROR,
        "The component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name=''.",
      ],
      [
        loc.Issue.Type.ERROR,
        "The component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName'.",
      ],
      [
        loc.Issue.Type.ERROR,
        "The component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName']Invalid'.",
      ],
    ]);

    file.setContents(
      unsupportedSedChangesContentsPtr,
      utils.UNSUPPORTED_SED_CHANGES_CONTENTS.length,
    );

    document = new loc.SedDocument(file);

    expectIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "Only changes of type 'changeAttribute' are currently supported. The change of type 'addXML' has been ignored.",
      ],
      [
        loc.Issue.Type.WARNING,
        "Only changes of type 'changeAttribute' are currently supported. The change of type 'changeXML' has been ignored.",
      ],
      [
        loc.Issue.Type.WARNING,
        "Only changes of type 'changeAttribute' are currently supported. The change of type 'removeXML' has been ignored.",
      ],
      [
        loc.Issue.Type.WARNING,
        "Only changes of type 'changeAttribute' are currently supported. The change of type 'computeChange' has been ignored.",
      ],
    ]);
  });

  test("Simulations", () => {
    const document = new loc.SedDocument();

    expect(document.hasSimulations).toBe(false);
    expect(document.simulationCount).toBe(0);
    expect(document.simulations.size()).toBe(0);
    expect(document.addSimulation(null)).toBe(false);

    const uniform_time_course = new loc.SedUniformTimeCourse(document);
    const one_step = new loc.SedOneStep(document);
    const steady_state = new loc.SedSteadyState(document);
    const analysis = new loc.SedAnalysis(document);

    expect(document.addSimulation(uniform_time_course)).toBe(true);
    expect(document.addSimulation(one_step)).toBe(true);
    expect(document.addSimulation(steady_state)).toBe(true);
    expect(document.addSimulation(analysis)).toBe(true);

    expect(document.hasSimulations).toBe(true);
    expect(document.simulationCount).toBe(4);
    expect(document.simulations.size()).toBe(4);
    expect(document.simulations.get(0)).toStrictEqual(uniform_time_course);
    expect(document.simulations.get(1)).toStrictEqual(one_step);
    expect(document.simulations.get(2)).toStrictEqual(steady_state);
    expect(document.simulations.get(3)).toStrictEqual(analysis);
    expect(document.simulation(0)).toStrictEqual(uniform_time_course);
    expect(document.simulation(1)).toStrictEqual(one_step);
    expect(document.simulation(2)).toStrictEqual(steady_state);
    expect(document.simulation(3)).toStrictEqual(analysis);
    expect(document.simulation(4)).toBeNull();

    expect(document.addSimulation(uniform_time_course)).toBe(false);
    expect(document.removeSimulation(uniform_time_course)).toBe(true);

    expect(document.addSimulation(one_step)).toBe(false);
    expect(document.removeSimulation(one_step)).toBe(true);

    expect(document.addSimulation(steady_state)).toBe(false);
    expect(document.removeSimulation(steady_state)).toBe(true);

    expect(document.addSimulation(analysis)).toBe(false);
    expect(document.removeSimulation(analysis)).toBe(true);

    expect(document.hasSimulations).toBe(false);
    expect(document.simulationCount).toBe(0);
    expect(document.simulations.size()).toBe(0);

    expect(document.removeSimulation(null)).toBe(false);
    expect(document.removeAllSimulations()).toBe(false);

    expect(document.addSimulation(uniform_time_course)).toBe(true);
    expect(document.removeAllSimulations()).toBe(true);

    steady_state.delete();
    uniform_time_course.delete();
    document.delete();
  });

  test("Tasks", () => {
    const document = new loc.SedDocument();

    expect(document.hasTasks).toBe(false);
    expect(document.taskCount).toBe(0);
    expect(document.tasks.size()).toBe(0);
    expect(document.addTask(null)).toBe(false);

    const file = new loc.File(utils.SEDML_FILE);
    const model = new loc.SedModel(document, file);
    const simulation = new loc.SedUniformTimeCourse(document);
    const task = new loc.SedTask(document, model, simulation);

    expect(task.model).not.toBe(null);
    expect(task.simulation).not.toBe(null);

    expect(document.addTask(task)).toBe(true);

    expect(document.hasTasks).toBe(true);
    expect(document.taskCount).toBe(1);
    expect(document.tasks.size()).toBe(1);
    expect(document.tasks.get(0)).toStrictEqual(task);
    expect(document.task(0)).toStrictEqual(task);
    expect(document.task(1)).toBeNull();

    expect(document.serialise()).toBe(sedTaskExpectedSerialisation(true));

    task.model = null;
    task.simulation = null;

    expect(task.model).toBe(null);
    expect(task.simulation).toBe(null);

    expect(document.serialise()).toBe(sedTaskExpectedSerialisation(false));

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "Task 'task1' requires a model."],
      [loc.Issue.Type.ERROR, "Task 'task1' requires a simulation."],
    ]);

    expect(document.addTask(task)).toBe(false);
    expect(document.removeTask(task)).toBe(true);

    expect(document.hasTasks).toBe(false);
    expect(document.taskCount).toBe(0);
    expect(document.tasks.size()).toBe(0);

    expect(document.removeTask(null)).toBe(false);
    expect(document.removeAllTasks()).toBe(false);

    expect(document.addTask(task)).toBe(true);
    expect(document.removeAllTasks()).toBe(true);

    task.delete();
    simulation.delete();
    model.delete();
    file.delete();
    document.delete();
  });

  test("ODE solver", () => {
    const document = new loc.SedDocument();
    const simulation = new loc.SedUniformTimeCourse(document);

    expect(simulation.odeSolver).toBe(null);

    const solver = new loc.SolverCvode();

    simulation.odeSolver = solver;

    expect(simulation.odeSolver).toStrictEqual(solver);

    simulation.odeSolver = null;

    expect(simulation.odeSolver).toBe(null);

    solver.delete();
    simulation.delete();
    document.delete();
  });

  test("NLA solver", () => {
    const document = new loc.SedDocument();
    const simulation = new loc.SedUniformTimeCourse(document);

    expect(simulation.nlaSolver).toBe(null);

    const solver = new loc.SolverKinsol();

    simulation.nlaSolver = solver;

    expect(simulation.nlaSolver).toStrictEqual(solver);

    simulation.nlaSolver = null;

    expect(simulation.nlaSolver).toBe(null);

    solver.delete();
    simulation.delete();
    document.delete();
  });

  test("SedOneStep", () => {
    const file = new loc.File(utils.CELLML_FILE);
    const document = new loc.SedDocument(file);
    const simulation = new loc.SedOneStep(document);

    expect(simulation.step).toBe(1.0);

    simulation.step = 1.23;

    expect(simulation.step).toBe(1.23);

    simulation.delete();
    document.delete();
    file.delete();
  });

  test("SedUniformTimeCourse", () => {
    const file = new loc.File(utils.CELLML_FILE);
    const document = new loc.SedDocument(file);
    const simulation = new loc.SedUniformTimeCourse(document);

    expect(simulation.initialTime).toBe(0.0);
    expect(simulation.outputStartTime).toBe(0.0);
    expect(simulation.outputEndTime).toBe(1000.0);
    expect(simulation.numberOfSteps).toBe(1000);

    simulation.initialTime = 1.23;
    simulation.outputStartTime = 4.56;
    simulation.outputEndTime = 7.89;
    simulation.numberOfSteps = 10;

    expect(simulation.initialTime).toBe(1.23);
    expect(simulation.outputStartTime).toBe(4.56);
    expect(simulation.outputEndTime).toBe(7.89);
    expect(simulation.numberOfSteps).toBe(10);

    simulation.delete();
    document.delete();
    file.delete();
  });

  test("SedInstanceAndSedInstanceTaskDifferentialModel", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const solver = document.simulations.get(0).odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.upperHalfBandwidth = -1;

    const instance = document.instantiate();
    const instanceTask = instance.tasks.get(0);

    expect(instance.hasTasks).toBe(true);
    expect(instance.taskCount).toBe(1);
    expect(instance.task(0)).toStrictEqual(instanceTask);
    expect(instance.task(1)).toStrictEqual(null);

    expect(instanceTask.voi.size()).toBe(0);
    expect(instanceTask.voiAsArray).toStrictEqual([]);
    expect(instanceTask.voiName).toBe("environment/time");
    expect(instanceTask.voiUnit).toBe("millisecond");

    expect(instanceTask.stateCount).toBe(4);
    expect(instanceTask.state(0).size()).toBe(0);
    expect(instanceTask.stateAsArray(0)).toStrictEqual([]);
    expect(instanceTask.state(4).size()).toBe(0);
    expect(instanceTask.stateAsArray(4)).toStrictEqual([]);
    expect(instanceTask.stateName(0)).toBe("membrane/V");
    expect(instanceTask.stateName(4)).toBe("");
    expect(instanceTask.stateUnit(0)).toBe("millivolt");
    expect(instanceTask.stateUnit(4)).toBe("");

    expect(instanceTask.rateCount).toBe(4);
    expect(instanceTask.rate(0).size()).toBe(0);
    expect(instanceTask.rateAsArray(0)).toStrictEqual([]);
    expect(instanceTask.rate(4).size()).toBe(0);
    expect(instanceTask.rateAsArray(4)).toStrictEqual([]);
    expect(instanceTask.rateName(0)).toBe("membrane/V'");
    expect(instanceTask.rateName(4)).toBe("");
    expect(instanceTask.rateUnit(0)).toBe("millivolt/millisecond");
    expect(instanceTask.rateUnit(4)).toBe("");

    expect(instanceTask.constantCount).toBe(5);
    expect(instanceTask.constant(0).size()).toBe(0);
    expect(instanceTask.constantAsArray(0)).toStrictEqual([]);
    expect(instanceTask.constant(5).size()).toBe(0);
    expect(instanceTask.constantAsArray(5)).toStrictEqual([]);
    expect(instanceTask.constantName(0)).toBe("membrane/Cm");
    expect(instanceTask.constantName(5)).toBe("");
    expect(instanceTask.constantUnit(0)).toBe("microF_per_cm2");
    expect(instanceTask.constantUnit(5)).toBe("");

    expect(instanceTask.computedConstantCount).toBe(3);
    expect(instanceTask.computedConstant(0).size()).toBe(0);
    expect(instanceTask.computedConstantAsArray(0)).toStrictEqual([]);
    expect(instanceTask.computedConstant(3).size()).toBe(0);
    expect(instanceTask.computedConstantAsArray(3)).toStrictEqual([]);
    expect(instanceTask.computedConstantName(0)).toBe("leakage_current/E_L");
    expect(instanceTask.computedConstantName(3)).toBe("");
    expect(instanceTask.computedConstantUnit(0)).toBe("millivolt");
    expect(instanceTask.computedConstantUnit(3)).toBe("");

    expect(instanceTask.algebraicCount).toBe(10);
    expect(instanceTask.algebraic(0).size()).toBe(0);
    expect(instanceTask.algebraicAsArray(0)).toStrictEqual([]);
    expect(instanceTask.algebraic(10).size()).toBe(0);
    expect(instanceTask.algebraicAsArray(10)).toStrictEqual([]);
    expect(instanceTask.algebraicName(0)).toBe("membrane/i_Stim");
    expect(instanceTask.algebraicName(10)).toBe("");
    expect(instanceTask.algebraicUnit(0)).toBe("microA_per_cm2");
    expect(instanceTask.algebraicUnit(10)).toBe("");

    instance.run();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  /*---GRY--- TO BE UNCOMMENTED ONCE WE CAN INTERPRET A MODEL WITH ONE/SEVERAL NLA SYSTEM/S.
  test("SedInstanceAndSedInstanceTaskNonDifferentialModel", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      someSolverNla1ContentsPtr,
      utils.SOME_SOLVER_NLA1_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);

    const instance = document.instantiate();
    const instanceTask = instance.tasks.get(0);

    expect(instanceTask.voi.size()).toBe(0);
    expect(instanceTask.voiAsArray).toStrictEqual([]);
    expect(instanceTask.voiName).toBe("");
    expect(instanceTask.voiUnit).toBe("");

    expect(instanceTask.stateCount).toBe(0);
    expect(instanceTask.state(0).size()).toBe(0);
    expect(instanceTask.stateAsArray(0)).toStrictEqual([]);
    expect(instanceTask.stateName(0)).toBe("");
    expect(instanceTask.stateUnit(0)).toBe("");

    expect(instanceTask.rateCount).toBe(0);
    expect(instanceTask.rate(0).size()).toBe(0);
    expect(instanceTask.rateAsArray(0)).toStrictEqual([]);
    expect(instanceTask.rateName(0)).toBe("");
    expect(instanceTask.rateUnit(0)).toBe("");

    instance.delete();
    document.delete();
    file.delete();
  });
  */

  test("SedDocument", () => {
    let file = new loc.File(utils.HTTP_REMOTE_CELLML_FILE);
    let document = new loc.SedDocument(file);

    document.delete();
    file.delete();

    file = new loc.File(utils.HTTP_REMOTE_SEDML_FILE);
    document = new loc.SedDocument(file);

    document.delete();
    file.delete();

    file = new loc.File(utils.HTTP_REMOTE_COMBINE_ARCHIVE);
    document = new loc.SedDocument(file);

    document.delete();
    file.delete();
  });

  test("Solver", () => {
    // Get the duplicate() method of different solvers to be covered.

    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    let solver = new loc.SolverForwardEuler();

    simulation.odeSolver = solver;

    let instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues).toBe(false);

    instance.delete();
    solver.delete();

    solver = new loc.SolverFourthOrderRungeKutta();

    simulation.odeSolver = solver;

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues).toBe(false);

    instance.delete();
    solver.delete();

    solver = new loc.SolverHeun();

    simulation.odeSolver = solver;

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues).toBe(false);

    instance.delete();
    solver.delete();

    solver = new loc.SolverSecondOrderRungeKutta();

    simulation.odeSolver = solver;

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues).toBe(false);

    instance.delete();
    solver.delete();
    document.delete();
    file.delete();
  });
});
