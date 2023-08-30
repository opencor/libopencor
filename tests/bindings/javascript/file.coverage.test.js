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
  test("Empty file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(null, 0);

    expect(file.type().value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
  });

  test("http remote file", () => {
    new libopencor.File(utils.HTTP_REMOTE_FILE);
  });

  test("Same local file", () => {
    const file1 = new libopencor.File(utils.LOCAL_FILE);
    const file2 = new libopencor.File(utils.LOCAL_FILE);

    expect(file1).toStrictEqual(file2);
  });

  test("Same remote file", () => {
    const file1 = new libopencor.File(utils.REMOTE_FILE);
    const file2 = new libopencor.File(utils.REMOTE_FILE);

    expect(file1).toStrictEqual(file2);
  });
});
