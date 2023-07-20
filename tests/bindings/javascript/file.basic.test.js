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

const expectedUnknownFileIssues = [
  [
    libopencor.Issue.Type.ERROR,
    "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
  ],
];

describe("File basic tests", () => {
  let someUnknownContentsPtr;

  beforeAll(() => {
    someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someUnknownContentsPtr);
  });

  test("Local file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.fileName()).toBe(utils.LOCAL_FILE);
    expect(file.url()).toBe("");
    expect(file.contents()).toStrictEqual(utils.SOME_UNKNOWN_CONTENTS);
    expectIssues(expectedUnknownFileIssues, file);
  });

  test("Remote file", () => {
    const file = new libopencor.File(
      utils.REMOTE_FILE,
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.fileName()).toBe("");
    expect(file.url()).toBe(utils.REMOTE_FILE);
    expect(file.contents()).toStrictEqual(utils.SOME_UNKNOWN_CONTENTS);
    expectIssues(expectedUnknownFileIssues, file);
  });
});
