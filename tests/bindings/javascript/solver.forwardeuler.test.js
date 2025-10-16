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

import test from "node:test";

import libOpenCOR from "./libopencor.js";
import * as odeModel from "./ode.model.js";
import * as utils from "./utils.js";
import { assertIssues } from "./utils.js";

const loc = await libOpenCOR();

test.describe("Solver Forward Euler tests", () => {
  let solverOdeContentsPtr;

  test.before(() => {
    solverOdeContentsPtr = utils.allocateMemory(loc, utils.SOLVER_ODE_CONTENTS);
  });

  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test.after(() => {
    utils.freeMemory(loc, solverOdeContentsPtr);
  });

  test("Step value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new loc.SolverForwardEuler();

    solver.step = 0.0;

    simulation.odeSolver = solver;

    const instance = document.instantiate();

    assertIssues(loc, instance, [[loc.Issue.Type.ERROR, "The step cannot be equal to 0. It must be greater than 0."]]);
  });

  test("Solve", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = new loc.SolverForwardEuler();

    solver.step = 0.0123;

    simulation.odeSolver = solver;

    odeModel.run(
      document,
      [-63.78772730984361, 0.13474847497254067, 0.9842548213064144, 0.7411775397899872],
      [7, 7, 7, 7],
      [49.735769641933295, -0.12796285748502675, -0.051257421327489505, 0.09833105129202639],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.952418192953083, -823.3611773703722, 789.5903038580858, 3.9606641552180846, 0.11561746333925448,
        0.002883800022109429, 0.9670345037893372, 0.5403702607259088, 0.05631535007932083,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );
  });
});
