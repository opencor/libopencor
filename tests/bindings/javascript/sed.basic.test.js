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

test.describe('Sed basic tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('No file', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasIssues, false);
  });

  test('Unknown file', () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(utils.UNKNOWN_CONTENTS);

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [loc.Issue.Type.ERROR, 'A simulation experiment description cannot be created using an unknown file.']
    ]);
  });

  test('CellML file', () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(utils.CELLML_CONTENTS);

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('SED-ML file', () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(utils.SEDML_CONTENTS);

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, true);

    new loc.File(utils.CELLML_FILE);

    document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('SED-ML file with absolute CellML file', () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(utils.SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS);

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, true);

    new loc.File(utils.LOCAL_FILE);

    document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('SED-ML file with remote CellML file', () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(utils.SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS);

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, true);

    new loc.File(utils.REMOTE_FILE);

    document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('COMBINE archive', () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(utils.COMBINE_ARCHIVE_CONTENTS);

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
  });

  test('COMBINE archive with no manifest file', () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(utils.COMBINE_ARCHIVE_WITH_NO_MANIFEST_FILE_CONTENTS);

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with no master file.'
      ]
    ]);
  });

  test('COMBINE archive with no master file', () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(utils.COMBINE_ARCHIVE_WITH_NO_MASTER_FILE_CONTENTS);

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with no master file.'
      ]
    ]);
  });

  test('COMBINE archive with unknown direct CellML file', () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(utils.COMBINE_ARCHIVE_WITH_UNKNOWN_DIRECT_CELLML_FILE_CONTENTS);

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).'
      ]
    ]);
  });

  test('COMBINE archive with unknown indirect CellML file', () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(utils.COMBINE_ARCHIVE_WITH_UNKNOWN_INDIRECT_CELLML_FILE_CONTENTS);

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assertIssues(loc, instance, [[loc.Issue.Type.ERROR, "Task: task 'task1' requires a model of CellML type."]]);
  });

  test('COMBINE archive with unknown SED-ML file', () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(utils.COMBINE_ARCHIVE_WITH_UNKNOWN_SEDML_FILE_CONTENTS);

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        'A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).'
      ]
    ]);
  });
});
