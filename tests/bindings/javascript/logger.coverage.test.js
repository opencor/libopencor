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

const loc = await libOpenCOR();

describe("Issue coverage tests", () => {
  let someCellmlContentsPtr;
  let someErrorCellmlContentsPtr;
  let someSedmlContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_CELLML_CONTENTS,
    );
    someErrorCellmlContentsPtr = utils.allocateMemory(
      loc,
      utils.SOME_ERROR_CELLML_CONTENTS,
    );
    someSedmlContentsPtr = utils.allocateMemory(loc, utils.SOME_SEDML_CONTENTS);
  });

  afterAll(() => {
    utils.freeMemory(loc, someCellmlContentsPtr);
    utils.freeMemory(loc, someErrorCellmlContentsPtr);
    utils.freeMemory(loc, someSedmlContentsPtr);
  });

  test("hasIssues", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasIssues).toBe(false);

    file.delete();
  });

  test("issueCount", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.issueCount).toBe(0);

    file.delete();
  });

  test("issues", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.issues.size()).toBe(0);

    file.delete();
  });

  test("issue()", () => {
    const file = new loc.File(utils.ERROR_CELLML_FILE);

    file.setContents(
      someErrorCellmlContentsPtr,
      utils.SOME_ERROR_CELLML_CONTENTS.length,
    );

    expect(file.issue(0)).not.toBeNull();
    expect(file.issue(file.issueCount)).toBeNull();

    file.delete();
  });

  test("hasErrors", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasErrors).toBe(false);

    file.delete();
  });

  test("errorCount", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.errorCount).toBe(0);

    file.delete();
  });

  test("errors", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.errors.size()).toBe(0);

    file.delete();
  });

  test("error()", () => {
    const file = new loc.File(utils.ERROR_CELLML_FILE);

    file.setContents(
      someErrorCellmlContentsPtr,
      utils.SOME_ERROR_CELLML_CONTENTS.length,
    );

    expect(file.error(0)).not.toBeNull();
    expect(file.error(file.errorCount)).toBeNull();

    file.delete();
  });

  test("hasWarnings", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasWarnings).toBe(false);

    file.delete();
  });

  test("warningCount", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.warningCount).toBe(0);

    file.delete();
  });

  test("warnings", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.warnings.size()).toBe(0);

    file.delete();
  });

  test("warning()", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(someSedmlContentsPtr, utils.SOME_SEDML_CONTENTS.length);

    const document = new loc.SedDocument(file);

    expect(document.warning(0)).not.toBeNull();
    expect(document.warning(document.warningCount)).toBeNull();

    document.delete();
    file.delete();
  });
});
