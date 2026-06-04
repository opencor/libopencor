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
import test from 'node:test';

import libOpenCOR from './libopencor.js';
import * as utils from './utils.js';

const loc = await libOpenCOR();

test.describe('File coverage tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Empty file', () => {
    const file = new loc.File(utils.resourcePath('unknown_file.txt'));

    file.setContents(null);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
  });

  test('File with null character', () => {
    const file = new loc.File(utils.resourcePath('unknown_file.txt'));

    file.setContents(Uint8Array.from([0]));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
  });

  test('SED-ML file with no parent', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.sedml'));

    file.setContents(utils.fileContents(file.path));
  });

  test('Same local file', () => {
    const filePath = utils.resourcePath('file.txt');
    const file1 = new loc.File(filePath);
    const file2 = new loc.File(filePath);

    assert.deepStrictEqual(file1, file2);
  });

  test('Same remote file', () => {
    const file1 = new loc.File(utils.REMOTE_FILE);
    const file2 = new loc.File(utils.REMOTE_FILE);

    assert.deepStrictEqual(file1, file2);
  });

  test('Unmanage file with children', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.omex'));
    const fileManager = loc.FileManager.instance();

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(fileManager.fileCount, 3);

    fileManager.unmanage(file);

    assert.strictEqual(fileManager.fileCount, 0);
  });
});
