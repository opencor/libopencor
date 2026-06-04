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

const expectedNoIssues = [];
const expectedUnknownFileIssues = [
  [loc.Issue.Type.ERROR, 'The file is not a CellML file, a SED-ML file, or a COMBINE archive.']
];

test.describe('File basic tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Local file', () => {
    const fileName = utils.resourcePath('unknown_file.txt');
    const file = new loc.File(fileName);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, fileName);
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, fileName);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), utils.fileContents(file.path));
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('Remote file', () => {
    const file = new loc.File(utils.REMOTE_FILE);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, '/some/path/file');
    assert.strictEqual(file.url, utils.REMOTE_FILE);
    assert.strictEqual(file.path, utils.REMOTE_FILE);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);

    const fileContents = utils.fileContents(utils.resourcePath('unknown_file.txt'));

    file.setContents(fileContents);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), fileContents);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('Encoded remote file', () => {
    const file = new loc.File(
      'https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO%20BG%20example%203.1.cellml'
    );

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, '/some/path/file');
    assert.strictEqual(
      file.url,
      'https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml'
    );
    assert.strictEqual(
      file.path,
      'https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml'
    );
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);

    const fileContents = utils.fileContents(utils.resourcePath('unknown_file.txt'));

    file.setContents(fileContents);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), fileContents);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('File manager', () => {
    const fileManager = loc.FileManager.instance();
    const fileName = utils.resourcePath('file.txt');

    assert.strictEqual(fileManager.hasFiles, false);
    assert.strictEqual(fileManager.fileCount, 0);
    assert.strictEqual(fileManager.files.length, 0);
    assert.strictEqual(fileManager.file(0), null);
    assert.strictEqual(fileManager.fileFromFileNameOrUrl(fileName), null);

    const localFile = new loc.File(fileName);
    const sameFileManager = loc.FileManager.instance();

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 1);
    assert.strictEqual(sameFileManager.files.length, 1);
    assert.deepStrictEqual(fileManager.file(0), localFile);
    assert.deepStrictEqual(sameFileManager.fileFromFileNameOrUrl(fileName), localFile);

    const remoteFile = new loc.File(utils.REMOTE_FILE);

    assert.strictEqual(fileManager.hasFiles, true);
    assert.strictEqual(fileManager.fileCount, 2);
    assert.strictEqual(fileManager.files.length, 2);
    assert.deepStrictEqual(fileManager.file(1), remoteFile);
    assert.deepStrictEqual(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE), remoteFile);

    sameFileManager.unmanage(localFile);

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 1);
    assert.strictEqual(sameFileManager.files.length, 1);
    assert.deepStrictEqual(fileManager.file(1), null);
    assert.deepStrictEqual(sameFileManager.fileFromFileNameOrUrl(fileName), null);

    sameFileManager.manage(localFile);

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 2);
    assert.strictEqual(sameFileManager.files.length, 2);
    assert.deepStrictEqual(fileManager.file(1), localFile);
    assert.deepStrictEqual(sameFileManager.fileFromFileNameOrUrl(fileName), localFile);

    fileManager.reset();

    assert.strictEqual(fileManager.hasFiles, false);
    assert.strictEqual(fileManager.fileCount, 0);
    assert.strictEqual(fileManager.files.length, 0);
    assert.deepStrictEqual(fileManager.file(0), null);
    assert.deepStrictEqual(fileManager.file(1), null);
    assert.deepStrictEqual(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE), null);
    assert.deepStrictEqual(fileManager.fileFromFileNameOrUrl(utils.resourcePath('unknown_file.txt')), null);
  });
});
