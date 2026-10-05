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
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverHeun();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task instance | Heun: the step cannot be equal to 0. It must be greater than 0.']
    ]);
  });

  test('Solve', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverHeun();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.69125875774881, 0.13451645104446466, 0.9841326274924806, 0.7413698256563633],
      [7, 7, 7, 7],
      [49.6688833220731, -0.12753254549561774, -0.05169249513778936, 0.09771079281705342],
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

  test('Solve with several steps per output point', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverHeun();

    // Note: the output interval is 0.001, hence our step means that we have several steps per output point.

    solver.step = 0.00023;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.858008378004236, 0.13493674292039365, 0.9843049673379705, 0.7410297884851652],
      [7, 7, 7, 7],
      [49.71217587285217, -0.12803270100633332, -0.05109318448839987, 0.09842339656850847],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0.0, -15.97350251340127, -823.4677642314681, 789.7290908720172, 3.967254037487925, 0.11516691467271484,
        0.0028736839793807555, 0.967257817289645, 0.5410587660096677, 0.0562658980176819
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });
});
