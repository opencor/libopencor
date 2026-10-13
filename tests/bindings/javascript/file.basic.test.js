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
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

    const filePath = utils.resourcePath('file.txt');
    const file = new loc.File(filePath);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, filePath);
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, filePath);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
  });

  test('Non-existing relative local file', () => {
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

    const file = new loc.File('some/relative/../../path/../dir/file.txt');

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, 'dir/file.txt');
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, 'dir/file.txt');
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
  });

  test('Non-existing relative local file with leading parent directories', () => {
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

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
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

    const TOO_LONG_NAME_LENGTH = 5000;

    const file = new loc.File(`/${'a'.repeat(TOO_LONG_NAME_LENGTH)}`);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
  });

  test('Local file with null character', () => {
    // A file name with a null character cannot exist and must never be truncated at that null character (e.g.,
    // "/some/path/file.txt\0.cellml" must not be considered to be "/some/path/file.txt").
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

    const filePath = utils.resourcePath('cellml_2.cellml');
    const fileName = `${filePath}\0.txt`;
    const file = new loc.File(fileName);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, fileName);
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, fileName);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
    assert.strictEqual(new loc.File(filePath).isAliasOf(file), false);
  });

  test('Local file with non-UTF-8 character', () => {
    // A file name that is not valid UTF-8 (e.g., "/some/path/\xFF.cellml", which is what "file:///some/path/%FF.cellml"
    // gets percent-decoded to) must not result in an exception being thrown and must be kept as is.
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

    const fileName = utils.resourcePath('\udcff.cellml');
    const file = new loc.File(fileName);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName.endsWith('\udcff.cellml'), true);
    assert.strictEqual(file.fileName, fileName);
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, fileName);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
    assert.strictEqual(new loc.File(`file://${utils.resourcePath('%FF.cellml')}`).isAliasOf(file), true);

    // A file name with a character that is outside of the Basic Multilingual Plane (e.g., an emoji) is also kept as is.

    const emojiFileName = utils.resourcePath('\u{1f600}.cellml');
    const emojiFile = new loc.File(emojiFileName);

    assert.strictEqual(emojiFile.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(emojiFile.fileName.endsWith('\u{1f600}.cellml'), true);
    assert.strictEqual(emojiFile.fileName, emojiFileName);
    assertIssues(loc, emojiFile, expectedNoIssues);
    assert.strictEqual(new loc.File(`file://${utils.resourcePath('%F0%9F%98%80.cellml')}`).isAliasOf(emojiFile), true);
  });

  test('Local directory', () => {
    // A directory is not a file, but it must not result in an exception being thrown.

    const file = new loc.File(utils.resourcePath('api'));

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);
  });

  test('URL-based local file', () => {
    // Note: the file is of unknown type and has no issues since our JavaScript bindings never retrieve the contents of
    //       a file.

    const filePath = utils.resourcePath('file.txt');
    const file = new loc.File(`file://${filePath}`);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, filePath);
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, filePath);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);

    // A scheme is case-insensitive.

    assert.strictEqual(new loc.File(`FILE://${filePath}`).isAliasOf(file), true);

    // A "file" URL with the "localhost" host or without an authority also refers to a local file while a "file" URL
    // with another host refers to a remote file.

    assert.strictEqual(new loc.File(`file://localhost${filePath}`).isAliasOf(file), true);
    assert.strictEqual(new loc.File(`file:${filePath}`).isAliasOf(file), true);

    const remoteFile = new loc.File(`file://example.com${filePath}`);

    assert.strictEqual(remoteFile.isAliasOf(file), false);
    assert.strictEqual(remoteFile.fileName, '');
    assert.strictEqual(remoteFile.url, `file://example.com${filePath}`);
  });

  test('Local files with percent signs', () => {
    // A local file name is never decoded since a percent sign is a valid character in a file name.
    // Note: the file names are not canonicalised against the local file system since our JavaScript bindings use a
    //       virtual file system.

    const dir = path.join(os.tmpdir(), 'libopencor_local_files_with_percent_signs');
    const cellmlContents = utils.fileContents(utils.resourcePath('cellml_2.cellml'));
    const notAModel = new TextEncoder().encode('not a model');

    fs.mkdirSync(dir, { recursive: true });
    fs.writeFileSync(path.join(dir, 'a%20model.cellml'), cellmlContents);
    fs.writeFileSync(path.join(dir, 'b%20c.cellml'), cellmlContents);
    fs.writeFileSync(path.join(dir, 'b c.cellml'), notAModel);

    const a = new loc.File(path.join(dir, 'a%20model.cellml'));
    const b = new loc.File(path.join(dir, 'b%20c.cellml'));
    const bc = new loc.File(path.join(dir, 'b c.cellml'));

    a.setContents(utils.fileContents(a.fileName));
    b.setContents(utils.fileContents(b.fileName));
    bc.setContents(utils.fileContents(bc.fileName));

    assert.strictEqual(a.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(a.fileName, path.join(dir, 'a%20model.cellml'));
    assertIssues(loc, a, expectedNoIssues);
    assert.strictEqual(b.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(b.fileName, path.join(dir, 'b%20c.cellml'));
    assert.deepStrictEqual(b.contents(), cellmlContents);
    assertIssues(loc, b, expectedNoIssues);
    assert.strictEqual(bc.isAliasOf(b), false);
    assert.strictEqual(bc.fileName, path.join(dir, 'b c.cellml'));
    assert.deepStrictEqual(bc.contents(), notAModel);

    // The path of a "file" URL is, however, decoded.

    const dirUrl = `file://${dir}`;

    assert.strictEqual(new loc.File(`${dirUrl}/b%2520c.cellml`).isAliasOf(b), true);
    assert.strictEqual(new loc.File(`${dirUrl}/b%20c.cellml`).isAliasOf(bc), true);
    assert.strictEqual(loc.FileManager.instance().fileFromFileNameOrUrl(`${dirUrl}/b%2520c.cellml`).isAliasOf(b), true);
    assert.strictEqual(loc.FileManager.instance().fileFromFileNameOrUrl(`${dirUrl}/b%20c.cellml`).isAliasOf(bc), true);

    fs.rmSync(dir, { recursive: true, force: true });
  });

  test('Remote files with percent-encoded characters', () => {
    // Percent-encoded characters are decoded, unless they have a meaning in a URL, in which case the URL would refer to
    // a different file.

    const file = new loc.File('https://example.com/my%20file.cellml');

    assert.strictEqual(file.url, 'https://example.com/my file.cellml');
    assert.strictEqual(new loc.File('https://example.com/my file.cellml').isAliasOf(file), true);
    assert.strictEqual(
      loc.FileManager.instance().fileFromFileNameOrUrl('https://example.com/my%20file.cellml').isAliasOf(file),
      true
    );
    assert.strictEqual(
      loc.FileManager.instance().fileFromFileNameOrUrl('https://example.com/my file.cellml').isAliasOf(file),
      true
    );

    for (const [encodedUrl, otherUrl] of [
      ['https://example.com/a%2Fb.cellml', 'https://example.com/a/b.cellml'],
      ['https://example.com/x/..%2Fy.cellml', 'https://example.com/y.cellml'],
      ['https://example.com/a%3Fb.cellml', 'https://example.com/a?b.cellml'],
      ['https://example.com/a%23b.cellml', 'https://example.com/a#b.cellml'],
      ['https://example.com/a%2520b.cellml', 'https://example.com/a%20b.cellml'],
      ['https://example.com/a%5Cb.cellml', 'https://example.com/a/b.cellml']
    ]) {
      const encodedFile = new loc.File(encodedUrl);
      const otherFile = new loc.File(otherUrl);

      assert.strictEqual(encodedFile.isAliasOf(otherFile), false);
      assert.strictEqual(encodedFile.url, encodedUrl);
      assert.strictEqual(new loc.File(encodedFile.url).isAliasOf(encodedFile), true);
    }

    // A percent sign that doesn't start a percent-encoded character is percent-encoded.

    const percentFile = new loc.File('https://example.com/100%.cellml');

    assert.strictEqual(percentFile.url, 'https://example.com/100%25.cellml');
    assert.strictEqual(new loc.File('https://example.com/100%25.cellml').isAliasOf(percentFile), true);
    assert.strictEqual(new loc.File(percentFile.url).isAliasOf(percentFile), true);

    // A percent-encoded character that is not valid UTF-8 is also decoded.

    const nonUtf8File = new loc.File('https://example.com/%FF.cellml');

    assert.strictEqual(nonUtf8File.url, 'https://example.com/\udcff.cellml');
    assert.strictEqual(new loc.File(nonUtf8File.url).isAliasOf(nonUtf8File), true);

    // A percent-encoded character that is outside of the Basic Multilingual Plane (e.g., an emoji) is also decoded.

    const emojiFile = new loc.File('https://example.com/%F0%9F%98%80.cellml');

    assert.strictEqual(emojiFile.url, 'https://example.com/\u{1f600}.cellml');
    assert.strictEqual(new loc.File(emojiFile.url).isAliasOf(emojiFile), true);

    // A string with a lone surrogate that doesn't stand for an invalid byte (i.e. other than U+DC80 to U+DCFF) is
    // rejected.
    // Note: this has no C++ equivalent since a std::string cannot contain a lone surrogate.

    assert.throws(() => new loc.File('https://example.com/\ud800.cellml'), TypeError);
    assert.throws(() => new loc.File('https://example.com/\udc41.cellml'), TypeError);
  });

  test('Remote file', () => {
    // Note: the file is of unknown type and has no file name and no contents since our JavaScript bindings never
    //       retrieve the contents of a file, so a remote file is never downloaded.

    const file = new loc.File(utils.REMOTE_FILE);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, '');
    assert.strictEqual(file.url, utils.REMOTE_FILE);
    assert.strictEqual(file.path, utils.REMOTE_FILE);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
  });

  test('Encoded remote file', () => {
    // Note: the file is of unknown type and has no file name and no contents since our JavaScript bindings never
    //       retrieve the contents of a file, so a remote file is never downloaded.

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
  });

  test('Remote file with dot segments', () => {
    const file = new loc.File('https://example.com/a/./b/../../c/model.cellml');

    assert.strictEqual(file.url, 'https://example.com/c/model.cellml');
    assert.strictEqual(file.path, 'https://example.com/c/model.cellml');
  });

  test('Remote file with upper-case scheme', () => {
    // A scheme is case-insensitive, so it gets normalised to lowercase.

    const file = new loc.File('HTTPS://example.com/model.cellml');

    assert.strictEqual(file.url, 'https://example.com/model.cellml');
    assert.strictEqual(file.path, 'https://example.com/model.cellml');
    assert.strictEqual(new loc.File('https://example.com/model.cellml').isAliasOf(file), true);
    assert.strictEqual(new loc.File('Https://example.com/model.cellml').isAliasOf(file), true);
    assert.strictEqual(
      loc.FileManager.instance().fileFromFileNameOrUrl('HTTPS://example.com/model.cellml').isAliasOf(file),
      true
    );
  });

  test('Remote file with upper-case host', () => {
    // A host is case-insensitive, so it gets normalised to lowercase, unlike a path which is case-sensitive.

    const file = new loc.File('https://EXAMPLE.com/model.cellml');

    assert.strictEqual(file.url, 'https://example.com/model.cellml');
    assert.strictEqual(file.path, 'https://example.com/model.cellml');
    assert.strictEqual(new loc.File('https://example.com/model.cellml').isAliasOf(file), true);
    assert.strictEqual(new loc.File('https://Example.COM/model.cellml').isAliasOf(file), true);
    assert.strictEqual(
      loc.FileManager.instance().fileFromFileNameOrUrl('https://EXAMPLE.com/model.cellml').isAliasOf(file),
      true
    );
    assert.strictEqual(new loc.File('https://example.com/MODEL.cellml').isAliasOf(file), false);
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

  test('Local virtual file', () => {
    const filePath = utils.resourcePath('unknown_file.txt');
    const file = new loc.File(filePath);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, filePath);
    assert.strictEqual(file.url, '');
    assert.strictEqual(file.path, filePath);
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);

    const someUnknownContents = utils.fileContents(filePath);

    file.setContents(someUnknownContents);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), someUnknownContents);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('Remote virtual file', () => {
    const file = new loc.File('https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt');

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.strictEqual(file.fileName, '');
    assert.strictEqual(
      file.url,
      'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt'
    );
    assert.strictEqual(
      file.path,
      'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt'
    );
    assert.deepStrictEqual(file.contents(), Uint8Array.from([]));
    assertIssues(loc, file, expectedNoIssues);

    const filePath = utils.resourcePath('unknown_file.txt');
    const someUnknownContents = utils.fileContents(filePath);

    file.setContents(someUnknownContents);

    assert.strictEqual(file.type.value, loc.File.Type.UNKNOWN_FILE.value);
    assert.deepStrictEqual(file.contents(), someUnknownContents);
    assertIssues(loc, file, expectedUnknownFileIssues);
  });

  test('Remote virtual file matching local file', () => {
    // A remote file that has no local copy has an empty file name, so it must never be confused with a local file that
    // has an empty file name.

    const fileManager = loc.FileManager.instance();
    const remoteFile = new loc.File(utils.REMOTE_FILE);
    const localFile = new loc.File('');

    assert.strictEqual(localFile.isAliasOf(remoteFile), false);
    assert.strictEqual(remoteFile.fileName, '');
    assert.strictEqual(localFile.fileName, '');
    assert.strictEqual(localFile.url, '');
    assert.strictEqual(fileManager.fileFromFileNameOrUrl('').isAliasOf(localFile), true);
    assert.strictEqual(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE).isAliasOf(remoteFile), true);
  });

  test('File manager', () => {
    const fileManager = loc.FileManager.instance();
    const filePath = utils.resourcePath('file.txt');

    assert.strictEqual(fileManager.hasFiles, false);
    assert.strictEqual(fileManager.fileCount, 0);
    assert.strictEqual(fileManager.files.length, 0);
    assert.strictEqual(fileManager.file(0), null);
    assert.strictEqual(fileManager.fileFromFileNameOrUrl(filePath), null);

    const localFile = new loc.File(filePath);
    const sameFileManager = loc.FileManager.instance();

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 1);
    assert.strictEqual(sameFileManager.files.length, 1);
    assert.strictEqual(fileManager.file(0).isAliasOf(localFile), true);
    assert.strictEqual(sameFileManager.fileFromFileNameOrUrl(filePath).isAliasOf(localFile), true);

    const remoteFile = new loc.File(utils.REMOTE_FILE);

    assert.strictEqual(fileManager.hasFiles, true);
    assert.strictEqual(fileManager.fileCount, 2);
    assert.strictEqual(fileManager.files.length, 2);
    assert.strictEqual(fileManager.file(1).isAliasOf(remoteFile), true);
    assert.strictEqual(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE).isAliasOf(remoteFile), true);

    sameFileManager.unmanage(localFile);

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 1);
    assert.strictEqual(sameFileManager.files.length, 1);
    assert.strictEqual(fileManager.file(1), null);
    assert.strictEqual(sameFileManager.fileFromFileNameOrUrl(filePath), null);

    sameFileManager.manage(localFile);

    assert.strictEqual(sameFileManager.hasFiles, true);
    assert.strictEqual(sameFileManager.fileCount, 2);
    assert.strictEqual(sameFileManager.files.length, 2);
    assert.strictEqual(fileManager.file(1).isAliasOf(localFile), true);
    assert.strictEqual(sameFileManager.fileFromFileNameOrUrl(filePath).isAliasOf(localFile), true);

    fileManager.reset();

    assert.strictEqual(fileManager.hasFiles, false);
    assert.strictEqual(fileManager.fileCount, 0);
    assert.strictEqual(fileManager.files.length, 0);
    assert.strictEqual(fileManager.file(0), null);
    assert.strictEqual(fileManager.file(1), null);
    assert.strictEqual(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE), null);
    assert.strictEqual(fileManager.fileFromFileNameOrUrl(utils.resourcePath('unknown_file.txt')), null);
  });
});
