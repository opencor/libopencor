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

test.describe('Solver Fourth-Order Runge-Kutta tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Step value with invalid number', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverFourthOrderRungeKutta();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | Fourth-order Runge-Kutta: the step cannot be equal to 0. It must be greater than 0.'
      ]
    ]);
  });

  test('Solve', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverFourthOrderRungeKutta();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.821233121797334, 0.13484386454832029, 0.9842671578347936, 0.7411048872918694],
      [7, 7, 7, 7],
      [49.702731788195074, -0.12792228908800213, -0.051224913686711004, 0.09826609124379636],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.9624699365392, -823.4022571720553, 789.6619953203995, 3.9638055464297928, 0.11540244924165802,
        0.002878972863622386, 0.9671411492217459, 0.540698489075857, 0.05629176887371371
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });

  test('Solve with several steps per output point', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverFourthOrderRungeKutta();

    // Note: the output interval is 0.001, hence our step means that we have several steps per output point.

    solver.step = 0.00023;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.877005962008575, 0.1349846360910032, 0.9843244598029772, 0.7409908167381015],
      [7, 7, 7, 7],
      [49.717088373085595, -0.12808963797656314, -0.051025251075106234, 0.0985047823162028],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0.0, -15.979201788602571, -823.5007314934002, 789.7628449089171, 3.9690357121931417, 0.11504542917451763,
        0.002870955622750495, 0.9673179295109868, 0.5412448884626809, 0.056252538152474815
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });
});
