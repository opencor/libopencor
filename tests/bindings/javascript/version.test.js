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

const loc = await libOpenCOR();

describe("Version tests", () => {
  test("libOpenCOR", () => {
    const versionMajor = 0;
    const versionPatch = 0;

    const now = new Date();
    const year = now.getFullYear();
    const month = now.getMonth() + 1;
    const day = now.getDate();

    let version = BigInt(0);
    let number = BigInt(
      10000000000 * versionMajor +
        100 * (10000 * year + 100 * month + day) +
        versionPatch,
    );
    let i = BigInt(0);

    const ten = BigInt(10);
    const four = BigInt(4);

    while (number != 0) {
      version |= number % ten << i;
      number /= ten;
      i += four;
    }

    expect(loc.version()).toBe(version);
    expect(loc.versionString()).toBe(
      `${versionMajor}.${year}${String(month).padStart(2, "0")}${String(day).padStart(2, "0")}.${versionPatch}`,
    );
  });

  test("Clang", () => {
    expect(loc.clangVersion()).toBe(0x160006);
    expect(loc.clangVersionString()).toBe("16.0.6");
  });

  test("libCellML", () => {
    expect(loc.libcellmlVersion()).toBe(0x000500);
    expect(loc.libcellmlVersionString()).toBe("0.5.0");
  });

  test("libCOMBINE", () => {
    expect(loc.libcombineVersion()).toBe(220);
    expect(loc.libcombineVersionString()).toBe("0.2.20");
  });

  test("libSEDML", () => {
    expect(loc.libsedmlVersion()).toBe(20033);
    expect(loc.libsedmlVersionString()).toBe("2.0.33");
  });

  test("LLVM", () => {
    expect(loc.llvmVersion()).toBe(0x160006);
    expect(loc.llvmVersionString()).toBe("16.0.6");
  });

  test("SUNDIALS", () => {
    expect(loc.sundialsVersion()).toBe(0x070300);
    expect(loc.sundialsVersionString()).toBe("7.3.0");
  });
});
