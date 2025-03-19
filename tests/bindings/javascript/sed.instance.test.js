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
import { expectIssues } from "./utils.js";

const libopencor = await libOpenCOR();

describe("Sed instance tests", () => {
  let someCellmlContentsPtr;
  let someErrorCellmlContentsPtr;
  let someOverconstrainedContentsPtr;
  let someUnderconstrainedContentsPtr;
  let someUnsuitablyConstrainedContentsPtr;
  let someAlgebraicContentsPtr;
  let someNlaContentsPtr;
  let someDaeContentsPtr;
  let someCombineArchiveContentsPtr;
  let someCombineArchiveWithCellmlFileAsMasterFileContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    someErrorCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_ERROR_CELLML_CONTENTS,
    );
    someOverconstrainedContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_OVERCONSTRAINED_CONTENTS,
    );
    someUnderconstrainedContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNDERCONSTRAINED_CONTENTS,
    );
    someUnsuitablyConstrainedContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNSUITABLY_CONSTRAINED_CONTENTS,
    );
    someAlgebraicContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_ALGEBRAIC_CONTENTS,
    );
    someNlaContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_NLA_CONTENTS,
    );
    someDaeContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_DAE_CONTENTS,
    );
    someCombineArchiveContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS,
    );
    someCombineArchiveWithCellmlFileAsMasterFileContentsPtr =
      utils.allocateMemory(
        libopencor,
        utils.SOME_COMBINE_ARCHIVE_WITH_CELLML_FILE_AS_MASTER_FILE_CONTENTS,
      );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someErrorCellmlContentsPtr);
    utils.freeMemory(libopencor, someOverconstrainedContentsPtr);
    utils.freeMemory(libopencor, someUnderconstrainedContentsPtr);
    utils.freeMemory(libopencor, someUnsuitablyConstrainedContentsPtr);
    utils.freeMemory(libopencor, someAlgebraicContentsPtr);
    utils.freeMemory(libopencor, someNlaContentsPtr);
    utils.freeMemory(libopencor, someDaeContentsPtr);
    utils.freeMemory(libopencor, someCombineArchiveContentsPtr);
    utils.freeMemory(
      libopencor,
      someCombineArchiveWithCellmlFileAsMasterFileContentsPtr,
    );
  });

  test("No file", () => {
    const document = new libopencor.SedDocument();
    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The simulation experiment description does not contain any tasks to run.",
      ],
    ]);

    instance.delete();
    document.delete();
  });

  test("Invalid CellML file", () => {
    const file = new libopencor.File(
      utils.resourcePath(utils.ERROR_CELLML_FILE),
    );

    file.setContents(
      someErrorCellmlContentsPtr,
      utils.SOME_ERROR_CELLML_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [libopencor.Issue.Type.ERROR, "The CellML file is invalid."],
      [
        libopencor.Issue.Type.ERROR,
        "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS).",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Overconstrained CellML file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(
      someOverconstrainedContentsPtr,
      utils.SOME_OVERCONSTRAINED_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [libopencor.Issue.Type.ERROR, "The CellML file is overconstrained."],
      [
        libopencor.Issue.Type.ERROR,
        "Variable 'x' in component 'my_component' is computed more than once.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Underconstrained CellML file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(
      someUnderconstrainedContentsPtr,
      utils.SOME_UNDERCONSTRAINED_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [libopencor.Issue.Type.ERROR, "The CellML file is underconstrained."],
      [
        libopencor.Issue.Type.ERROR,
        "The type of variable 'x' in component 'my_component' is unknown.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Unsuitably constrained CellML file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(
      someUnsuitablyConstrainedContentsPtr,
      utils.SOME_UNSUITABLY_CONSTRAINED_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The CellML file is unsuitably constrained.",
      ],
      [
        libopencor.Issue.Type.ERROR,
        "Variable 'y' in component 'my_component' is computed more than once.",
      ],
      [
        libopencor.Issue.Type.ERROR,
        "The type of variable 'x' in component 'my_component' is unknown.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Algebraic model", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(
      someAlgebraicContentsPtr,
      utils.SOME_ALGEBRAIC_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("ODE model", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    const cvode = simulation.odeSolver;

    cvode.maximumNumberOfSteps = 10;

    let instance = document.instantiate();

    expect(instance.hasIssues()).toBe(false);

    instance.run();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "At t = 0.00140014, mxstep steps taken before reaching tout.",
      ],
    ]);

    instance.delete();

    cvode.maximumNumberOfSteps = 500;

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("ODE model with no ODE solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    document.simulations().get(0).odeSolver = null;

    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  /*---GRY--- TO BE UNCOMMENTED ONCE WE CAN INTERPRET A MODEL WITH ONE/SEVERAL NLA SYSTEM/S.
  test("NLA model", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    const kinsol = simulation.nlaSolver;

    kinsol.linearSolver = libopencor.SolverKinsol.LinearSolver.BANDED;
    kinsol.upperHalfBandwidth = -1;

    let instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
      ],
    ]);

    instance.delete();

    kinsol.linearSolver = libopencor.SolverKinsol.LinearSolver.DENSE;

    instance = document.instantiate();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("NLA model with no NLA solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    document.simulations().get(0).nlaSolver = null;

    const instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });
  */

  /*---GRY--- TO BE UNCOMMENTED ONCE WE CAN INTERPRET A MODEL WITH ONE/SEVERAL NLA SYSTEM/S.
  test("DAE model", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someDaeContentsPtr, utils.SOME_DAE_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);
    const kinsol = simulation.nlaSolver;

    kinsol.linearSolver = libopencor.SolverKinsol.LinearSolver.BANDED;
    kinsol.upperHalfBandwidth = -1;

    let instance = document.instantiate();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
      ],
    ]);

    instance.run();

    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
      ],
    ]);

    instance.delete();

    kinsol.linearSolver = libopencor.SolverKinsol.LinearSolver.DENSE;

    instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    document.delete();
    file.delete();
  });
  */

  test("DAE model with no ODE or NLA solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someDaeContentsPtr, utils.SOME_DAE_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations().get(0);

    simulation.odeSolver = null;
    simulation.nlaSolver = null;

    const instance = document.instantiate();

    /*---GRY--- TO BE UNCOMMENTED ONCE WE CAN INTERPRET A MODEL WITH ONE/SEVERAL NLA SYSTEM/S.
    expectIssues(libopencor, instance, [
      [
        libopencor.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
      ],
      [
        libopencor.Issue.Type.ERROR,
        "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
      ],
    ]);
    */

    instance.delete();
    document.delete();
    file.delete();
  });

  test("COMBINE archive", () => {
    const file = new libopencor.File(utils.resourcePath(utils.COMBINE_ARCHIVE));

    file.setContents(
      someCombineArchiveContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("COMBINE archive with CellML file as master file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.COMBINE_ARCHIVE));

    file.setContents(
      someCombineArchiveWithCellmlFileAsMasterFileContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_WITH_CELLML_FILE_AS_MASTER_FILE_CONTENTS
        .length,
    );

    const document = new libopencor.SedDocument(file);
    const instance = document.instantiate();

    expect(instance.hasIssues()).toBe(false);

    instance.delete();
    document.delete();
    file.delete();
  });
});
