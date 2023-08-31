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

describe("Issue coverage tests", () => {
  let someCellmlContentsPtr;
  let someUnknownContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someUnknownContentsPtr);
  });

  test("issue()", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.issue(0)).toBeNull();
  });

  test("hasIssues()", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasIssues()).toBe(false);
  });

  test("hasErrors()", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasErrors()).toBe(false);
  });

  test("errorCount()", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.errorCount()).toBe(0);
  });

  test("error()", () => {
    // Has an error.

    let file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.error(0)).not.toBeNull();

    // Doesn't have an error.

    file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.error(0)).toBeNull();
  });
});
