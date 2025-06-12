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

describe("Solver CVODE tests", () => {
  let solverOdeContentsPtr;

  beforeAll(() => {
    solverOdeContentsPtr = utils.allocateMemory(loc, utils.SOLVER_ODE_CONTENTS);
  });

  afterAll(() => {
    utils.freeMemory(loc, solverOdeContentsPtr);
  });

  test("Maximum step value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.maximumStep = -1.234;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The maximum step cannot be equal to -1.234. It must be greater or equal to 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Maximum number of steps value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.maximumNumberOfSteps = 0;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The maximum number of steps cannot be equal to 0. It must be greater than 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and upper half-bandwidth value with number too small", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.upperHalfBandwidth = -1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and upper half-bandwidth value with number too big", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.upperHalfBandwidth = 4;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and lower half-bandwidth value with number too small", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.lowerHalfBandwidth = -1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and lower half-bandwidth value with number too big", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.lowerHalfBandwidth = 4;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Relative tolerance value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.relativeTolerance = -1.234;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Absolute tolerance value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.absoluteTolerance = -1.234;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Solve", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);

    odeModel.run(
      document,
      [-63.886006, 0.135007, 0.984334, 0.740972],
      [49.72578, -0.128193, -0.050905, 0.098649],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.981902, -823.516722, 789.778714, 3.96988, 0.114988, 0.00287,
        0.967346, 0.541333, 0.056246,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve without interpolate solution", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.interpolateSolution = false;

    odeModel.run(
      document,
      [-63.886395, 0.135008, 0.984334, 0.740972],
      [49.725709, -0.128194, -0.050903, 0.098651],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.982019, -823.51695, 789.77946, 3.969916, 0.114985, 0.00287,
        0.967348, 0.541337, 0.056246,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with Adams-Moulton integration method", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.integrationMethod = loc.SolverCvode.IntegrationMethod.ADAMS_MOULTON;

    odeModel.run(
      document,
      [-63.888155, 0.135013, 0.984336, 0.740968],
      [49.725742, -0.128199, -0.050896, 0.098658],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.982546, -823.520056, 789.782845, 3.970081, 0.114974, 0.002869,
        0.967353, 0.541354, 0.056245,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with functional iteration type", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.iterationType = loc.SolverCvode.IterationType.FUNCTIONAL;

    odeModel.run(
      document,
      [-63.88619, 0.135008, 0.984334, 0.740972],
      [49.72565, -0.128193, -0.050904, 0.09865],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.981957, -823.516742, 789.779253, 3.969897, 0.114987, 0.00287,
        0.967347, 0.541335, 0.056246,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with banded linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;

    odeModel.run(
      document,
      [-54.957552, 0.114715, 0.971364, 0.756968],
      [47.196456, -0.10208, -0.092979, 0.062976],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -13.303366, -791.427395, 757.54033, 3.153423, 0.188831, 0.004484,
        0.923844, 0.454648, 0.062887,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with diagonal linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.DIAGONAL;

    odeModel.run(
      document,
      [-63.886488, 0.135009, 0.984334, 0.740972],
      [49.725561, -0.128191, -0.050903, 0.098651],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.982046, -823.518024, 789.780604, 3.969925, 0.114985, 0.00287,
        0.967348, 0.541338, 0.056246,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with GMRES linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.GMRES;

    odeModel.run(
      document,
      [9.518468, 0.367366, 0.01612, 0.491145],
      [-0.864067, 0.064354, 0.001718, -0.052716],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, 6.03944, -5.198281, 0.022996, 0.112958, 6.787605, 0.112665,
        0.018857, 0.032306, 0.140794,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with Bi-CGstab linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BICGSTAB;

    odeModel.run(
      document,
      [9.518406, 0.36737, 0.016121, 0.491141],
      [-0.864079, 0.064354, 0.001718, -0.052715],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, 6.039422, -5.198258, 0.022996, 0.112959, 6.787581, 0.112665,
        0.018857, 0.032306, 0.140793,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with TFQMR linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.TFQMR;

    odeModel.run(
      document,
      [9.518097, 0.367396, 0.016121, 0.491122],
      [-0.864142, 0.064349, 0.001718, -0.052711],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, 6.039329, -5.198098, 0.023, 0.112961, 6.787465, 0.112663, 0.018857,
        0.032307, 0.140793,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with GMRES linear solver and no preconditioner", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.GMRES;
    solver.preconditioner = loc.SolverCvode.Preconditioner.NO;

    odeModel.run(
      document,
      [-63.88632, 0.135008, 0.984334, 0.740972],
      [49.725847, -0.128194, -0.050904, 0.09865],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.981996, -823.516944, 789.779343, 3.969909, 0.114986, 0.00287,
        0.967347, 0.541336, 0.056246,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with Bi-CGstab linear solver and no preconditioner", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BICGSTAB;
    solver.preconditioner = loc.SolverCvode.Preconditioner.NO;

    odeModel.run(
      document,
      [-63.886025, 0.135008, 0.984333, 0.740972],
      [49.725781, -0.128193, -0.050904, 0.098649],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.981908, -823.516709, 789.778962, 3.969882, 0.114988, 0.00287,
        0.967346, 0.541333, 0.056246,
      ],
    );

    document.delete();
    file.delete();
  });

  test("Solve with TFQMR linear solver and no preconditioner", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.TFQMR;
    solver.preconditioner = loc.SolverCvode.Preconditioner.NO;

    odeModel.run(
      document,
      [-63.886116, 0.135008, 0.984333, 0.740972],
      [49.725704, -0.128193, -0.050903, 0.098649],
      [1.0, 0.0, 0.3, 120.0, 36.0],
      [-10.613, -115.0, 12.0],
      [
        0.0, -15.981935, -823.516302, 789.778803, 3.96989, 0.114987, 0.00287,
        0.967347, 0.541334, 0.056246,
      ],
    );

    document.delete();
    file.delete();
  });
});
