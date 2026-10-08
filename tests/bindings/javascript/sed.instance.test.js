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
import { assertIssues } from './utils.js';

const loc = await libOpenCOR();

const sleep = (ms) =>
  new Promise((resolve) => {
    setTimeout(resolve, ms);
  });

test.describe('Sed instance tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('No file', () => {
    const document = new loc.SedDocument();
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'The simulation experiment description does not contain any tasks to run.']
    ]);
    assert.strictEqual(instance.progress, 0.0);

    // Make sure that the issues are still present after "running" the instance.

    assert.strictEqual(instance.run(), 0.0);
    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'The simulation experiment description does not contain any tasks to run.']
    ]);
  });

  test('Invalid CellML file', () => {
    const file = new loc.File(utils.resourcePath('error.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task | Model: the CellML file is invalid.'],
      [
        loc.Issue.Type.ERROR,
        "Task | Model | CellML | Analyser: equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS)."
      ]
    ]);
  });

  test('CellML file with units prefix out of range', () => {
    // Note: libCellML handles such a units prefix by catching the std::out_of_range exception thrown by std::stoi(), so
    //       this checks that exceptions can be caught everywhere in libOpenCOR, including in our third-party libraries.

    const file = new loc.File(utils.resourcePath('api/sed/units_prefix_out_of_range.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task | Model: the CellML file is invalid.'],
      [
        loc.Issue.Type.ERROR,
        "Task | Model | CellML | Analyser: prefix '92233720368547758077876856757465433' of a unit referencing 'second' in units 'my_units' is out of the integer range."
      ]
    ]);
  });

  test('Overconstrained CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/overconstrained.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task | Model: the CellML file is overconstrained.'],
      [
        loc.Issue.Type.ERROR,
        "Task | Model | CellML | Analyser: variable 'x' in component 'my_component' is overconstrained."
      ]
    ]);
  });

  test('Underconstrained CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/underconstrained.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task | Model: the CellML file is underconstrained.'],
      [
        loc.Issue.Type.ERROR,
        "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown."
      ]
    ]);
  });

  test('Unsuitably constrained CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/unsuitably_constrained.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task | Model: the CellML file is unsuitably constrained.'],
      [
        loc.Issue.Type.ERROR,
        "Task | Model | CellML | Analyser: variable 'y' in component 'my_component' is overconstrained."
      ],
      [
        loc.Issue.Type.ERROR,
        "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown."
      ]
    ]);
  });

  test('Algebraic model', () => {
    const file = new loc.File(utils.resourcePath('api/sed/algebraic.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('Asynchronous run without active run', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.strictEqual(instance.waitForRun(), 0.0);
  });

  test('Asynchronous run lifecycle', async () => {
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.ok(instance.waitForRun() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Asynchronous run can be restarted', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);
    assert.ok(instance.waitForRun() > 0.0);
    assert.strictEqual(instance.hasIssues, false);

    assert.strictEqual(instance.startRun(), true);
    assert.ok(instance.waitForRun() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Progress before any run', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.progress, 0.0);
    assert.strictEqual(instance.tasks.get(0).progress, 0.0);
  });

  test('Progress of algebraic model', () => {
    const file = new loc.File(utils.resourcePath('api/sed/algebraic.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.progress, 0.0);

    instance.run();

    assert.strictEqual(instance.progress, 1.0);
    assert.strictEqual(instance.tasks.get(0).progress, 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Progress of ODE model', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.progress, 0.0);

    instance.run();

    assert.strictEqual(instance.progress, 1.0);
    assert.strictEqual(instance.tasks.get(0).progress, 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Stop run', async () => {
    const SIMULATION_PROPERTY = 1000000;
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = SIMULATION_PROPERTY;
    simulation.outputEndTime = SIMULATION_PROPERTY;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.stopRun();

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Stop run results have NaNs', async () => {
    const SIMULATION_PROPERTY = 1000000;
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = SIMULATION_PROPERTY;
    simulation.outputEndTime = SIMULATION_PROPERTY;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.stopRun();

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);

    const instanceTask = instance.tasks[0];
    const voi = instanceTask.voi;
    const state0 = instanceTask.state(0);

    assert.strictEqual(voi.length, state0.length);
    assert.strictEqual(voi.length, SIMULATION_PROPERTY + 1);

    assert.strictEqual(Number.isNaN(voi[0]), false);
    assert.strictEqual(Number.isNaN(state0[0]), false);

    let nanIndex = voi.length;

    for (let i = 1; i < voi.length; ++i) {
      if (Number.isNaN(state0[i])) {
        nanIndex = i;

        break;
      }
    }

    assert.ok(nanIndex < voi.length);
    assert.strictEqual(Number.isNaN(voi[nanIndex]), true);
    assert.ok(nanIndex < voi.length - 1);
  });

  test('Stop run when not running', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.stopRun();

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.strictEqual(instance.progress, 0.0);
  });

  test('Pause run and resume run', async () => {
    const SIMULATION_PROPERTY = 1000000;
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = SIMULATION_PROPERTY;
    simulation.outputEndTime = SIMULATION_PROPERTY;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.pauseRun();

    await sleep(50);

    instance.resumeRun();
    instance.stopRun();

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Pause run and resume run when not running', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.pauseRun();
    instance.resumeRun();

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.strictEqual(instance.progress, 0.0);
  });

  test('Pause run then stop run', async () => {
    const SIMULATION_PROPERTY = 1000000;
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = SIMULATION_PROPERTY;
    simulation.outputEndTime = SIMULATION_PROPERTY;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.pauseRun();

    await sleep(50);

    instance.stopRun();

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Pause run and resume run with natural completion', async () => {
    const moderateStepCount = 50000;
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = moderateStepCount;
    simulation.outputEndTime = moderateStepCount;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.pauseRun();

    await sleep(50);

    instance.resumeRun();

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.ok(instance.waitForRun() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Start run while already running', async () => {
    const SIMULATION_PROPERTY = 1000000;
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = SIMULATION_PROPERTY;
    simulation.outputEndTime = SIMULATION_PROPERTY;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.startRun(), false);

    instance.stopRun();

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Start run after previous run completed', async () => {
    const WAIT_ITERATIONS = 60000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < WAIT_ITERATIONS; ++i) {
      if (instance.status === loc.SedInstance.Status.IDLE) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assert.ok(instance.waitForRun() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('ODE model', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const cvode = simulation.odeSolver;

    cvode.maximumNumberOfSteps = 10;

    let instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task | CVODE: at t = 0.00140013827899996, mxstep steps taken before reaching tout.']
    ]);

    cvode.maximumNumberOfSteps = 500;

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('ODE model with no ODE solver', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    document.simulations[0].odeSolver = null;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."
      ]
    ]);
  });

  test('ODE model with non-uniform time course simulation', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument();
    const model = new loc.SedModel(document, file);
    const oneStep = new loc.SedOneStep(document);
    const steadyState = new loc.SedSteadyState(document);
    const task = new loc.SedTask(document, model, oneStep);

    oneStep.odeSolver = new loc.SolverCvode();
    steadyState.odeSolver = new loc.SolverCvode();

    document.addModel(model);
    document.addSimulation(oneStep);
    document.addSimulation(steadyState);
    document.addTask(task);

    let instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which (currently) requires a uniform time course simulation."
      ]
    ]);

    task.simulation = steadyState;

    instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation2' is to be used with model 'model1' which (currently) requires a uniform time course simulation."
      ]
    ]);
  });

  test('ODE model with invalid number of steps', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = 0;

    let instance = document.instantiate();

    const zeroStepsExpectedIssues = [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but 0 is provided."
      ]
    ];

    assertIssues(loc, instance, zeroStepsExpectedIssues);

    simulation.numberOfSteps = -100;

    instance = document.instantiate();

    const negativeStepsExpectedIssues = [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but -100 is provided."
      ]
    ];

    assertIssues(loc, instance, negativeStepsExpectedIssues);

    // Running the instance should not do anything, but it should still leave it idle.

    assert.strictEqual(instance.startRun(), true);
    assert.strictEqual(instance.waitForRun(), 0.0);
    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assertIssues(loc, instance, negativeStepsExpectedIssues);

    // Make sure that the number of steps is not reported as invalid anymore once it has been fixed.

    simulation.numberOfSteps = 1000;

    instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);
    assert.ok(instance.run() > 0.0);
    assert.strictEqual(instance.hasIssues, false);

    // Make sure that an invalid number of steps is reported when running an instance even if it was valid when the
    // instance was created.

    const instanceTask = instance.tasks[0];

    simulation.numberOfSteps = 0;

    assert.strictEqual(instance.run(), 0.0);
    assertIssues(loc, instance, zeroStepsExpectedIssues);
    assert.strictEqual(instanceTask.voi.length, 1001);

    simulation.numberOfSteps = -100;

    assert.strictEqual(instance.startRun(), true);
    assert.strictEqual(instance.waitForRun(), 0.0);
    assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
    assertIssues(loc, instance, negativeStepsExpectedIssues);
    assert.strictEqual(instanceTask.voi.length, 1001);

    // Make sure that the instance can be run again once the number of steps has been fixed.

    simulation.numberOfSteps = 500;

    assert.ok(instance.run() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
    assert.strictEqual(instanceTask.voi.length, 501);
  });

  test('ODE model with invalid times', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    // Initial time after the output start time.

    simulation.initialTime = 5.0;
    simulation.outputStartTime = 0.0;
    simulation.outputEndTime = 50.0;

    let instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 5, 0, and 50 are provided."
      ]
    ]);

    // Output start time equal to the output end time.

    simulation.initialTime = 0.0;
    simulation.outputStartTime = 5.0;
    simulation.outputEndTime = 5.0;

    instance = document.instantiate();

    const outputEndTimeExpectedIssues = [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 0, 5, and 5 are provided."
      ]
    ];

    assertIssues(loc, instance, outputEndTimeExpectedIssues);

    // Both invalid times and an invalid number of steps.

    simulation.numberOfSteps = 0;

    instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 0, 5, and 5 are provided."
      ],
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but 0 is provided."
      ]
    ]);

    // Non-finite times.

    simulation.outputEndTime = Number.NaN;
    simulation.numberOfSteps = 10;

    instance = document.instantiate();

    assert.strictEqual(instance.errorCount, 1);

    simulation.outputEndTime = Number.POSITIVE_INFINITY;

    instance = document.instantiate();

    assert.strictEqual(instance.errorCount, 1);

    simulation.initialTime = Number.NEGATIVE_INFINITY;
    simulation.outputStartTime = 0.0;
    simulation.outputEndTime = 50.0;

    instance = document.instantiate();

    assert.strictEqual(instance.errorCount, 1);

    simulation.initialTime = 0.0;
    simulation.outputStartTime = Number.NaN;

    instance = document.instantiate();

    assert.strictEqual(instance.errorCount, 1);

    // Valid times, but then made invalid after instantiation.

    simulation.outputStartTime = 0.0;
    simulation.outputEndTime = 50.0;
    simulation.numberOfSteps = 50;

    instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);
    assert.ok(instance.run() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
    assert.strictEqual(instance.progress, 1.0);

    simulation.outputStartTime = 5.0;
    simulation.outputEndTime = 5.0;

    assert.strictEqual(instance.run(), 0.0);
    assertIssues(loc, instance, outputEndTimeExpectedIssues);
    assert.strictEqual(instance.progress, 0.0);

    // Valid times again.

    simulation.outputStartTime = 0.0;
    simulation.outputEndTime = 50.0;

    assert.ok(instance.run() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
    assert.strictEqual(instance.progress, 1.0);
  });

  test('ODE model results reused when run again', () => {
    // Note: our results are returned as views on the WebAssembly heap, so make sure that they remain valid when running
    //       an instance again with the same number of steps.

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    const instanceTask = instance.tasks[0];
    const voi = instanceTask.voi;
    const state = instanceTask.state(0);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
    assert.strictEqual(voi.byteOffset, instanceTask.voi.byteOffset);
    assert.strictEqual(state.byteOffset, instanceTask.state(0).byteOffset);
    assert.strictEqual(voi[voi.length - 1], instanceTask.voi[voi.length - 1]);
    assert.strictEqual(state[state.length - 1], instanceTask.state(0)[state.length - 1]);
  });

  test('ODE model with rounding error on output end time', () => {
    // Note: with an output start time of -1, an output end time of 0, and 49 steps, the time of the last step is
    //       computed as -1 + 49 * (1 / 49), which is not exactly 0 due to rounding errors. This used to result in an
    //       extra step being taken and its results being written past the end of our results arrays.

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.initialTime = -1.0;
    simulation.outputStartTime = -1.0;
    simulation.outputEndTime = 0.0;
    simulation.numberOfSteps = 49;

    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
    assert.strictEqual(instance.progress, 1.0);

    const voi = instance.tasks[0].voi;

    assert.strictEqual(voi.length, 50);
    assert.strictEqual(voi[0], -1.0);
    assert.strictEqual(voi[voi.length - 1], 0.0);
  });

  test('NLA model', () => {
    const file = new loc.File(utils.resourcePath('api/sed/nla.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const kinsol = simulation.nlaSolver;

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    kinsol.upperHalfBandwidth = -1;

    let instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.'
      ]
    ]);

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.DENSE;

    instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('NLA model with no NLA solver', () => {
    const file = new loc.File(utils.resourcePath('api/sed/nla.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    document.simulations[0].nlaSolver = null;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."
      ]
    ]);
  });

  test('DAE model', () => {
    const file = new loc.File(utils.resourcePath('api/sed/dae.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const kinsol = simulation.nlaSolver;

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    kinsol.upperHalfBandwidth = -1;

    let instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.'
      ]
    ]);

    instance.run();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.'
      ]
    ]);

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.DENSE;

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('DAE model with no ODE or NLA solver', () => {
    const file = new loc.File(utils.resourcePath('api/sed/dae.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];

    simulation.odeSolver = null;
    simulation.nlaSolver = null;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."
      ],
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."
      ]
    ]);
  });

  test('DAE model with non-uniform time course simulation', () => {
    const file = new loc.File(utils.resourcePath('api/sed/dae.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument();
    const model = new loc.SedModel(document, file);
    const oneStep = new loc.SedOneStep(document);
    const task = new loc.SedTask(document, model, oneStep);

    document.addModel(model);
    document.addSimulation(oneStep);
    document.addTask(task);

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which (currently) requires a uniform time course simulation."
      ],
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."
      ],
      [
        loc.Issue.Type.ERROR,
        "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."
      ]
    ]);
  });

  test('DAE model with failing ODE solver', () => {
    const file = new loc.File(utils.resourcePath('api/sed/dae.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const cvode = simulation.odeSolver;

    cvode.maximumNumberOfSteps = 1;

    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task | CVODE: at t = 1.08537561647883e-09, mxstep steps taken before reaching tout.']
    ]);
  });

  test('COMBINE archive', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('COMBINE archive with CellML file as master file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/cellml_file_as_master_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('DAE model from CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);

    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('DAE model from SED-ML file', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assert.strictEqual(document.hasIssues, false);

    const nlaSolver = document.simulations[0].nlaSolver;

    assert.strictEqual(nlaSolver.linearSolver, loc.SolverKinsol.LinearSolver.GMRES);
    assert.strictEqual(nlaSolver.maximumNumberOfIterations, 123);
    assert.strictEqual(nlaSolver.upperHalfBandwidth, 1);
    assert.strictEqual(nlaSolver.lowerHalfBandwidth, 1);

    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('DAE model from COMBINE archive', () => {
    const file = new loc.File(utils.resourcePath('api/sed/dae/model.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);

    const nlaSolver = document.simulations[0].nlaSolver;

    assert.strictEqual(nlaSolver.linearSolver, loc.SolverKinsol.LinearSolver.GMRES);
    assert.strictEqual(nlaSolver.maximumNumberOfIterations, 123);
    assert.strictEqual(nlaSolver.upperHalfBandwidth, 1);
    assert.strictEqual(nlaSolver.lowerHalfBandwidth, 1);

    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('DAE model from legacy SED-ML file', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assert.strictEqual(document.hasIssues, false);

    const nlaSolver = document.simulations[0].nlaSolver;

    assert.strictEqual(nlaSolver.linearSolver, loc.SolverKinsol.LinearSolver.GMRES);
    assert.strictEqual(nlaSolver.maximumNumberOfIterations, 123);
    assert.strictEqual(nlaSolver.upperHalfBandwidth, 1);
    assert.strictEqual(nlaSolver.lowerHalfBandwidth, 1);

    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('DAE model from legacy COMBINE archive', () => {
    const file = new loc.File(utils.resourcePath('api/sed/dae/model_legacy.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);

    const nlaSolver = document.simulations[0].nlaSolver;

    assert.strictEqual(nlaSolver.linearSolver, loc.SolverKinsol.LinearSolver.GMRES);
    assert.strictEqual(nlaSolver.maximumNumberOfIterations, 123);
    assert.strictEqual(nlaSolver.upperHalfBandwidth, 1);
    assert.strictEqual(nlaSolver.lowerHalfBandwidth, 1);

    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('Simulation with initial time', () => {
    const file = new loc.File(utils.resourcePath('api/sed/simulation_with_initial_time.omex'));

    file.setContents(utils.fileContents(file.path));

    const instance = new loc.SedDocument(file).instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
    assert.strictEqual(instance.progress, 1.0);

    const instanceTask = instance.tasks[0];
    const voi = instanceTask.voi;

    assert.strictEqual(voi.length, 50001);
    assert.strictEqual(voi[0], 0.0);
    assert.strictEqual(voi[50000], 50.0);

    const x = instanceTask.state(0);
    const y = instanceTask.state(1);
    const z = instanceTask.state(2);

    assert.strictEqual(x.length, 50001);
    assert.strictEqual(y.length, 50001);
    assert.strictEqual(z.length, 50001);

    assert.notStrictEqual(x[0], 1.0);
    assert.notStrictEqual(y[0], 1.0);
    assert.notStrictEqual(z[0], 1.0);
  });

  test('Simulation with initial time failing', () => {
    const file = new loc.File(utils.resourcePath('api/sed/simulation_with_initial_time_failing.omex'));

    file.setContents(utils.fileContents(file.path));

    const instance = new loc.SedDocument(file).instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assert.strictEqual(instance.hasIssues, true);
  });

  test('Changes to variables used to initialise other variables', () => {
    const file = new loc.File(utils.resourcePath('api/sed/variables_initialised_using_variables.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();
    const model = document.model(0);
    const instanceTask = instance.tasks[0];

    const initialValue = (name) => {
      for (let i = 0; i < instanceTask.stateCount; ++i) {
        if (instanceTask.stateName(i) === name) {
          return instanceTask.state(i)[0];
        }
      }

      for (let i = 0; i < instanceTask.constantCount; ++i) {
        if (instanceTask.constantName(i) === name) {
          return instanceTask.constant(i)[0];
        }
      }

      for (let i = 0; i < instanceTask.computedConstantCount; ++i) {
        if (instanceTask.computedConstantName(i) === name) {
          return instanceTask.computedConstant(i)[0];
        }
      }

      return Number.NaN;
    };

    const checkInitialValues = (expectedInitialValues) => {
      instance.run();

      assert.strictEqual(instance.hasIssues, false);

      for (const [name, expectedInitialValue] of Object.entries(expectedInitialValues)) {
        assert.ok(Math.abs(initialValue(name) - expectedInitialValue) <= 1e-12 * Math.abs(expectedInitialValue), name);
      }
    };

    // No changes.

    checkInitialValues({
      'main/x': 3.0,
      'main/y': 3.0,
      'main/k2': 3.0,
      'main/k3': 3.0,
      'main/cc': 6.0,
      'main/z': 6.0,
      'main/w': 6.0,
      'main/v': 3.0,
      'main/u': 0.005,
      'main/q': 0.005,
      'main/r': 2.0
    });

    // Change a constant that is used (directly or indirectly) to initialise some variables.

    model.addChange(new loc.SedChangeAttribute('main', 'k', '5.0'));

    checkInitialValues({
      'main/x': 5.0,
      'main/y': 5.0,
      'main/k2': 5.0,
      'main/k3': 5.0,
      'main/cc': 10.0,
      'main/z': 10.0,
      'main/w': 10.0,
      'main/v': 5.0,
      'main/u': 0.005,
      'main/q': 0.005,
      'main/r': 2.0
    });

    // Change a state that is used (directly or indirectly) to initialise some variables.

    model.addChange(new loc.SedChangeAttribute('main', 'x', '7.0'));

    checkInitialValues({
      'main/x': 7.0,
      'main/y': 7.0,
      'main/k2': 5.0,
      'main/k3': 7.0,
      'main/cc': 14.0,
      'main/z': 14.0,
      'main/w': 14.0,
      'main/v': 5.0,
      'main/u': 0.005,
      'main/q': 0.005,
      'main/r': 2.0
    });

    // Change a state that is initialised using a computed constant.

    model.addChange(new loc.SedChangeAttribute('main', 'z', '1.0'));

    checkInitialValues({
      'main/x': 7.0,
      'main/y': 7.0,
      'main/k2': 5.0,
      'main/k3': 7.0,
      'main/cc': 14.0,
      'main/z': 1.0,
      'main/w': 1.0,
      'main/v': 5.0,
      'main/u': 0.005,
      'main/q': 0.005,
      'main/r': 2.0
    });

    // Change a constant that is initialised using a state.

    model.addChange(new loc.SedChangeAttribute('main', 'k3', '2.0'));

    checkInitialValues({
      'main/x': 7.0,
      'main/y': 7.0,
      'main/k2': 5.0,
      'main/k3': 2.0,
      'main/cc': 4.0,
      'main/z': 1.0,
      'main/w': 1.0,
      'main/v': 5.0,
      'main/u': 0.005,
      'main/q': 0.005,
      'main/r': 2.0
    });

    // Change some constants that are used to initialise some variables in another component and with different units.

    model.addChange(new loc.SedChangeAttribute('initialisation', 'u_init', '2.0'));
    model.addChange(new loc.SedChangeAttribute('initialisation', 'p', '4.0'));

    checkInitialValues({
      'main/x': 7.0,
      'main/y': 7.0,
      'main/k2': 5.0,
      'main/k3': 2.0,
      'main/cc': 4.0,
      'main/z': 1.0,
      'main/w': 1.0,
      'main/v': 5.0,
      'main/u': 0.002,
      'main/q': 0.004,
      'main/r': 2.0
    });

    // Change a constant that is used to initialise a variable in the same component but with different units.

    model.addChange(new loc.SedChangeAttribute('main', 's', '4.0'));

    checkInitialValues({
      'main/x': 7.0,
      'main/y': 7.0,
      'main/k2': 5.0,
      'main/k3': 2.0,
      'main/cc': 4.0,
      'main/z': 1.0,
      'main/w': 1.0,
      'main/v': 5.0,
      'main/u': 0.002,
      'main/q': 0.004,
      'main/r': 4.0
    });

    // Remove all our changes.

    model.removeAllChanges();

    checkInitialValues({
      'main/x': 3.0,
      'main/y': 3.0,
      'main/k2': 3.0,
      'main/k3': 3.0,
      'main/cc': 6.0,
      'main/z': 6.0,
      'main/w': 6.0,
      'main/v': 3.0,
      'main/u': 0.005,
      'main/q': 0.005,
      'main/r': 2.0
    });
  });
});
