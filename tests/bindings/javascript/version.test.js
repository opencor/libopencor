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

import fs from "fs";
import assert from "node:assert";
import test from "node:test";
import path from "path";
import { fileURLToPath } from "url";

import libOpenCOR from "./libopencor.js";

const loc = await libOpenCOR();

test.describe("Version tests", () => {
  test("libOpenCOR", () => {
    const versionStr = fs
      .readFileSync(
        path.resolve(
          path.dirname(fileURLToPath(import.meta.url)),
          "../../VERSION.txt",
        ),
        "utf8",
      )
      .trim();
    const [majorVersionStr, dateStr, patchVersionStr] = versionStr.split(".");
    const majorVersion = Number(majorVersionStr);
    const date = Number(dateStr);
    const patchVersion = Number(patchVersionStr);

    let version = BigInt(0);
    let number = BigInt(10000000000 * majorVersion + 100 * date + patchVersion);
    let i = BigInt(0);

    const ten = BigInt(10);
    const four = BigInt(4);

    while (number != 0) {
      version |= number % ten << i;
      number /= ten;
      i += four;
    }

    assert.strictEqual(loc.version(), version);
    assert.strictEqual(
      loc.versionString(),
      `${majorVersion}.${date}.${patchVersion}`,
    );
  });

  test("Clang", () => {
    assert.strictEqual(loc.clangVersion(), 0x160006);
    assert.strictEqual(loc.clangVersionString(), "16.0.6");
  });

  test("libCellML", () => {
    assert.strictEqual(loc.libcellmlVersion(), 0x000603);
    assert.strictEqual(loc.libcellmlVersionString(), "0.6.3");
  });

  test("libCOMBINE", () => {
    assert.strictEqual(loc.libcombineVersion(), 220);
    assert.strictEqual(loc.libcombineVersionString(), "0.2.20");
  });

  test("libSEDML", () => {
    assert.strictEqual(loc.libsedmlVersion(), 20033);
    assert.strictEqual(loc.libsedmlVersionString(), "2.0.33");
  });

  test("LLVM", () => {
    assert.strictEqual(loc.llvmVersion(), 0x160006);
    assert.strictEqual(loc.llvmVersionString(), "16.0.6");
  });

  test("SUNDIALS", () => {
    assert.strictEqual(loc.sundialsVersion(), 0x070300);
    assert.strictEqual(loc.sundialsVersionString(), "7.3.0");
  });
});
