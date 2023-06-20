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

describe("File type tests", () => {
  let someUnknownContentsPtr;
  let someCellmlContentsPtr;
  let someSedmlContentsPtr;

  beforeAll(async () => {
    someUnknownContentsPtr = await utils.createBlobPtr(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS
    );
    someCellmlContentsPtr = await utils.createBlobPtr(
      libopencor,
      utils.SOME_CELLML_CONTENTS
    );
    someSedmlContentsPtr = await utils.createBlobPtr(
      libopencor,
      utils.SOME_SEDML_CONTENTS
    );
  });

  afterAll(() => {
    utils.deleteBlobPtr(libopencor, someUnknownContentsPtr);
    utils.deleteBlobPtr(libopencor, someCellmlContentsPtr);
    utils.deleteBlobPtr(libopencor, someSedmlContentsPtr);
  });

  test("Unknown virtual file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.size
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
  });

  test("CellML virtual file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someCellmlContentsPtr,
      utils.SOME_CELLML_CONTENTS.size
    );

    expect(file.type().value).toBe(libopencor.File.Type.CELLML_FILE.value);
  });

  test("SED-ML virtual file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someSedmlContentsPtr,
      utils.SOME_SEDML_CONTENTS.size
    );

    expect(file.type().value).toBe(libopencor.File.Type.SEDML_FILE.value);
  });
});
