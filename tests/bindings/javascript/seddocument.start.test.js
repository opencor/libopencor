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

describe("SedDocument start tests", () => {
  let someCellmlContentsPtr;
  let someErrorCellmlContentsPtr;
  let someOverconstrainedContentsPtr;
  let someUnderconstrainedContentsPtr;
  let someUnsuitablyConstrainedContentsPtr;
  let someAlgebraicContentsPtr;
  let someNlaContentsPtr;
  let someDaeContentsPtr;

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
  });

  test("No file", () => {
    const sed = new libopencor.SedDocument();

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [
        libopencor.Issue.Type.ERROR,
        "A simulation experiment description must (currently) have exactly one model.",
      ],
      [
        libopencor.Issue.Type.ERROR,
        "A simulation experiment description must (currently) have exactly one simulation.",
      ],
    ]);
  });

  test("Invalid CellML file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someErrorCellmlContentsPtr,
      utils.SOME_ERROR_CELLML_CONTENTS.length,
    );

    const sed = new libopencor.SedDocument(file);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [libopencor.Issue.Type.ERROR, "The CellML file is invalid."],
    ]);
  });

  test("Overconstrained CellML file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someOverconstrainedContentsPtr,
      utils.SOME_OVERCONSTRAINED_CONTENTS.length,
    );

    const sed = new libopencor.SedDocument(file);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [libopencor.Issue.Type.ERROR, "The CellML file is overconstrained."],
    ]);
  });

  test("Underconstrained CellML file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someUnderconstrainedContentsPtr,
      utils.SOME_UNDERCONSTRAINED_CONTENTS.length,
    );

    const sed = new libopencor.SedDocument(file);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [libopencor.Issue.Type.ERROR, "The CellML file is underconstrained."],
    ]);
  });

  test("UnsuitablyConstrained CellML file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someUnsuitablyConstrainedContentsPtr,
      utils.SOME_UNSUITABLY_CONSTRAINED_CONTENTS.length,
    );

    const sed = new libopencor.SedDocument(file);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [
        libopencor.Issue.Type.ERROR,
        "The CellML file is unsuitably constrained.",
      ],
    ]);
  });

  test("Algebraic model", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someAlgebraicContentsPtr,
      utils.SOME_ALGEBRAIC_CONTENTS.length,
    );

    const sed = new libopencor.SedDocument(file);

    expect(sed.start()).toBe(true);
  });

  test("ODE model", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);

    /*---GRY--- TO BE RE-ENABLED ONCE WE CAN RUN A SIMULATION FROM JavaScript.
    simulation.setNumberOfSteps(10);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [
        libopencor.Issue.Type.ERROR,
        "At t = 3.29968, mxstep steps taken before reaching tout.",
      ],
    ]);
    */

    simulation.setOutputEndTime(50.0);
    simulation.setNumberOfSteps(50000);

    expect(sed.start()).toBe(true);
  });

  test("ODE model with no ODE solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    sed.simulations().get(0).setOdeSolver(null);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [
        libopencor.Issue.Type.ERROR,
        "The simulation is to be linked to an ODE model and must therefore specify an ODE solver.",
      ],
    ]);
  });

  test("NLA model", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.start()).toBe(true);
  });

  test("NLA model with no NLA solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    sed.simulations().get(0).setNlaSolver(null);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [
        libopencor.Issue.Type.ERROR,
        "The simulation is to be linked to an NLA model and must therefore specify an NLA solver.",
      ],
    ]);
  });

  /*---GRY--- TO BE RE-ENABLED ONCE WE KNOW WHAT SETTINGS SHOULD BE USED.
  test("DAE model", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someDaeContentsPtr, utils.SOME_DAE_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.start()).toBe(true);
  });
  */

  test("DAE model with no ODE or NLA solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someDaeContentsPtr, utils.SOME_DAE_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);

    simulation.setOdeSolver(null);
    simulation.setNlaSolver(null);

    expect(sed.start()).toBe(false);

    expectIssues(sed, [
      [
        libopencor.Issue.Type.ERROR,
        "The simulation is to be linked to a DAE model and must therefore specify an ODE solver.",
      ],
      [
        libopencor.Issue.Type.ERROR,
        "The simulation is to be linked to a DAE model and must therefore specify an NLA solver.",
      ],
    ]);
  });
});
