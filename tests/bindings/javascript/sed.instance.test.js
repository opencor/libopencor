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

import assert from "node:assert";
import test from "node:test";

import libOpenCOR from "./libopencor.js";
import * as utils from "./utils.js";
import { assertIssues } from "./utils.js";

const loc = await libOpenCOR();

test.describe("Sed instance tests", () => {
  let cellmlContentsPtr;
  let errorCellmlContentsPtr;
  let overconstrainedContentsPtr;
  let underconstrainedContentsPtr;
  let unsuitablyConstrainedContentsPtr;
  let algebraicContentsPtr;
  let nlaContentsPtr;
  let daeContentsPtr;
  let combineArchiveContentsPtr;
  let combineArchiveWithCellmlFileAsMasterFileContentsPtr;

  test.before(() => {
    cellmlContentsPtr = utils.allocateMemory(loc, utils.CELLML_CONTENTS);
    errorCellmlContentsPtr = utils.allocateMemory(
      loc,
      utils.ERROR_CELLML_CONTENTS,
    );
    overconstrainedContentsPtr = utils.allocateMemory(
      loc,
      utils.OVERCONSTRAINED_CONTENTS,
    );
    underconstrainedContentsPtr = utils.allocateMemory(
      loc,
      utils.UNDERCONSTRAINED_CONTENTS,
    );
    unsuitablyConstrainedContentsPtr = utils.allocateMemory(
      loc,
      utils.UNSUITABLY_CONSTRAINED_CONTENTS,
    );
    algebraicContentsPtr = utils.allocateMemory(loc, utils.ALGEBRAIC_CONTENTS);
    nlaContentsPtr = utils.allocateMemory(loc, utils.NLA_CONTENTS);
    daeContentsPtr = utils.allocateMemory(loc, utils.DAE_CONTENTS);
    combineArchiveContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_CONTENTS,
    );
    combineArchiveWithCellmlFileAsMasterFileContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_WITH_CELLML_FILE_AS_MASTER_FILE_CONTENTS,
    );
  });

  test.after(() => {
    utils.freeMemory(loc, cellmlContentsPtr);
    utils.freeMemory(loc, errorCellmlContentsPtr);
    utils.freeMemory(loc, overconstrainedContentsPtr);
    utils.freeMemory(loc, underconstrainedContentsPtr);
    utils.freeMemory(loc, unsuitablyConstrainedContentsPtr);
    utils.freeMemory(loc, algebraicContentsPtr);
    utils.freeMemory(loc, nlaContentsPtr);
    utils.freeMemory(loc, daeContentsPtr);
    utils.freeMemory(loc, combineArchiveContentsPtr);
    utils.freeMemory(loc, combineArchiveWithCellmlFileAsMasterFileContentsPtr);
  });

  test("No file", () => {
    const document = new loc.SedDocument();
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The simulation experiment description does not contain any tasks to run.",
      ],
    ]);

    instance.delete();
    document.delete();
  });

  test("Invalid CellML file", () => {
    const file = new loc.File(utils.ERROR_CELLML_FILE);

    file.setContents(
      errorCellmlContentsPtr,
      utils.ERROR_CELLML_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "The CellML file is invalid."],
      [
        loc.Issue.Type.ERROR,
        "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS).",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Overconstrained CellML file", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      overconstrainedContentsPtr,
      utils.OVERCONSTRAINED_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "The CellML file is overconstrained."],
      [
        loc.Issue.Type.ERROR,
        "Variable 'x' in component 'my_component' is computed more than once.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Underconstrained CellML file", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      underconstrainedContentsPtr,
      utils.UNDERCONSTRAINED_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "The CellML file is underconstrained."],
      [
        loc.Issue.Type.ERROR,
        "The type of variable 'x' in component 'my_component' is unknown.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Unsuitably constrained CellML file", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(
      unsuitablyConstrainedContentsPtr,
      utils.UNSUITABLY_CONSTRAINED_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "The CellML file is unsuitably constrained."],
      [
        loc.Issue.Type.ERROR,
        "Variable 'y' in component 'my_component' is computed more than once.",
      ],
      [
        loc.Issue.Type.ERROR,
        "The type of variable 'x' in component 'my_component' is unknown.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Algebraic model", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(algebraicContentsPtr, utils.ALGEBRAIC_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("ODE model", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const cvode = simulation.odeSolver;

    cvode.maximumNumberOfSteps = 10;

    let instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "At t = 0.00140013827899996, mxstep steps taken before reaching tout.",
      ],
    ]);

    instance.delete();

    cvode.maximumNumberOfSteps = 500;

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("ODE model with no ODE solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    const document = new loc.SedDocument(file);

    document.simulations.get(0).odeSolver = null;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("NLA model", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(nlaContentsPtr, utils.NLA_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const kinsol = simulation.nlaSolver;

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    kinsol.upperHalfBandwidth = -1;

    let instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
      ],
    ]);

    instance.delete();

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.DENSE;

    instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("NLA model with no NLA solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(nlaContentsPtr, utils.NLA_CONTENTS.length);

    const document = new loc.SedDocument(file);

    document.simulations.get(0).nlaSolver = null;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("DAE model", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(daeContentsPtr, utils.DAE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const kinsol = simulation.nlaSolver;

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.BANDED;
    kinsol.upperHalfBandwidth = -1;

    let instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
      ],
    ]);

    instance.run();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
      ],
    ]);

    instance.delete();

    kinsol.linearSolver = loc.SolverKinsol.LinearSolver.DENSE;

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("DAE model with no ODE or NLA solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(daeContentsPtr, utils.DAE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.odeSolver = null;
    simulation.nlaSolver = null;

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
      ],
      [
        loc.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("COMBINE archive", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveContentsPtr,
      utils.COMBINE_ARCHIVE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("COMBINE archive with CellML file as master file", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveWithCellmlFileAsMasterFileContentsPtr,
      utils.COMBINE_ARCHIVE_WITH_CELLML_FILE_AS_MASTER_FILE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.delete();
    document.delete();
    file.delete();
  });
});
