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

    assert.strictEqual(instance.isRunning, false);
    assert.strictEqual(instance.waitForRun(), 0.0);
  });

  test('Asynchronous run lifecycle', async () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 200; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.isRunning, false);
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
    const largeStepCount = 1000000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = largeStepCount;
    simulation.outputEndTime = largeStepCount;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 60000; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.stopRun();

    for (let i = 0; i < 60000; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Stop run when not running', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.stopRun();

    assert.strictEqual(instance.isRunning, false);
    assert.strictEqual(instance.progress, 0.0);
  });

  test('Pause run and resume run', async () => {
    const largeStepCount = 1000000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = largeStepCount;
    simulation.outputEndTime = largeStepCount;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 60000; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.pauseRun();

    await sleep(50);

    instance.resumeRun();
    instance.stopRun();

    for (let i = 0; i < 60000; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.isRunning, false);
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

    assert.strictEqual(instance.isRunning, false);
    assert.strictEqual(instance.progress, 0.0);
  });

  test('Pause run then stop run', async () => {
    const largeStepCount = 1000000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = largeStepCount;
    simulation.outputEndTime = largeStepCount;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 60000; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.pauseRun();

    await sleep(50);

    instance.stopRun();

    for (let i = 0; i < 60000; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.isRunning, false);
    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Pause run and resume run with natural completion', async () => {
    const moderateStepCount = 50000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = moderateStepCount;
    simulation.outputEndTime = moderateStepCount;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 60000; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    instance.pauseRun();

    await sleep(50);

    instance.resumeRun();

    for (let i = 0; i < 60000; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.isRunning, false);
    assert.ok(instance.waitForRun() > 0.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Start run while already running', async () => {
    const largeStepCount = 1000000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = largeStepCount;
    simulation.outputEndTime = largeStepCount;

    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 60000; ++i) {
      if (instance.progress > 0.0) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.startRun(), false);

    instance.stopRun();

    for (let i = 0; i < 60000; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.isRunning, false);
    assert.ok(instance.progress < 1.0);
    assert.strictEqual(instance.hasIssues, false);
  });

  test('Start run after previous run completed', async () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 60000; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.isRunning, false);

    assert.strictEqual(instance.startRun(), true);

    for (let i = 0; i < 60000; ++i) {
      if (!instance.isRunning) {
        break;
      }

      await sleep(1);
    }

    assert.strictEqual(instance.isRunning, false);
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
});
