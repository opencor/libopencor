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

test.describe("File type tests", () => {
  let unknownContentsPtr;
  let cellmlContentsPtr;
  let sedmlContentsPtr;
  let combineArchiveContentsPtr;

  test.before(() => {
    unknownContentsPtr = utils.allocateMemory(loc, utils.UNKNOWN_CONTENTS);
    cellmlContentsPtr = utils.allocateMemory(loc, utils.CELLML_CONTENTS);
    sedmlContentsPtr = utils.allocateMemory(loc, utils.SEDML_CONTENTS);
    combineArchiveContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_CONTENTS,
    );
  });

  test.after(() => {
    utils.freeMemory(loc, unknownContentsPtr);
    utils.freeMemory(loc, cellmlContentsPtr);
    utils.freeMemory(loc, sedmlContentsPtr);
    utils.freeMemory(loc, combineArchiveContentsPtr);
  });

  test("Unknown file", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(unknownContentsPtr, utils.UNKNOWN_CONTENTS.length);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assertIssues(loc, file, [
      [
        loc.Issue.Type.ERROR,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
      ],
    ]);
  });

  test("CellML file", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.type.value, loc.File.Type.CELLML_FILE.value);
  });

  test("SED-ML file", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(sedmlContentsPtr, utils.SEDML_CONTENTS.length);

    assert.strictEqual(file.type.value, loc.File.Type.SEDML_FILE.value);
  });

  test("COMBINE archive", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveContentsPtr,
      utils.COMBINE_ARCHIVE_CONTENTS.length,
    );

    assert.strictEqual(file.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });
});
