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

import assert from 'node:assert';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import { fileURLToPath } from 'node:url';

import libOpenCOR from './libopencor.js';

const loc = await libOpenCOR();

test.describe('Version tests', () => {
  test('libOpenCOR', () => {
    const versionStr = fs
      .readFileSync(path.resolve(path.dirname(fileURLToPath(import.meta.url)), '../../VERSION.txt'), 'utf8')
      .trim();
    const [majorVersionStr, dateStr, patchVersionStr] = versionStr.split('.');
    const majorVersion = Number(majorVersionStr);
    const date = Number(dateStr);
    const patchVersion = Number(patchVersionStr);

    let version = BigInt(0);
    let number = BigInt(10000000000 * majorVersion + 100 * date + patchVersion);
    let i = BigInt(0);

    const ten = BigInt(10);
    const four = BigInt(4);

    while (number !== 0n) {
      version |= (number % ten) << i;
      number /= ten;
      i += four;
    }

    assert.strictEqual(loc.version(), version);
    assert.strictEqual(loc.versionString(), versionStr);

    // The version is a big integer and the version string is a string.
    // Note: this has no C++ equivalent since C++ is statically typed.

    assert.strictEqual(typeof loc.version(), 'bigint');
    assert.strictEqual(typeof loc.versionString(), 'string');
  });

  test('libCellML', () => {
    assert.strictEqual(loc.libcellmlVersion(), 0x000701);
    assert.strictEqual(loc.libcellmlVersionString(), '0.7.1');

    // The version is a number and the version string is a string.
    // Note: this has no C++ equivalent since C++ is statically typed.

    assert.strictEqual(typeof loc.libcellmlVersion(), 'number');
    assert.strictEqual(typeof loc.libcellmlVersionString(), 'string');
  });

  test('libCOMBINE', () => {
    assert.strictEqual(loc.libcombineVersion(), 220);
    assert.strictEqual(loc.libcombineVersionString(), '0.2.20');

    // The version is a number and the version string is a string.
    // Note: this has no C++ equivalent since C++ is statically typed.

    assert.strictEqual(typeof loc.libcombineVersion(), 'number');
    assert.strictEqual(typeof loc.libcombineVersionString(), 'string');
  });

  test('libSEDML', () => {
    assert.strictEqual(loc.libsedmlVersion(), 20033);
    assert.strictEqual(loc.libsedmlVersionString(), '2.0.33');

    // The version is a number and the version string is a string.
    // Note: this has no C++ equivalent since C++ is statically typed.

    assert.strictEqual(typeof loc.libsedmlVersion(), 'number');
    assert.strictEqual(typeof loc.libsedmlVersionString(), 'string');
  });

  test('LLVM', () => {
    assert.strictEqual(loc.llvmVersion(), 0x220108);
    assert.strictEqual(loc.llvmVersionString(), '22.1.8');

    // The version is a number and the version string is a string.
    // Note: this has no C++ equivalent since C++ is statically typed.

    assert.strictEqual(typeof loc.llvmVersion(), 'number');
    assert.strictEqual(typeof loc.llvmVersionString(), 'string');
  });

  test('SUNDIALS', () => {
    assert.strictEqual(loc.sundialsVersion(), 0x070700);
    assert.strictEqual(loc.sundialsVersionString(), '7.7.0');

    // The version is a number and the version string is a string.
    // Note: this has no C++ equivalent since C++ is statically typed.

    assert.strictEqual(typeof loc.sundialsVersion(), 'number');
    assert.strictEqual(typeof loc.sundialsVersionString(), 'string');
  });
});
