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

test.describe('Solver Forward Euler tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Step value with invalid number', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverForwardEuler();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, 'Task instance | Forward Euler: the step cannot be equal to 0. It must be greater than 0.']
    ]);
  });

  test('Solve', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverForwardEuler();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.78772730984361, 0.13474847497254067, 0.9842548213064144, 0.7411775397899872],
      [7, 7, 7, 7],
      [49.72329170523945, -0.12781121226443642, -0.05143568084969071, 0.09812028768120297],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.952418192953083, -823.3611773703722, 789.5903038580858, 3.9606641552180846, 0.11561746333925448,
        0.002883800022109429, 0.9670345037893372, 0.5403702607259088, 0.05631535007932083
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });

  test('Solve with several steps per output point', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const solver = new loc.SolverForwardEuler();

    // Note: the output interval is 0.001, hence our step means that we have several steps per output point.

    solver.step = 0.00023;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.8628497072159, 0.1349464944831147, 0.9843147710706077, 0.7410206879914387],
      [7, 7, 7, 7],
      [49.71982516869149, -0.12804483158875093, -0.051095600308459874, 0.09844363921709567],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0.0, -15.974954912164769, -823.4798652631416, 789.7349950066149, 3.9677080630368713, 0.11513594322974895,
        0.002872988441057678, 0.9672731463752849, 0.541106196763732, 0.05626249309901385
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );
  });
});
