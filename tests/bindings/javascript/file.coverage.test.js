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

describe("File coverage tests", () => {
  let nullCharacterContentsPtr;
  let combineArchiveContentsPtr;

  beforeAll(() => {
    nullCharacterContentsPtr = utils.allocateMemory(
      loc,
      utils.NULL_CHARACTER_CONTENTS,
    );
    combineArchiveContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(loc, nullCharacterContentsPtr);
    utils.freeMemory(loc, combineArchiveContentsPtr);
  });

  test("Empty file", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(null, 0);

    expect(file.type.value).toBe(loc.File.Type.UNKNOWN_FILE.value);

    file.delete();
  });

  test("File with null character", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(
      nullCharacterContentsPtr,
      utils.NULL_CHARACTER_CONTENTS.length,
    );

    expect(file.type.value).toBe(loc.File.Type.UNKNOWN_FILE.value);

    file.delete();
  });

  test("SED-ML file with no parent", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(utils.SEDML_CONTENTS, utils.SEDML_CONTENTS.length);
  });

  test("Same local file", () => {
    const file1 = new loc.File(utils.LOCAL_FILE);
    const file2 = new loc.File(utils.LOCAL_FILE);

    expect(file1).toStrictEqual(file2);

    file1.delete();
    file2.delete();
  });

  test("Same remote file", () => {
    const file1 = new loc.File(utils.REMOTE_FILE);
    const file2 = new loc.File(utils.REMOTE_FILE);

    expect(file1).toStrictEqual(file2);

    file1.delete();
    file2.delete();
  });

  test("Unmanage file with children", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);
    const fileManager = loc.FileManager.instance();

    file.setContents(
      combineArchiveContentsPtr,
      utils.COMBINE_ARCHIVE_CONTENTS.length,
    );

    expect(fileManager.fileCount).toBe(4);

    fileManager.unmanage(file);

    expect(fileManager.fileCount).toBe(1);
  });
});
