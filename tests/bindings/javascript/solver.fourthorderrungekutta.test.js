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

describe("Solver Fourth-Order Runge-Kutta", () => {
  let someSolverOdeContentsPtr;

  beforeAll(() => {
    someSolverOdeContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_SOLVER_ODE_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(loc, someSolverOdeContentsPtr);
  });

  test("Step value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

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

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new loc.SolverFourthOrderRungeKutta();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.821233, 0.134844, 0.984267, 0.741105],
      [49.702735, -0.127922, -0.051225, 0.098266],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.96247, -823.402257, 789.661995, 3.963806, 0.115402, 0.002879,
        0.967141, 0.540698, 0.056292,
      ],
    );

    solver.delete();
    document.delete();
    file.delete();
  });
});
