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

test.describe('File type tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  function doTestDataset(number, specificChildFileNames) {
    const file = new loc.File(utils.resourcePath('cellml_2.omex'));

    file.setContents(utils.fileContents(utils.resourcePath(`api/file/dataset_${number}.omex`)));

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
});
