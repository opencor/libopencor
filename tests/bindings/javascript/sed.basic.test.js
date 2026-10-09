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

function sedmlContents(modelSource) {
  return new TextEncoder().encode(`<?xml version='1.0' encoding='UTF-8'?>
<sedML level="1" version="3" xmlns="http://sed-ml.org/sed-ml/level1/version3">
    <listOfModels>
        <model id="model" language="urn:sedml:language:cellml" source="${modelSource}"/>
    </listOfModels>
</sedML>
`);
}

test.describe('Sed basic tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('No file', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasIssues, false);
  });

  test('Unknown file', () => {
    const file = new loc.File(utils.resourcePath('unknown_file.txt'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [loc.Issue.Type.ERROR, 'A simulation experiment description cannot be created using an unknown file.']
    ]);
  });

  test('CellML file', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('SED-ML file', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.sedml'));

    file.setContents(utils.fileContents(file.path));

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, true);

    new loc.File(utils.resourcePath('cellml_2.cellml'));

    document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('SED-ML file with absolute CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/absolute_cellml_file.sedml'));

    file.setContents(utils.fileContents(file.path));

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, true);

    new loc.File(utils.resourcePath('file.txt'));

    document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('SED-ML file with relative CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/relative_cellml_file.sedml'));

    file.setContents(sedmlContents('../../cellml_2.cellml'));

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.path, utils.resourcePath('cellml_2.cellml'));

    const neededFile = new loc.File(utils.resourcePath('cellml_2.cellml'));

    document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('SED-ML file with relative CellML file in working directory', () => {
    // A relative model source is relative to the SED-ML file, not to the current working directory, even if the
    // current working directory contains a file with that name.

    const origDir = process.cwd();

    process.chdir(utils.resourcePath());

    const file = new loc.File(utils.resourcePath('api/sed/relative_cellml_file.sedml'));

    file.setContents(sedmlContents('cellml_2.cellml'));

    const document = new loc.SedDocument(file);

    process.chdir(origDir);

    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.path, utils.resourcePath('api/sed/cellml_2.cellml'));
  });

  test('SED-ML file with remote CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/remote_cellml_file.sedml'));

    file.setContents(utils.fileContents(file.path));

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, true);

    new loc.File(utils.REMOTE_FILE);

    document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('COMBINE archive', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('COMBINE archive with no manifest file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/no_manifest_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with no master file.'
      ]
    ]);
  });

  test('COMBINE archive with no master file and one CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_one_cellml_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.simulations.get(0).outputEndTime, 1000);

    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('COMBINE archive with no master file and one unknown CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_one_unknown_cellml_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).'
      ]
    ]);
  });

  test('COMBINE archive with no master file and several CellML files', () => {
    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_several_cellml_files.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with no master file, no SED-ML file, and several CellML files.'
      ]
    ]);
  });

  test('COMBINE archive with no master file and one SED-ML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_one_sedml_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.simulations.get(0).outputEndTime, 50);

    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('COMBINE archive with no master file and one unknown SED-ML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_one_unknown_sedml_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).'
      ]
    ]);
  });

  test('COMBINE archive with no master file and several SED-ML files', () => {
    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_several_sedml_files.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with no master file and several SED-ML files.'
      ]
    ]);
  });

  test('COMBINE archive with unknown direct CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/unknown_direct_cellml_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).'
      ]
    ]);
  });

  test('COMBINE archive with unknown indirect CellML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/unknown_indirect_cellml_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assertIssues(loc, instance, [[loc.Issue.Type.ERROR, "Task: task 'task1' requires a model of CellML type."]]);
  });

  test('COMBINE archive with unknown SED-ML file', () => {
    const file = new loc.File(utils.resourcePath('api/sed/unknown_sedml_file.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).'
      ]
    ]);
  });
});
