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

describe("Solver Heun", () => {
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
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    const solver = new libopencor.SolverHeun();

    solver.setStep(0.0);

    simulation.setOdeSolver(solver);

    const instance = document.createInstance();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The step cannot be equal to 0. It must be greater than 0.",
      ],
    ]);
  });

  test("Solve", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someSolverOdeContentsPtr,
      utils.SOME_SOLVER_ODE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    const solver = new libopencor.SolverHeun();

    solver.setStep(0.0123);

    simulation.setOdeSolver(solver);

    odeModel.run(
      libopencor,
      document,
      [-63.691259, 0.134516, 0.984133, 0.74137],
      [49.66942, -0.127532, -0.051693, 0.097711],
      [
        0.0, -15.923478, -823.166811, 789.421406, 1.0, 0.0, -10.613, 0.3,
        -115.0, 120.0, 3.951622, 0.116239, 0.002898, 0.966726, 12.0, 36.0,
        0.539425, 0.056383,
      ],
    );
  });
});
