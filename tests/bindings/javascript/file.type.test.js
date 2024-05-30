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
  let someUnknownContentsPtr;
  let someCellmlContentsPtr;
  let someSedmlContentsPtr;
  let someCombineArchiveContentsPtr;

  beforeAll(() => {
    someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    someSedmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_CONTENTS,
    );
    someCombineArchiveContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someUnknownContentsPtr);
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someSedmlContentsPtr);
    utils.freeMemory(libopencor, someCombineArchiveContentsPtr);
  });

  test("Unknown file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expectIssues(libopencor, file, [
      [
        libopencor.Issue.Type.ERROR,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
      ],
    ]);
  });

  test("CellML file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.type().value).toBe(libopencor.File.Type.CELLML_FILE.value);
  });

  test("SED-ML file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someSedmlContentsPtr, utils.SOME_SEDML_CONTENTS.length);

    expect(file.type().value).toBe(libopencor.File.Type.SEDML_FILE.value);
  });

  test("COMBINE archive", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someCombineArchiveContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.COMBINE_ARCHIVE.value);
  });
});
