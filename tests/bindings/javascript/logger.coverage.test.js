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

const loc = await libOpenCOR();

test.describe("Issue coverage tests", () => {
  let cellmlContentsPtr;
  let errorCellmlContentsPtr;
  let sedmlContentsPtr;

  test.before(() => {
    cellmlContentsPtr = utils.allocateMemory(loc, utils.CELLML_CONTENTS);
    errorCellmlContentsPtr = utils.allocateMemory(
      loc,
      utils.ERROR_CELLML_CONTENTS,
    );
    sedmlContentsPtr = utils.allocateMemory(loc, utils.SEDML_CONTENTS);
  });

  test.after(() => {
    utils.freeMemory(loc, cellmlContentsPtr);
    utils.freeMemory(loc, errorCellmlContentsPtr);
    utils.freeMemory(loc, sedmlContentsPtr);
  });

  test("hasIssues", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.hasIssues, false);
  });

  test("issueCount", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.issueCount, 0);
  });

  test("issues", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.issues.size(), 0);
  });

  test("issue()", () => {
    const file = new loc.File(utils.ERROR_CELLML_FILE);

    file.setContents(
      errorCellmlContentsPtr,
      utils.ERROR_CELLML_CONTENTS.length,
    );

    assert.notStrictEqual(file.issue(0), null);
    assert.strictEqual(file.issue(file.issueCount), null);
  });

  test("hasErrors", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.hasErrors, false);
  });

  test("errorCount", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.errorCount, 0);
  });

  test("errors", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.errors.size(), 0);
  });

  test("error()", () => {
    const file = new loc.File(utils.ERROR_CELLML_FILE);

    file.setContents(
      errorCellmlContentsPtr,
      utils.ERROR_CELLML_CONTENTS.length,
    );

    assert.notStrictEqual(file.error(0), null);
    assert.strictEqual(file.error(file.errorCount), null);
  });

  test("hasWarnings", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.hasWarnings, false);
  });

  test("warningCount", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.warningCount, 0);
  });

  test("warnings", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    assert.strictEqual(file.warnings.size(), 0);
  });

  test("warning()", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(sedmlContentsPtr, utils.SEDML_CONTENTS.length);

    const document = new loc.SedDocument(file);

    assert.notStrictEqual(document.warning(0), null);
    assert.strictEqual(document.warning(document.warningCount), null);
  });
});
