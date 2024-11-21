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

const libopencor = await libOpenCOR();

describe("File coverage tests", () => {
  let someNullCharacterContentsPtr;

  beforeAll(() => {
    someNullCharacterContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_NULL_CHARACTER_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someNullCharacterContentsPtr);
  });

  test("Empty file", () => {
    const file = new libopencor.File(utils.UNKNOWN_FILE);

    file.setContents(null, 0);

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);

    file.delete();
  });

  test("File with null character", () => {
    const file = new libopencor.File(utils.UNKNOWN_FILE);

    file.setContents(
      someNullCharacterContentsPtr,
      utils.SOME_NULL_CHARACTER_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);

    file.delete();
  });

  test("SED-ML file with no parent", () => {
    const file = new libopencor.File(utils.SEDML_FILE);

    file.setContents(
      utils.SOME_SEDML_CONTENTS,
      utils.SOME_SEDML_CONTENTS.length,
    );
  });

  test("Same local file", () => {
    const file1 = new libopencor.File(utils.LOCAL_FILE);
    const file2 = new libopencor.File(utils.LOCAL_FILE);

    expect(file1).toStrictEqual(file2);

    file1.delete();
    file2.delete();
  });

  test("Same remote file", () => {
    const file1 = new libopencor.File(utils.REMOTE_FILE);
    const file2 = new libopencor.File(utils.REMOTE_FILE);

    expect(file1).toStrictEqual(file2);

    file1.delete();
    file2.delete();
  });
});
