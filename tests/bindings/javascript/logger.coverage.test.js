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

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
  });

  test("hasErrors()", () => {
    const file = new libopencor.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasErrors()).toBe(false);
  });

  test("errors()", () => {
    const file = new libopencor.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.errors().size()).toBe(0);
  });

  test("hasWarnings()", () => {
    const file = new libopencor.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasWarnings()).toBe(false);
  });

  test("warnings()", () => {
    const file = new libopencor.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.warnings().size()).toBe(0);
  });

  test("hasMessages()", () => {
    const file = new libopencor.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.hasMessages()).toBe(false);
  });

  test("messages()", () => {
    const file = new libopencor.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    expect(file.messages().size()).toBe(0);
  });
});
