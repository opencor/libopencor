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
    const now = new Date();
    const year = now.getFullYear();
    const month = now.getMonth() + 1;
    const day = now.getDate();

    let version = 0;
    let number = 10000 * year + 100 * month + day;
    let i = 0;

    while (number != 0) {
      version |= number % 10 << i;
      number = Math.floor(number / 10);
      i += 4;
    }

    expect(libopencor.version()).toBe(version);
    expect(libopencor.versionString()).toBe(
      `${year}${String(month).padStart(2, "0")}${String(day).padStart(2, "0")}`,
    );
  });

  test("libCellML", () => {
    expect(libopencor.libcellmlVersion()).toBe(0x000500);
    expect(libopencor.libcellmlVersionString()).toBe("0.5.0");
  });

  test("libCOMBINE", () => {
    expect(libopencor.libcombineVersion()).toBe(220);
    expect(libopencor.libcombineVersionString()).toBe("0.2.20");
  });

  test("libSEDML", () => {
    expect(libopencor.libsedmlVersion()).toBe(20032);
    expect(libopencor.libsedmlVersionString()).toBe("2.0.32");
  });

  test("SUNDIALS", () => {
    expect(libopencor.sundialsVersion()).toBe(0x070100);
    expect(libopencor.sundialsVersionString()).toBe("7.1.0");
  });
});
