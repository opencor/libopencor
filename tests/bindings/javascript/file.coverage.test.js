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

test.describe("File coverage tests", () => {
  let nullCharacterContentsPtr;
  let combineArchiveContentsPtr;

  test.before(() => {
    nullCharacterContentsPtr = utils.allocateMemory(loc, utils.NULL_CHARACTER_CONTENTS);
    combineArchiveContentsPtr = utils.allocateMemory(loc, utils.COMBINE_ARCHIVE_CONTENTS);
  });

  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test.after(() => {
    utils.freeMemory(loc, nullCharacterContentsPtr);
    utils.freeMemory(loc, combineArchiveContentsPtr);
  });

  test("Empty file", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(null, 0);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
  });

  test("File with null character", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(nullCharacterContentsPtr, utils.NULL_CHARACTER_CONTENTS.length);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
  });

  test("SED-ML file with no parent", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(utils.SEDML_CONTENTS, utils.SEDML_CONTENTS.length);
  });

  test("Same local file", () => {
    const file1 = new loc.File(utils.LOCAL_FILE);
    const file2 = new loc.File(utils.LOCAL_FILE);

    assert.deepStrictEqual(file1, file2);
  });

  test("Same remote file", () => {
    const file1 = new loc.File(utils.REMOTE_FILE);
    const file2 = new loc.File(utils.REMOTE_FILE);

    assert.deepStrictEqual(file1, file2);
  });

  test("Unmanage file with children", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);
    const fileManager = loc.FileManager.instance();

    file.setContents(combineArchiveContentsPtr, utils.COMBINE_ARCHIVE_CONTENTS.length);

    assert.strictEqual(fileManager.fileCount, 3);

    fileManager.unmanage(file);

    assert.strictEqual(fileManager.fileCount, 0);
  });
});
