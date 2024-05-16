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

describe("Solver Second-Order Runge-Kutta", () => {
  let someOdeModelContentsPtr;

  beforeAll(() => {
    someOdeModelContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_ODE_MODEL_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someOdeModelContentsPtr);
  });

  test("Step value with invalid number", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someOdeModelContentsPtr,
      utils.SOME_ODE_MODEL_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    const solver = new libopencor.SolverSecondOrderRungeKutta();

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
      someOdeModelContentsPtr,
      utils.SOME_ODE_MODEL_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    const solver = new libopencor.SolverSecondOrderRungeKutta();

    solver.setStep(0.0123);

    simulation.setOdeSolver(solver);

    odeModel.run(
      libopencor,
      document,
      [-63.886525, 0.135009, 0.984334, 0.740971],
      [49.725722, -0.128194, -0.050903, 0.098651],
      [
        0.0, -15.982058, -823.516942, 789.779614, 1.0, 0.0, -10.613, 0.3,
        -115.0, 120.0, 3.969929, 0.114985, 0.00287, 0.967348, 12.0, 36.0,
        0.541338, 0.056246,
      ],
    );
  });
});