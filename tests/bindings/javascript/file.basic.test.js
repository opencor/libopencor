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

describe("File basic tests", () => {
  let someUknownContents;
  let someUknownContentsPtr;

  beforeAll(async () => {
    someUknownContents = await utils.blobToString(utils.SOME_UNKNOWN_CONTENTS);
    someUknownContentsPtr = await utils.createBlobPtr(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS
    );
  });

  afterAll(() => {
    utils.deleteBlobPtr(libopencor, someUknownContentsPtr);
  });

  test("Local virtual file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someUknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.size
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.fileName()).toBe(utils.LOCAL_FILE);
    expect(file.url()).toBe("");
    expect(utils.arrayBufferToString(file.contents())).toBe(someUknownContents);
    expect(file.size()).toBe(utils.SOME_UNKNOWN_CONTENTS.size);
  });

  test("Remote virtual file", () => {
    const file = new libopencor.File(
      utils.REMOTE_FILE,
      someUknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.size
    );

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.fileName()).toBe("");
    expect(file.url()).toBe(utils.REMOTE_FILE);
    expect(utils.arrayBufferToString(file.contents())).toBe(someUknownContents);
    expect(file.size()).toBe(utils.SOME_UNKNOWN_CONTENTS.size);
  });
});
