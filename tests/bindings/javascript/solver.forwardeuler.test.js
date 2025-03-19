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

const libopencor = await libOpenCOR();

describe("Solver Forward Euler", () => {
  let someSolverOdeContentsPtr;

  beforeAll(() => {
    someSolverOdeContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SOLVER_ODE_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someSolverOdeContentsPtr);
  });

  test("Step value with invalid number", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new libopencor.SolverForwardEuler();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The step cannot be equal to 0. It must be greater than 0.",
      ],
    ]);

    instance.delete();
    solver.delete();
    document.delete();
    file.delete();
  });

  test("Solve", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new libopencor.SolverForwardEuler();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      libopencor,
      document,
      [-63.787727, 0.134748, 0.984255, 0.741178],
      [49.73577, -0.127963, -0.051257, 0.098331],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.952418, -823.361177, 789.590304, 3.960664, 0.115617, 0.002884,
        0.967035, 0.54037, 0.056315,
      ],
    );

    solver.delete();
    document.delete();
    file.delete();
  });
});
