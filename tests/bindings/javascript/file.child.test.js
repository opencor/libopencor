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

test.describe('File child tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  function doTestDataset(number, specificChildFileNames) {
    const file = new loc.File(utils.resourcePath(`api/file/dataset_${number}.omex`));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.hasChildFiles, true);
    assert.strictEqual(file.childFileCount, specificChildFileNames.length + 1);
    assert.strictEqual(file.childFileNames.length, specificChildFileNames.length + 1);
    assert.strictEqual(file.childFiles.length, specificChildFileNames.length + 1);

    let index = -1;
    const simulationFile = file.childFileFromFileName('simulation.json');

    assert.notStrictEqual(file.childFile(++index), null);
    assert.notStrictEqual(simulationFile, null);

    for (let i = 0; i < specificChildFileNames.length; ++i) {
      assert.notStrictEqual(file.childFile(++index), null);
      assert.notStrictEqual(file.childFileFromFileName(specificChildFileNames[i]), null);
    }

    assert.strictEqual(file.childFile(++index), null);
    assert.strictEqual(file.childFileFromFileName(utils.resourcePath('unknown_file.txt')), null);

    assert.deepStrictEqual(
      simulationFile.contents(),
      utils.fileContents(utils.resourcePath(`api/file/dataset_${number}.json`))
    );
  }

  test('No child files', () => {
    const filePath = utils.resourcePath('unknown_file.txt');
    const file = new loc.File(filePath);

    assert.strictEqual(file.hasChildFiles, false);
    assert.strictEqual(file.childFileCount, 0);
    assert.strictEqual(file.childFileNames.length, 0);
    assert.strictEqual(file.childFiles.length, 0);
    assert.strictEqual(file.childFile(0), null);
    assert.strictEqual(file.childFileFromFileName(filePath), null);
  });

  test('Dataset 135', () => {
    doTestDataset(135, ['HumanSAN_Fabbri_Fantini_Wilders_Severi_2017.cellml']);
  });

  test('Dataset 157', () => {
    doTestDataset(157, [
      'fabbri_et_al_based_composite_SAN_model.cellml',
      'fabbri_et_al_based_composite_SAN_model.sedml'
    ]);
  });

  test('Remote virtual COMBINE archives', () => {
    // Two remote COMBINE archives that are not downloaded must not share their child files.

    const file135 = new loc.File(`${utils.REMOTE_BASE_PATH}/api/file/dataset_135.omex`);
    const file157 = new loc.File(`${utils.REMOTE_BASE_PATH}/api/file/dataset_157.omex`);

    file135.setContents(utils.fileContents(utils.resourcePath('api/file/dataset_135.omex')));
    file157.setContents(utils.fileContents(utils.resourcePath('api/file/dataset_157.omex')));

    assert.strictEqual(file135.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
    assert.strictEqual(file157.type.value, loc.File.Type.COMBINE_ARCHIVE.value);

    const simulationFile135 = file135.childFileFromFileName('simulation.json');
    const simulationFile157 = file157.childFileFromFileName('simulation.json');

    assert.notStrictEqual(simulationFile135, null);
    assert.notStrictEqual(simulationFile157, null);
    assert.strictEqual(simulationFile135.isAliasOf(simulationFile157), false);
    assert.deepStrictEqual(
      simulationFile135.contents(),
      utils.fileContents(utils.resourcePath('api/file/dataset_135.json'))
    );
    assert.deepStrictEqual(
      simulationFile157.contents(),
      utils.fileContents(utils.resourcePath('api/file/dataset_157.json'))
    );
  });

  test('COMBINE archive replaced with other contents', () => {
    // Replacing the contents of a COMBINE archive with some other contents must release its child files.

    const file = new loc.File(utils.resourcePath('cellml_2.omex'));
    const fileManager = loc.FileManager.instance();

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(fileManager.fileCount, 3);

    file.setContents(utils.fileContents(utils.resourcePath('cellml_2.cellml')));

    assert.strictEqual(file.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(file.hasChildFiles, false);
    assert.strictEqual(fileManager.fileCount, 1);
  });

  test('COMBINE archive with files outside of it', () => {
    // A COMBINE archive may reference files that are located outside of it (e.g., "../sibling.cellml" or even the
    // COMBINE archive itself), in which case they must be ignored rather than replace the contents of another managed
    // file (or result in an infinite recursion).

    const expectedIssues = [
      [
        loc.Issue.Type.WARNING,
        "COMBINE archive: the file '../sibling.cellml' is not located within the COMBINE archive. It has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "COMBINE archive: the file '%2E%2E/encoded_sibling.cellml' is not located within the COMBINE archive. It has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "COMBINE archive: the file '../entries_outside_of_archive.omex' is not located within the COMBINE archive. It has been ignored."
      ]
    ];
    const sibling = new loc.File(utils.resourcePath('api/file/sibling.cellml'));
    const someContents = new TextEncoder().encode('Some contents.');

    sibling.setContents(someContents);

    const file = new loc.File(utils.resourcePath('api/file/entries_outside_of_archive.omex'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
    assert.strictEqual(file.childFileCount, 1);
    assert.notStrictEqual(file.childFileFromFileName('model.cellml'), null);
    utils.assertIssues(loc, file, expectedIssues);
    assert.deepStrictEqual(sibling.contents(), someContents);
    assert.strictEqual(loc.FileManager.instance().fileCount, 3);
  });

  test('COMBINE archive with percent-encoded locations', () => {
    // A location is a URI reference, so it may be percent-encoded (e.g., "./my%20file.txt"), in which case the name of
    // its ZIP entry should be its decoded location (e.g., "my file.txt"), although some tools use the location itself
    // (e.g., "other%20file.txt"). Either way, a child file must have the contents of its ZIP entry and be retrievable
    // using either its location or its file name.
    // Note: the ZIP entry for "./my%2520file.txt" is "my%20file.txt", so it must not be used for "./my%20file.txt".

    const locationsAndFileNames = [
      ['./file.txt', 'file.txt'],
      ['./my%20file.txt', 'my file.txt'],
      ['./my%2520file.txt', 'my%20file.txt'],
      ['./other%20file.txt', 'other file.txt']
    ];
    const file = new loc.File(utils.resourcePath('api/file/percent_encoded_locations.omex'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
    assert.strictEqual(file.hasIssues, false);
    assert.strictEqual(file.childFileCount, locationsAndFileNames.length);

    for (const [i, [location, fileName]] of locationsAndFileNames.entries()) {
      const childFile = file.childFileFromFileName(location);

      assert.notStrictEqual(childFile, null);
      assert.strictEqual(file.childFileNames.get(i), location);
      assert.strictEqual(file.childFile(i).isAliasOf(childFile), true);
      assert.strictEqual(file.childFileFromFileName(fileName).isAliasOf(childFile), true);
      assert.deepStrictEqual(childFile.contents(), new TextEncoder().encode(fileName));
    }
  });

  test('COMBINE archive with duplicate locations', () => {
    // Different locations may refer to the same file (e.g., "./my%20file.txt" and "./my file.txt", or "./file.txt" and
    // "file.txt"), in which case only the first one is kept and the other ones are ignored. The ignored locations can,
    // however, still be used to retrieve the child file since they refer to the same file.

    const expectedIssues = [
      [
        loc.Issue.Type.WARNING,
        "COMBINE archive: the file 'file.txt' is the same as the file './file.txt'. It has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "COMBINE archive: the file './my file.txt' is the same as the file './my%20file.txt'. It has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "COMBINE archive: the file './other%20file.txt' is the same as the file './other file.txt'. It has been ignored."
      ]
    ];
    const locationsFileNamesAndDuplicateLocations = [
      ['./file.txt', 'file.txt', 'file.txt'],
      ['./my%20file.txt', 'my file.txt', './my file.txt'],
      ['./other file.txt', 'other file.txt', './other%20file.txt']
    ];
    const file = new loc.File(utils.resourcePath('api/file/duplicate_locations.omex'));

    file.setContents(utils.fileContents(file.path));

    assert.strictEqual(file.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
    utils.assertIssues(loc, file, expectedIssues);
    assert.strictEqual(file.childFileCount, locationsFileNamesAndDuplicateLocations.length);

    for (const [i, [location, fileName, duplicateLocation]] of locationsFileNamesAndDuplicateLocations.entries()) {
      const childFile = file.childFileFromFileName(location);

      assert.notStrictEqual(childFile, null);
      assert.strictEqual(file.childFileNames.get(i), location);
      assert.strictEqual(file.childFile(i).isAliasOf(childFile), true);
      assert.strictEqual(file.childFileFromFileName(fileName).isAliasOf(childFile), true);
      assert.strictEqual(file.childFileFromFileName(duplicateLocation).isAliasOf(childFile), true);
      assert.deepStrictEqual(childFile.contents(), new TextEncoder().encode(fileName));
    }

    assert.strictEqual(loc.FileManager.instance().fileCount, 4);
  });
});
