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

import test from 'node:test';

import libOpenCOR from './libopencor.js';
import * as odeModel from './ode.model.js';
import * as utils from './utils.js';
import { assertIssues } from './utils.js';

const loc = await libOpenCOR();

test.describe('Solver Heun tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Step value with invalid number', () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(utils.SOLVER_ODE_CONTENTS);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new loc.SolverHeun();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task instance | Heun: the step cannot be equal to 0. It must be greater than 0.']
    ]);
  });

  test('Solve', () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(utils.SOLVER_ODE_CONTENTS);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new loc.SolverHeun();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.69125875774881, 0.13451645104446466, 0.9841326274924806, 0.7413698256563633],
      [7, 7, 7, 7],
      [49.66942041994269, -0.12753246872257323, -0.051692906785376816, 0.09771071198596298],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.923477627324644, -823.1668113969306, 789.4214057021819, 3.95162235535478, 0.11623876280854108,
        0.002897743423202202, 0.9667255844752269, 0.539425339436885, 0.05638329929313714
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });
});
