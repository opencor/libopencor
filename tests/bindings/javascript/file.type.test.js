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

describe("File type tests", () => {
  test("Unknown file", () => {
    const someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expectIssues(
      [
        [
          libopencor.Issue.Type.ERROR,
          "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
        ],
      ],
      file,
    );

    utils.freeMemory(libopencor, someUnknownContentsPtr);
  });

  test("CellML file", () => {
    const someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someCellmlContentsPtr,
      utils.SOME_CELLML_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.CELLML_FILE.value);

    utils.freeMemory(libopencor, someCellmlContentsPtr);
  });

  test("SED-ML file", () => {
    const someSedmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_CONTENTS,
    );
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someSedmlContentsPtr,
      utils.SOME_SEDML_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.SEDML_FILE.value);

    utils.freeMemory(libopencor, someSedmlContentsPtr);
  });
});
