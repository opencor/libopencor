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
import { assertIssues } from './utils.js';

const loc = await libOpenCOR();

const expectedUnknownFileIssues = [
  [loc.Issue.Type.ERROR, 'The file is not a CellML file, a SED-ML file, or a COMBINE archive.']
];

test.describe('File type tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Irretrievable file', () => {
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

    const file = new loc.File(utils.resourcePath('irretrievable_file.txt'));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assertIssues(loc, file, []);
  });

  test('Unknown file', () => {
    const file = new loc.File(utils.resourcePath('unknown_file.txt'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('SBML file', () => {
    const file = new loc.File(utils.resourcePath('sbml.sbml'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('Error SED-ML file', () => {
    const file = new loc.File(utils.resourcePath('error.sedml'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.SEDML_FILE.value);
  });

  test('CellML 1.x file', () => {
    const file = new loc.File(utils.resourcePath('cellml_1_x.cellml'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.CELLML_FILE.value);
  });

  test('SED-ML 1.x file', () => {
    const file = new loc.File(utils.resourcePath('cellml_1_x.sedml'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.SEDML_FILE.value);
  });

  test('COMBINE 1.x archive', () => {
    const file = new loc.File(utils.resourcePath('cellml_1_x.omex'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });

  test('CellML 2 file', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.CELLML_FILE.value);
  });

  test('SED-ML 2 file', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.sedml'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.SEDML_FILE.value);
  });

  test('COMBINE 2 archive', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.omex'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });

  test('Unknown virtual file', () => {
    const filePath = utils.resourcePath('unknown_file.txt');
    const file = new loc.File(filePath);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));

    file.setContents(utils.fileContents(filePath));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('CellML virtual file', () => {
    const filePath = utils.resourcePath('cellml_2.cellml');
    const file = new loc.File(filePath);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));

    file.setContents(utils.fileContents(filePath));

    assert.strictEqual(file.type.value, loc.File.Type.CELLML_FILE.value);
  });

  test('SED-ML virtual file', () => {
    const filePath = utils.resourcePath('cellml_2.sedml');
    const file = new loc.File(filePath);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));

    file.setContents(utils.fileContents(filePath));

    assert.strictEqual(file.type.value, loc.File.Type.SEDML_FILE.value);
  });

  test('COMBINE virtual archive', () => {
    const filePath = utils.resourcePath('cellml_2.omex');
    const file = new loc.File(filePath);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));

    file.setContents(utils.fileContents(filePath));

    assert.strictEqual(file.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });
});
