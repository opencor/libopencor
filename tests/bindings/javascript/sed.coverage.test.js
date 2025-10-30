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

import assert from 'node:assert';
import test from 'node:test';

import libOpenCOR from './libopencor.js';
import * as utils from './utils.js';
import { assertIssues, assertValues } from './utils.js';

const loc = await libOpenCOR();

test.describe('Sed coverage tests', () => {
  let sedChangesContentsPtr;
  let invalidSedChangesContentsPtr;
  let unsupportedSedChangesContentsPtr;
  let solverOdeContentsPtr;
  let solverNla1ContentsPtr;
  let mathContentsPtr;

  test.before(() => {
    sedChangesContentsPtr = utils.allocateMemory(loc, utils.SED_CHANGES_CONTENTS);
    invalidSedChangesContentsPtr = utils.allocateMemory(loc, utils.INVALID_SED_CHANGES_CONTENTS);
    unsupportedSedChangesContentsPtr = utils.allocateMemory(loc, utils.UNSUPPORTED_SED_CHANGES_CONTENTS);
    solverOdeContentsPtr = utils.allocateMemory(loc, utils.SOLVER_ODE_CONTENTS);
    solverNla1ContentsPtr = utils.allocateMemory(loc, utils.SOLVER_NLA1_CONTENTS);
    mathContentsPtr = utils.allocateMemory(loc, utils.MATH_CONTENTS);
  });

  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test.after(() => {
    utils.freeMemory(loc, sedChangesContentsPtr);
    utils.freeMemory(loc, invalidSedChangesContentsPtr);
    utils.freeMemory(loc, unsupportedSedChangesContentsPtr);
    utils.freeMemory(loc, solverOdeContentsPtr);
    utils.freeMemory(loc, solverNla1ContentsPtr);
    utils.freeMemory(loc, mathContentsPtr);
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

  test('Initialise', () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
`;
    const document = new loc.SedDocument();

    assert.strictEqual(document.serialise(), expectedSerialisation);
  });

  test('Models', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasModels, false);
    assert.strictEqual(document.modelCount, 0);
    assert.strictEqual(document.models.size(), 0);
    assert.strictEqual(document.addModel(null), false);

    const file = new loc.File(utils.SEDML_FILE);
    const model = new loc.SedModel(document, file);

    assert.deepStrictEqual(model.file, file);

    assert.strictEqual(document.addModel(model), true);

    assert.strictEqual(document.hasModels, true);
    assert.strictEqual(document.modelCount, 1);
    assert.strictEqual(document.models.size(), 1);
    assert.deepStrictEqual(document.models.get(0), model);
    assert.deepStrictEqual(document.model(0), model);
    assert.deepStrictEqual(document.model(1), null);

    assert.strictEqual(document.addModel(model), false);
    assert.strictEqual(document.removeModel(model), true);

    assert.strictEqual(document.hasModels, false);
    assert.strictEqual(document.modelCount, 0);
    assert.strictEqual(document.models.size(), 0);

    assert.strictEqual(document.removeModel(null), false);
    assert.strictEqual(document.removeAllModels(), false);

    assert.strictEqual(document.addModel(model), true);
    assert.strictEqual(document.removeAllModels(), true);

    assert.strictEqual(model.hasChanges, false);
    assert.strictEqual(model.changeCount, 0);
    assert.strictEqual(model.changes.size(), 0);
    assert.strictEqual(model.addChange(null), false);
    assert.strictEqual(model.removeAllChanges(), false);

    const changeAttribute = new loc.SedChangeAttribute('component', 'variable', 'newValue');

    assert.strictEqual(model.addChange(changeAttribute), true);

    assert.strictEqual(model.hasChanges, true);
    assert.strictEqual(model.changeCount, 1);
    assert.strictEqual(model.changes.size(), 1);
    assert.deepStrictEqual(model.changes.get(0), changeAttribute);
    assert.deepStrictEqual(model.change(0), changeAttribute);
    assert.deepStrictEqual(model.change(1), null);

    assert.strictEqual(model.addChange(changeAttribute), false);
    assert.strictEqual(model.removeChange(changeAttribute), true);

    assert.strictEqual(model.addChange(changeAttribute), true);
    assert.strictEqual(model.removeAllChanges(), true);

    assert.strictEqual(model.hasChanges, false);
    assert.strictEqual(model.changeCount, 0);
    assert.strictEqual(model.changes.size(), 0);

    assert.strictEqual(model.removeChange(null), false);
  });

  test('Changes', () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(sedChangesContentsPtr, utils.SED_CHANGES_CONTENTS.length);

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);

    file.setContents(invalidSedChangesContentsPtr, utils.INVALID_SED_CHANGES_CONTENTS.length);

    document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target 'invalidTarget'."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the new value 'invalidNewValue' for the change of type 'changeAttribute' is not a valid double value."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name=''."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName'."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name=''."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName'."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName']Invalid'."
      ]
    ]);

    file.setContents(unsupportedSedChangesContentsPtr, utils.UNSUPPORTED_SED_CHANGES_CONTENTS.length);

    document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'addXML' has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'changeXML' has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'removeXML' has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'computeChange' has been ignored."
      ]
    ]);
  });

  test('Simulations', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasSimulations, false);
    assert.strictEqual(document.simulationCount, 0);
    assert.strictEqual(document.simulations.size(), 0);
    assert.strictEqual(document.addSimulation(null), false);

    const uniform_time_course = new loc.SedUniformTimeCourse(document);
    const one_step = new loc.SedOneStep(document);
    const steady_state = new loc.SedSteadyState(document);
    const analysis = new loc.SedAnalysis(document);

    assert.strictEqual(document.addSimulation(uniform_time_course), true);
    assert.strictEqual(document.addSimulation(one_step), true);
    assert.strictEqual(document.addSimulation(steady_state), true);
    assert.strictEqual(document.addSimulation(analysis), true);

    assert.strictEqual(document.hasSimulations, true);
    assert.strictEqual(document.simulationCount, 4);
    assert.strictEqual(document.simulations.size(), 4);
    assert.deepStrictEqual(document.simulations.get(0), uniform_time_course);
    assert.deepStrictEqual(document.simulations.get(1), one_step);
    assert.deepStrictEqual(document.simulations.get(2), steady_state);
    assert.deepStrictEqual(document.simulations.get(3), analysis);
    assert.deepStrictEqual(document.simulation(0), uniform_time_course);
    assert.deepStrictEqual(document.simulation(1), one_step);
    assert.deepStrictEqual(document.simulation(2), steady_state);
    assert.deepStrictEqual(document.simulation(3), analysis);
    assert.deepStrictEqual(document.simulation(4), null);

    assert.strictEqual(document.addSimulation(uniform_time_course), false);
    assert.strictEqual(document.removeSimulation(uniform_time_course), true);

    assert.strictEqual(document.addSimulation(one_step), false);
    assert.strictEqual(document.removeSimulation(one_step), true);

    assert.strictEqual(document.addSimulation(steady_state), false);
    assert.strictEqual(document.removeSimulation(steady_state), true);

    assert.strictEqual(document.addSimulation(analysis), false);
    assert.strictEqual(document.removeSimulation(analysis), true);

    assert.strictEqual(document.hasSimulations, false);
    assert.strictEqual(document.simulationCount, 0);
    assert.strictEqual(document.simulations.size(), 0);

    assert.strictEqual(document.removeSimulation(null), false);
    assert.strictEqual(document.removeAllSimulations(), false);

    assert.strictEqual(document.addSimulation(uniform_time_course), true);
    assert.strictEqual(document.removeAllSimulations(), true);
  });

  test('Tasks', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasTasks, false);
    assert.strictEqual(document.taskCount, 0);
    assert.strictEqual(document.tasks.size(), 0);
    assert.strictEqual(document.addTask(null), false);

    const file = new loc.File(utils.SEDML_FILE);
    const model = new loc.SedModel(document, file);
    const simulation = new loc.SedUniformTimeCourse(document);
    const task = new loc.SedTask(document, model, simulation);

    assert.deepStrictEqual(task.model, model);
    assert.deepStrictEqual(task.simulation, simulation);

    assert.strictEqual(document.addTask(task), true);

    assert.strictEqual(document.hasTasks, true);
    assert.strictEqual(document.taskCount, 1);
    assert.strictEqual(document.tasks.size(), 1);
    assert.deepStrictEqual(document.tasks.get(0), task);
    assert.deepStrictEqual(document.task(0), task);
    assert.deepStrictEqual(document.task(1), null);

    assert.strictEqual(document.serialise(), sedTaskExpectedSerialisation(true));

    task.model = null;
    task.simulation = null;

    assert.strictEqual(task.model, null);
    assert.strictEqual(task.simulation, null);

    assert.strictEqual(document.serialise(), sedTaskExpectedSerialisation(false));

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "Task: task 'task1' requires a model."],
      [loc.Issue.Type.ERROR, "Task: task 'task1' requires a simulation."]
    ]);

    assert.strictEqual(document.addTask(task), false);
    assert.strictEqual(document.removeTask(task), true);

    assert.strictEqual(document.hasTasks, false);
    assert.strictEqual(document.taskCount, 0);
    assert.strictEqual(document.tasks.size(), 0);

    assert.strictEqual(document.removeTask(null), false);
    assert.strictEqual(document.removeAllTasks(), false);

    assert.strictEqual(document.addTask(task), true);
    assert.strictEqual(document.removeAllTasks(), true);
  });

  test('ODE solver', () => {
    const document = new loc.SedDocument();
    const simulation = new loc.SedUniformTimeCourse(document);

    assert.strictEqual(simulation.odeSolver, null);

    const solver = new loc.SolverCvode();

    simulation.odeSolver = solver;

    assert.deepStrictEqual(simulation.odeSolver, solver);

    simulation.odeSolver = null;

    assert.strictEqual(simulation.odeSolver, null);
  });

  test('NLA solver', () => {
    const document = new loc.SedDocument();
    const simulation = new loc.SedUniformTimeCourse(document);

    assert.strictEqual(simulation.nlaSolver, null);

    const solver = new loc.SolverKinsol();

    simulation.nlaSolver = solver;

    assert.deepStrictEqual(simulation.nlaSolver, solver);

    simulation.nlaSolver = null;

    assert.strictEqual(simulation.nlaSolver, null);
  });

  test('SedOneStep', () => {
    const file = new loc.File(utils.CELLML_FILE);
    const document = new loc.SedDocument(file);
    const simulation = new loc.SedOneStep(document);

    assert.strictEqual(simulation.step, 1.0);

    simulation.step = 1.23;

    assert.strictEqual(simulation.step, 1.23);
  });

  test('SedUniformTimeCourse', () => {
    const file = new loc.File(utils.CELLML_FILE);
    const document = new loc.SedDocument(file);
    const simulation = new loc.SedUniformTimeCourse(document);

    assert.strictEqual(simulation.initialTime, 0.0);
    assert.strictEqual(simulation.outputStartTime, 0.0);
    assert.strictEqual(simulation.outputEndTime, 1000.0);
    assert.strictEqual(simulation.numberOfSteps, 1000);

    simulation.initialTime = 1.23;
    simulation.outputStartTime = 4.56;
    simulation.outputEndTime = 7.89;
    simulation.numberOfSteps = 10;

    assert.strictEqual(simulation.initialTime, 1.23);
    assert.strictEqual(simulation.outputStartTime, 4.56);
    assert.strictEqual(simulation.outputEndTime, 7.89);
    assert.strictEqual(simulation.numberOfSteps, 10);
  });

  test('SedInstanceAndSedInstanceTaskDifferentialModel', () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const solver = document.simulations.get(0).odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.upperHalfBandwidth = -1;

    const instance = document.instantiate();
    const instanceTask = instance.tasks.get(0);

    assert.strictEqual(instance.hasTasks, true);
    assert.strictEqual(instance.taskCount, 1);
    assert.deepStrictEqual(instance.task(0), instanceTask);
    assert.strictEqual(instance.task(1), null);

    assert.strictEqual(instanceTask.voi.size(), 0);
    assert.deepStrictEqual(instanceTask.voiAsArray, []);
    assert.strictEqual(instanceTask.voiName, 'environment/time');
    assert.strictEqual(instanceTask.voiUnit, 'millisecond');

    assert.strictEqual(instanceTask.stateCount, 4);
    assert.strictEqual(instanceTask.state(0).size(), 0);
    assert.deepStrictEqual(instanceTask.stateAsArray(0), []);
    assert.strictEqual(instanceTask.state(4).size(), 0);
    assert.deepStrictEqual(instanceTask.stateAsArray(4), []);
    assert.strictEqual(instanceTask.stateName(0), 'membrane/V');
    assert.strictEqual(instanceTask.stateName(4), '');
    assert.strictEqual(instanceTask.stateUnit(0), 'millivolt');
    assert.strictEqual(instanceTask.stateUnit(4), '');

    assert.strictEqual(instanceTask.rateCount, 4);
    assert.strictEqual(instanceTask.rate(0).size(), 0);
    assert.deepStrictEqual(instanceTask.rateAsArray(0), []);
    assert.strictEqual(instanceTask.rate(4).size(), 0);
    assert.deepStrictEqual(instanceTask.rateAsArray(4), []);
    assert.strictEqual(instanceTask.rateName(0), "membrane/V'");
    assert.strictEqual(instanceTask.rateName(4), '');
    assert.strictEqual(instanceTask.rateUnit(0), 'millivolt/millisecond');
    assert.strictEqual(instanceTask.rateUnit(4), '');

    assert.strictEqual(instanceTask.constantCount, 5);
    assert.strictEqual(instanceTask.constant(0).size(), 0);
    assert.deepStrictEqual(instanceTask.constantAsArray(0), []);
    assert.strictEqual(instanceTask.constant(5).size(), 0);
    assert.deepStrictEqual(instanceTask.constantAsArray(5), []);
    assert.strictEqual(instanceTask.constantName(0), 'membrane/Cm');
    assert.strictEqual(instanceTask.constantName(5), '');
    assert.strictEqual(instanceTask.constantUnit(0), 'microF_per_cm2');
    assert.strictEqual(instanceTask.constantUnit(5), '');

    assert.strictEqual(instanceTask.computedConstantCount, 3);
    assert.strictEqual(instanceTask.computedConstant(0).size(), 0);
    assert.deepStrictEqual(instanceTask.computedConstantAsArray(0), []);
    assert.strictEqual(instanceTask.computedConstant(3).size(), 0);
    assert.deepStrictEqual(instanceTask.computedConstantAsArray(3), []);
    assert.strictEqual(instanceTask.computedConstantName(0), 'leakage_current/E_L');
    assert.strictEqual(instanceTask.computedConstantName(3), '');
    assert.strictEqual(instanceTask.computedConstantUnit(0), 'millivolt');
    assert.strictEqual(instanceTask.computedConstantUnit(3), '');

    assert.strictEqual(instanceTask.algebraicCount, 10);
    assert.strictEqual(instanceTask.algebraic(0).size(), 0);
    assert.deepStrictEqual(instanceTask.algebraicAsArray(0), []);
    assert.strictEqual(instanceTask.algebraic(10).size(), 0);
    assert.deepStrictEqual(instanceTask.algebraicAsArray(10), []);
    assert.strictEqual(instanceTask.algebraicName(0), 'membrane/i_Stim');
    assert.strictEqual(instanceTask.algebraicName(10), '');
    assert.strictEqual(instanceTask.algebraicUnit(0), 'microA_per_cm2');
    assert.strictEqual(instanceTask.algebraicUnit(10), '');

    instance.run();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | CVODE: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.'
      ]
    ]);
  });

  test('SedInstanceAndSedInstanceTaskNonDifferentialModel', () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverNla1ContentsPtr, utils.SOLVER_NLA1_CONTENTS.length);

    const document = new loc.SedDocument(file);

    const instance = document.instantiate();
    const instanceTask = instance.tasks.get(0);

    assert.strictEqual(instanceTask.voi.size(), 0);
    assert.deepStrictEqual(instanceTask.voiAsArray, []);
    assert.strictEqual(instanceTask.voiName, '');
    assert.strictEqual(instanceTask.voiUnit, '');

    assert.strictEqual(instanceTask.stateCount, 0);
    assert.strictEqual(instanceTask.state(0).size(), 0);
    assert.deepStrictEqual(instanceTask.stateAsArray(0), []);
    assert.strictEqual(instanceTask.stateName(0), '');
    assert.strictEqual(instanceTask.stateUnit(0), '');

    assert.strictEqual(instanceTask.rateCount, 0);
    assert.strictEqual(instanceTask.rate(0).size(), 0);
    assert.deepStrictEqual(instanceTask.rateAsArray(0), []);
    assert.strictEqual(instanceTask.rateName(0), '');
    assert.strictEqual(instanceTask.rateUnit(0), '');
  });

  test('SedDocument', () => {
    let file = new loc.File(utils.HTTP_REMOTE_CELLML_FILE);

    new loc.SedDocument(file);

    file = new loc.File(utils.HTTP_REMOTE_SEDML_FILE);

    new loc.SedDocument(file);

    file = new loc.File(utils.HTTP_REMOTE_COMBINE_ARCHIVE);

    new loc.SedDocument(file);
  });

  test('Solver', () => {
    // Get the duplicate() method of different solvers to be covered.

    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    let solver = new loc.SolverForwardEuler();

    simulation.odeSolver = solver;

    let instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    solver = new loc.SolverFourthOrderRungeKutta();

    simulation.odeSolver = solver;

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    solver = new loc.SolverHeun();

    simulation.odeSolver = solver;

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    solver = new loc.SolverSecondOrderRungeKutta();

    simulation.odeSolver = solver;

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('Math', () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(mathContentsPtr, utils.MATH_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();
    const instanceTask = instance.tasks.get(0);

    assert.strictEqual(instanceTask.constantCount, 0);
    assert.strictEqual(instanceTask.computedConstantCount, 37);
    assert.strictEqual(instanceTask.algebraicCount, 0);

    instance.run();

    assertValues(
      instanceTask,
      0,
      [],
      [],
      [],
      [],
      [],
      [],
      [
        243.0,
        3.0,
        7.0,
        20.085536923187668,
        1.0986122886681098,
        0.47712125471966244,
        4.0,
        3.0,
        3.0,
        5.0,
        3.0,
        0.1411200080598672,
        -0.9899924966004454,
        -0.1425465430742778,
        -1.0101086659079939,
        7.086167395737187,
        -7.015252551434534,
        10.017874927409903,
        10.067661995777765,
        0.9950547536867305,
        0.0993279274194332,
        0.09982156966882273,
        1.0049698233136892,
        0.3046926540153975,
        1.2661036727794992,
        1.2490457723982544,
        1.2309594173407747,
        0.3398369094541219,
        0.3217505543966422,
        1.8184464592320668,
        1.762747174039086,
        0.30951960420311175,
        1.8738202425274144,
        0.32745015023725843,
        0.34657359027997264,
        Infinity,
        NaN
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
      [],
      []
    );
  });
});
