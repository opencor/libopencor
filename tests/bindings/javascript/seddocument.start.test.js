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
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someErrorCellmlContentsPtr);
    utils.freeMemory(libopencor, someOverconstrainedContentsPtr);
    utils.freeMemory(libopencor, someUnderconstrainedContentsPtr);
    utils.freeMemory(libopencor, someUnsuitablyConstrainedContentsPtr);
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
});
