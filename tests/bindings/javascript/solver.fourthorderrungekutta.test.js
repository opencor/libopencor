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
import * as odeModel from "./ode.model.js";
import * as utils from "./utils.js";
import { expectIssues } from "./utils.js";

const loc = await libOpenCOR();

describe("Solver Fourth-Order Runge-Kutta tests", () => {
  let solverOdeContentsPtr;

  beforeAll(() => {
    solverOdeContentsPtr = utils.allocateMemory(loc, utils.SOLVER_ODE_CONTENTS);
  });

  afterAll(() => {
    utils.freeMemory(loc, solverOdeContentsPtr);
  });

  test("Step value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new loc.SolverFourthOrderRungeKutta();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The step cannot be equal to 0. It must be greater than 0.",
      ],
    ]);

    instance.delete();
    solver.delete();
    document.delete();
    file.delete();
  });

  test("Solve", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new loc.SolverFourthOrderRungeKutta();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [
        -63.821233121797334, 0.13484386454832029, 0.9842671578347936,
        0.7411048872918694,
      ],
      [7, 7, 7, 7],
      [
        49.70273520660513, -0.1279222891468715, -0.051224913137817824,
        0.0982660916488527,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.9624699365392, -823.4022571720553, 789.6619953203995,
        3.9638055464297928, 0.11540244924165802, 0.002878972863622386,
        0.9671411492217459, 0.540698489075857, 0.05629176887371371,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    solver.delete();
    document.delete();
    file.delete();
  });
});
