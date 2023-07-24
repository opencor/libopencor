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

describe("File coverage tests", () => {
  let someUnknownContentsPtr;
  let someErrorCellmlContentsPtr;
  let someWarningCellmlContentsPtr;

  beforeAll(() => {
    someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
    someErrorCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_ERROR_CELLML_CONTENTS,
    );
    someWarningCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_WARNING_CELLML_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someUnknownContentsPtr);
    utils.freeMemory(libopencor, someErrorCellmlContentsPtr);
    utils.freeMemory(libopencor, someWarningCellmlContentsPtr);
  });

  test("http remote file", () => {
    new libopencor.File(
      utils.HTTP_REMOTE_FILE,
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );
  });

  test("Error CellML file", () => {
    const expectedIssues = [
      [
        libopencor.Issue.Type.ERROR,
        "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS).",
      ],
    ];
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someErrorCellmlContentsPtr,
      utils.SOME_ERROR_CELLML_CONTENTS.length,
    );

    expectIssues(expectedIssues, file);
  });

  test("Warning CellML file", () => {
    const expectedIssues = [
      [
        libopencor.Issue.Type.WARNING,
        "The units in 'metre = 1.0' in component 'main' are not equivalent. 'metre' is in 'metre' while '1.0' is 'dimensionless'.",
      ],
    ];
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someWarningCellmlContentsPtr,
      utils.SOME_WARNING_CELLML_CONTENTS.length,
    );

    expectIssues(expectedIssues, file);
  });
});
