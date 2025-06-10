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

const loc = await libOpenCOR();

describe("File type tests", () => {
  let someUnknownContentsPtr;
  let someCellmlContentsPtr;
  let someSedmlContentsPtr;
  let someCombineArchiveContentsPtr;

  beforeAll(() => {
    someUnknownContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_UNKNOWN_CONTENTS,
    );
    someCellmlContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_CELLML_CONTENTS,
    );
    someSedmlContentsPtr = utils.allocateMemory(loc, utils.SOME_SEDML_CONTENTS);
    someCombineArchiveContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(loc, someUnknownContentsPtr);
    utils.freeMemory(loc, someCellmlContentsPtr);
    utils.freeMemory(loc, someSedmlContentsPtr);
    utils.freeMemory(loc, someCombineArchiveContentsPtr);
  });

  test("Unknown file", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.type.value).toBe(loc.File.Type.UNKNOWN_FILE.value);
    expectIssues(loc, file, [
      [
        loc.Issue.Type.ERROR,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
      ],
    ]);

    file.delete();
  });

  test("CellML file", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.type.value).toBe(loc.File.Type.CELLML_FILE.value);

    file.delete();
  });

  test("SED-ML file", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(someSedmlContentsPtr, utils.SOME_SEDML_CONTENTS.length);

    expect(file.type.value).toBe(loc.File.Type.SEDML_FILE.value);

    file.delete();
  });

  test("COMBINE archive", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      someCombineArchiveContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS.length,
    );

    expect(file.type.value).toBe(loc.File.Type.COMBINE_ARCHIVE.value);

    file.delete();
  });
});
