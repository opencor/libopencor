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
import * as utils from "./utils.js";
//---GRY--- TO BE ENABLED ONCE WE CAN INTERPRET NLA-BASED MODELS.
// import { expectIssues } from "./utils.js";

const loc = await libOpenCOR();

describe("Solver KINSOL tests", () => {
  let solverNla1ContentsPtr;
  let solverNla2ContentsPtr;

  beforeAll(() => {
    solverNla1ContentsPtr = utils.allocateMemory(
      loc,
      utils.SOLVER_NLA1_CONTENTS,
    );
    solverNla2ContentsPtr = utils.allocateMemory(
      loc,
      utils.SOLVER_NLA2_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(loc, solverNla1ContentsPtr);
    utils.freeMemory(loc, solverNla2ContentsPtr);
  });

  test("Dummy", () => {});

  /*---GRY--- ENABLE THOSE TESTS ONCE WE CAN INTERPRET NLA-BASED MODELS.
  test("Maximum number of iterations value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla1ContentsPtr,
      utils.SOLVER_NLA1_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.maximumNumberOfIterations = -1.234;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The maximum number of iterations cannot be equal to 0. It must be greater than 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and upper half-bandwidth value with number too small", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla2ContentsPtr,
      utils.SOLVER_NLA2_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    solver.upperHalfBandwidth = -1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 2.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and upper half-bandwidth value with number too big", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla1ContentsPtr,
      utils.SOLVER_NLA1_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    solver.upperHalfBandwidth = 1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to 1. It must be between 0 and 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and lower half-bandwidth value with number too small", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla2ContentsPtr,
      utils.SOLVER_NLA2_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    solver.lowerHalfBandwidth = -1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 2.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and lower half-bandwidth value with number too big", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla1ContentsPtr,
      utils.SOLVER_NLA1_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    solver.lowerHalfBandwidth = 1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The lower half-bandwidth cannot be equal to 1. It must be between 0 and 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  function expectNla1Solution(instanceTask) {
    expect(instanceTask.stateCount).toBe(0);
    expect(instanceTask.stateCount).toBe(0);
    expect(instanceTask.rateCount).toBe(0);
    expect(instanceTask.constantCount).toBe(0);
    expect(instanceTask.computedConstantCount).toBe(0);
    expect(instanceTask.algebraicCount).toBe(2);

    expect(instanceTask.algebraic(0).get(0)).toBeCloseTo(3.0, 5);
    expect(instanceTask.algebraic(1).get(0)).toBeCloseTo(7.0, 5);
  }

  function expectNla2Solution(instanceTask) {
    expect(instanceTask.stateCount).toBe(0);
    expect(instanceTask.rateCount).toBe(0);
    expect(instanceTask.constantCount).toBe(0);
    expect(instanceTask.computedConstantCount).toBe(0);
    expect(instanceTask.algebraicCount).toBe(3);

    expect(instanceTask.algebraic(0).get(0)).toBeCloseTo(3.0, 5);
    expect(instanceTask.algebraic(1).get(0)).toBeCloseTo(7.0, 5);
    expect(instanceTask.algebraic(2).get(0)).toBeCloseTo(-5.0, 5);
  }

  test("Solve", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla1ContentsPtr,
      utils.SOLVER_NLA1_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    expectNla1Solution(instance.tasks.get(0));

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Solve with banded linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla2ContentsPtr,
      utils.SOLVER_NLA2_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    solver.upperHalfBandwidth = 2;
    solver.lowerHalfBandwidth = 2;

    const instance = document.instantiate();

    instance.run();

    expectNla2Solution(instance.tasks.get(0));

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Solve with GMRES linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla1ContentsPtr,
      utils.SOLVER_NLA1_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.GMRES;

    const instance = document.instantiate();

    instance.run();

    expectNla1Solution(instance.tasks.get(0));

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Solve with BiCGStab linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla2ContentsPtr,
      utils.SOLVER_NLA2_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BICGSTAB;

    const instance = document.instantiate();

    instance.run();

    expectNla2Solution(instance.tasks.get(0));

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Solve with TFQMR linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      solverNla1ContentsPtr,
      utils.SOLVER_NLA1_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = loc.SolverKinsol.LinearSolver.TFQMR;

    const instance = document.instantiate();

    instance.run();

    expectNla1Solution(instance.tasks.get(0));

    instance.delete();
    document.delete();
    file.delete();
  });
    */
});
