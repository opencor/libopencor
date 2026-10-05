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

test.describe('Solver Second-Order Runge-Kutta tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Step value with invalid number', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverSecondOrderRungeKutta();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | Second-order Runge-Kutta: the step cannot be equal to 0. It must be greater than 0.'
      ]
    ]);
  });

  test('Solve', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverSecondOrderRungeKutta();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.88652516147998, 0.13500864016199698, 0.9843341849003562, 0.7409712245332702],
      [7, 7, 7, 7],
      [49.71938512138122, -0.12811816929092643, -0.050991110380662605, 0.09854560587827293],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.982057548443994, -823.5169415955212, 789.7796140225834, 3.969928522897354, 0.11498460412672941,
        0.0028695894879267065, 0.9673480100796346, 0.5413381517778336, 0.056245845061545056
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });

  test('Solve with several steps per output point', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverSecondOrderRungeKutta();

    // Note: the output interval is 0.001, hence our step means that we have several steps per output point.

    solver.step = 0.00023;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.88651288432655, 0.13500860975935491, 0.9843342010633087, 0.7409712995545554],
      [7, 7, 7, 7],
      [49.71953096780917, -0.12811813303462535, -0.05099127164245879, 0.09854552349361953],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0.0, -15.982053865297964, -823.5171418796781, 789.7796647771669, 3.9699273713911563, 0.11498468255362436,
        0.0028695912494467683, 0.9673479713011951, 0.5413380314928411, 0.05624585369328158
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });
});
