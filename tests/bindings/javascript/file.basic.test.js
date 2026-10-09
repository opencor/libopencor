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
import os from 'node:os';
import path from 'node:path';
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

  test('Non-existing relative local file with leading parent directories', () => {
    const file = new loc.File('../models/./lorenz.cellml');

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, '../models/lorenz.cellml');
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, '../models/lorenz.cellml');
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
  });

  test('Too long local file name', () => {
    // A file name that is too long for the file system must not result in an exception being thrown.

    const TOO_LONG_NAME_LENGTH = 5000;

    const file = new loc.File(`/${'a'.repeat(TOO_LONG_NAME_LENGTH)}`);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
  });

  test('Local directory', () => {
    // A directory is not a file, but it must not result in an exception being thrown.

    const file = new loc.File(utils.resourcePath('api'));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
  });

  test('Remote file', () => {
    const file = new loc.File(utils.REMOTE_FILE);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, '');
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
    assert.strictEqual(file.fileName, '');
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

  test('Remote file with dot segments', () => {
    const file = new loc.File('https://example.com/a/./b/../../c/model.cellml');

    assert.strictEqual(file.url, 'https://example.com/c/model.cellml');
    assert.strictEqual(file.path, 'https://example.com/c/model.cellml');
  });

  test('Remote file matching local file', () => {
    // The host and path of a URL must never be resolved as a local file, even if such a local file exists.

    const origDir = process.cwd();
    const tempDir = path.join(os.tmpdir(), 'libopencor_remote_file_matching_local_file');

    fs.mkdirSync(path.join(tempDir, 'example.com'), { recursive: true });
    fs.writeFileSync(path.join(tempDir, 'example.com', 'model.cellml'), '');

    process.chdir(tempDir);

    const url = new loc.File('https://example.com/model.cellml').url;

    process.chdir(origDir);
    fs.rmSync(tempDir, { recursive: true, force: true });

    assert.strictEqual(url, 'https://example.com/model.cellml');
  });

  test('Concurrent file creations', () => {
    // Creating a file must never change the current working directory since it is shared by all the threads of the
    // process. Also, the file manager must cope with files that get looked up while they are being destroyed.
    // Note: JavaScript code cannot call libOpenCOR from several threads at once, so we interleave what the two
    //       file-creating threads and the checking thread do in the C++ version of this test. We also delete our
    //       handles as soon as we are done with them so that, like in the C++ version of this test, files get destroyed
    //       as soon as they are not needed anymore.

    const FILE_CREATION_COUNT = 1000;

    const origDir = process.cwd();
    const fileManager = loc.FileManager.instance();
    let workingDirectoryChanged = false;

    const check = () => {
      if (process.cwd() !== origDir) {
        workingDirectoryChanged = true;
      }

      fileManager.fileFromFileNameOrUrl('https://example.com/model.cellml')?.delete();

      for (const file of fileManager.files) {
        file.delete();
      }
    };
    const createFiles = () => {
      new loc.File('https://example.com/model.cellml').delete();
      check();
      new loc.File('non_existing_dir/../non_existing_file.txt').delete();
      check();
    };

    for (let i = 0; i < FILE_CREATION_COUNT; ++i) {
      createFiles();
      createFiles();
    }

    assert.strictEqual(workingDirectoryChanged, false);
    assert.strictEqual(process.cwd(), origDir);
  });

  test('Remote virtual file matching local file', () => {
    // A remote file that has no local copy has an empty file name, so it must never be confused with a local file that
    // has an empty file name.

    const fileManager = loc.FileManager.instance();
    const remoteFile = new loc.File(utils.REMOTE_FILE);
    const localFile = new loc.File('');

    assert.strictEqual(fileManager.fileCount, 2);
    assert.strictEqual(remoteFile.fileName, '');
    assert.strictEqual(localFile.fileName, '');
    assert.strictEqual(localFile.url, '');
    assert.strictEqual(fileManager.fileFromFileNameOrUrl('').url, '');
    assert.strictEqual(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE).url, utils.REMOTE_FILE);
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
