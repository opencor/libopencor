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

const libopencor = await libOpenCOR();

describe("Version tests", () => {
  test("libOpenCOR", () => {
    expect(libopencor.version()).toBe(0x000100);
    expect(libopencor.versionString()).toBe("0.1.0");
  });

  test("libCellML", () => {
    expect(libopencor.libcellmlVersion()).toBe(0x000400);
    expect(libopencor.libcellmlVersionString()).toBe("0.4.0");
  });

  test("libCOMBINE", () => {
    expect(libopencor.libcombineVersion()).toBe(218);
    expect(libopencor.libcombineVersionString()).toBe("0.2.18");
  });

  test("libSEDML", () => {
    expect(libopencor.libsedmlVersion()).toBe(20032);
    expect(libopencor.libsedmlVersionString()).toBe("2.0.32");
  });

  test("SUNDIALS", () => {
    expect(libopencor.sundialsVersion()).toBe(0x060200);
    expect(libopencor.sundialsVersionString()).toBe("6.2.0");
  });
});
