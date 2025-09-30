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

const expectedUnknownFileIssues = [
  [
    loc.Issue.Type.ERROR,
    "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
  ],
];

test.describe("File basic tests", () => {
  let unknownContentsPtr;

  test.before(() => {
    unknownContentsPtr = utils.allocateMemory(loc, utils.UNKNOWN_CONTENTS);
  });

  test.after(() => {
    utils.freeMemory(loc, unknownContentsPtr);
  });

  test("Local file", () => {
    const file = new loc.File(utils.LOCAL_FILE);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, utils.LOCAL_FILE);
    assert.strictEqual(file.url, "");
    assert.strictEqual(file.path, utils.LOCAL_FILE);
    assert.deepStrictEqual(file.contents(), utils.NO_CONTENTS);
    assertIssues(loc, file, expectedUnknownFileIssues);

    file.setContents(unknownContentsPtr, utils.UNKNOWN_CONTENTS.length);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), utils.UNKNOWN_CONTENTS);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test("Remote file", () => {
    const file = new loc.File(utils.REMOTE_FILE);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, "/some/path/file");
    assert.strictEqual(file.url, utils.REMOTE_FILE);
    assert.strictEqual(file.path, utils.REMOTE_FILE);
    assert.deepStrictEqual(file.contents(), utils.NO_CONTENTS);
    assertIssues(loc, file, expectedUnknownFileIssues);

    file.setContents(unknownContentsPtr, utils.UNKNOWN_CONTENTS.length);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), utils.UNKNOWN_CONTENTS);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test("File manager", () => {
    const fileManager = loc.FileManager.instance();

    assert.strictEqual(fileManager.hasFiles, false);
    assert.strictEqual(fileManager.fileCount, 0);
    assert.strictEqual(fileManager.files.size(), 0);
    assert.strictEqual(fileManager.file(0), null);
    assert.strictEqual(
      fileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE),
      null,
    );

    const localFile = new loc.File(utils.LOCAL_FILE);
    const sameFileManager = loc.FileManager.instance();

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 1);
    assert.strictEqual(sameFileManager.files.size(), 1);
    assert.deepStrictEqual(fileManager.file(0), localFile);
    assert.deepStrictEqual(
      sameFileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE),
      localFile,
    );

    const remoteFile = new loc.File(utils.REMOTE_FILE);

    assert.strictEqual(fileManager.hasFiles, true);
    assert.strictEqual(fileManager.fileCount, 2);
    assert.strictEqual(fileManager.files.size(), 2);
    assert.deepStrictEqual(fileManager.file(1), remoteFile);
    assert.deepStrictEqual(
      fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE),
      remoteFile,
    );

    sameFileManager.unmanage(localFile);

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 1);
    assert.strictEqual(sameFileManager.files.size(), 1);
    assert.deepStrictEqual(fileManager.file(1), null);
    assert.deepStrictEqual(
      sameFileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE),
      null,
    );

    sameFileManager.manage(localFile);

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 2);
    assert.strictEqual(sameFileManager.files.size(), 2);
    assert.deepStrictEqual(fileManager.file(1), localFile);
    assert.deepStrictEqual(
      sameFileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE),
      localFile,
    );

    fileManager.reset();

    assert.strictEqual(fileManager.hasFiles, false);
    assert.strictEqual(fileManager.fileCount, 0);
    assert.strictEqual(fileManager.files.size(), 0);
    assert.deepStrictEqual(fileManager.file(0), null);
    assert.deepStrictEqual(fileManager.file(1), null);
    assert.deepStrictEqual(
      fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE),
      null,
    );
    assert.deepStrictEqual(
      fileManager.fileFromFileNameOrUrl(utils.UNKNOWN_FILE),
      null,
    );
  });
});
