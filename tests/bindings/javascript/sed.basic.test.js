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
    let document = new loc.SedDocument();

    assert.strictEqual(document.hasIssues, false);

    document = new loc.SedDocument(null);

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
    // A relative model source is relative to the SED-ML file and its leading ".." components must be kept.

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

  test('SED-ML file with unsupported scheme CellML file', () => {
    // A model source with a scheme other than "http", "https", or "file" is an absolute URI that we cannot retrieve, so
    // its model is ignored rather than considered to be relative to the SED-ML file.

    const file = new loc.File(utils.resourcePath('api/sed/unsupported_scheme_cellml_file.sedml'));

    file.setContents(sedmlContents('urn:miriam:biomodels.db:BIOMD0000000012'));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the model 'urn:miriam:biomodels.db:BIOMD0000000012' has an unsupported scheme. It has been ignored."
      ]
    ]);
    assert.strictEqual(document.models.length, 0);
  });

  test('SED-ML file with empty source CellML file', () => {
    // An empty model source refers to the SED-ML file itself (see
    // https://www.rfc-editor.org/rfc/rfc3986#section-5.2.2), but libSEDML considers it to be an error, so the file is
    // not considered to be a SED-ML file (and its model source never gets resolved).

    const file = new loc.File(utils.resourcePath('api/sed/empty_source_cellml_file.sedml'));

    file.setContents(sedmlContents(''));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [loc.Issue.Type.ERROR, 'A simulation experiment description cannot be created using an unknown file.']
    ]);
    assert.strictEqual(document.models.length, 0);
  });

  test('SED-ML file with file URL CellML file', () => {
    // A "file" URL model source refers to a local file if it has no host (e.g., "file:///some/path/model.cellml" or
    // "file:/some/path/model.cellml") or the "localhost" host (e.g., "file://localhost/some/path/model.cellml"), and to
    // a remote file otherwise (e.g., "file://example.com/some/path/model.cellml").

    const urlPath = utils.resourcePath('cellml_2.cellml');
    const neededFile = new loc.File(utils.resourcePath('cellml_2.cellml'));

    neededFile.setContents(utils.fileContents(neededFile.path));

    for (const source of [`file://${urlPath}`, `file:${urlPath}`, `file://localhost${urlPath}`]) {
      const file = new loc.File(utils.resourcePath('api/sed/file_url_cellml_file.sedml'));

      file.setContents(sedmlContents(source));

      const document = new loc.SedDocument(file);

      assert.strictEqual(document.hasIssues, false);
      assert.strictEqual(document.models.length, 1);
      assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
    }

    const file = new loc.File(utils.resourcePath('api/sed/file_url_cellml_file.sedml'));

    file.setContents(sedmlContents(`file://example.com${urlPath}`));

    const neededRemoteFile = new loc.File(`file://example.com${urlPath}`);
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededRemoteFile), true);
  });

  test('SED-ML file with query and fragment CellML file', () => {
    // The query and fragment of a model source are ignored unless they are percent-encoded, in which case they are part
    // of the file name of the model.

    const file = new loc.File(utils.resourcePath('api/sed/query_and_fragment_cellml_file.sedml'));

    file.setContents(sedmlContents('../../cellml_2.cellml?a=b#c'));

    const neededFile = new loc.File(utils.resourcePath('cellml_2.cellml'));
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);

    const encodedFile = new loc.File(utils.resourcePath('api/sed/encoded_query_and_fragment_cellml_file.sedml'));

    encodedFile.setContents(sedmlContents('../../cellml_2.cellml%3Fa=b%23c'));

    const encodedNeededFile = new loc.File(utils.resourcePath('cellml_2.cellml?a=b#c'));
    const encodedDocument = new loc.SedDocument(encodedFile);

    assert.strictEqual(encodedDocument.hasIssues, false);
    assert.strictEqual(encodedDocument.models.length, 1);
    assert.strictEqual(encodedDocument.models[0].file.isAliasOf(encodedNeededFile), true);
  });

  test('SED-ML file with query-only CellML file', () => {
    // A query-only model source refers to the SED-ML file itself since the query of a model source is ignored.

    const file = new loc.File(utils.resourcePath('api/sed/query_only_cellml_file.sedml'));

    file.setContents(sedmlContents('?model=1'));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(file), true);
  });

  test('SED-ML file with fragment-only CellML file', () => {
    // A fragment-only model source refers to the SED-ML file itself since the fragment of a model source is ignored.

    const file = new loc.File(utils.resourcePath('api/sed/fragment_only_cellml_file.sedml'));

    file.setContents(sedmlContents('#model'));

    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(file), true);
  });

  test('Remote SED-ML file with relative CellML file', () => {
    // A relative model source is relative to the remote SED-ML file, whatever the platform and even if the query and/or
    // fragment of the SED-ML file's URL contain some forward slashes.

    const file = new loc.File('https://example.com/simulations/simulation.sedml?a=b/c#d/e');

    file.setContents(sedmlContents('../models/./model.cellml'));

    const neededFile = new loc.File('https://example.com/models/model.cellml');
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('Remote SED-ML file without path with relative CellML file', () => {
    // A relative model source is relative to the root of a remote SED-ML file which URL has no path.

    const file = new loc.File('https://example.com?a=b/c');

    file.setContents(sedmlContents('model.cellml'));

    const neededFile = new loc.File('https://example.com/model.cellml');
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('Remote SED-ML file with absolute CellML file', () => {
    // An absolute model source is relative to the origin of the remote SED-ML file, i.e. never a local file name.

    const file = new loc.File('https://example.com/simulations/simulation.sedml?a=b/c#d/e');

    file.setContents(sedmlContents('/models/./model.cellml'));

    const neededFile = new loc.File('https://example.com/models/model.cellml');
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('Remote SED-ML file with network-path CellML file', () => {
    // A network-path model source is relative to the scheme of the remote SED-ML file, i.e. never a local file name.

    const file = new loc.File('https://example.com/simulations/simulation.sedml');

    file.setContents(sedmlContents('//example.org/models/model.cellml'));

    const neededFile = new loc.File('https://example.org/models/model.cellml');
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('Remote SED-ML file with query-only CellML file', () => {
    // A query-only model source is relative to the URL of the remote SED-ML file without its query and fragment, i.e.
    // never a local file name.

    const file = new loc.File('https://example.com/simulations/simulation.sedml?a=b/c#d/e');

    file.setContents(sedmlContents('?model=1'));

    const neededFile = new loc.File('https://example.com/simulations/simulation.sedml?model=1');
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('Remote SED-ML file with fragment-only CellML file', () => {
    // A fragment-only model source is relative to the URL of the remote SED-ML file without its fragment, i.e. never a
    // local file name.

    const file = new loc.File('https://example.com/simulations/simulation.sedml?a=b/c#d/e');

    file.setContents(sedmlContents('#model'));

    const neededFile = new loc.File('https://example.com/simulations/simulation.sedml?a=b/c#model');
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('Remote SED-ML file with remote CellML file', () => {
    // A model source that is a URL is used as is (its scheme being normalised), i.e. it is not relative to the remote
    // SED-ML file.

    const file = new loc.File('https://example.com/simulations/simulation.sedml');

    file.setContents(sedmlContents('HTTPS://example.org/models/model.cellml'));

    const neededFile = new loc.File('https://example.org/models/model.cellml');
    const document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(neededFile), true);
  });

  test('Remote SED-ML file with unsupported scheme CellML file', () => {
    // A model source with a scheme other than "http", "https", or "file" is an absolute URI that we cannot retrieve, so
    // its model is ignored rather than considered to be relative to the remote SED-ML file.

    const file = new loc.File('https://example.com/simulations/simulation.sedml');

    file.setContents(sedmlContents('urn:miriam:biomodels.db:BIOMD0000000012'));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the model 'urn:miriam:biomodels.db:BIOMD0000000012' has an unsupported scheme. It has been ignored."
      ]
    ]);
    assert.strictEqual(document.models.length, 0);
  });

  test('Remote SED-ML file with empty source CellML file', () => {
    // An empty model source refers to the remote SED-ML file itself (see
    // https://www.rfc-editor.org/rfc/rfc3986#section-5.2.2), but libSEDML considers it to be an error, so the file is
    // not considered to be a SED-ML file (and its model source never gets resolved).

    const file = new loc.File('https://example.com/simulations/simulation.sedml');

    file.setContents(sedmlContents(''));

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [loc.Issue.Type.ERROR, 'A simulation experiment description cannot be created using an unknown file.']
    ]);
    assert.strictEqual(document.models.length, 0);
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

  test('COMBINE archive with no master file and one unknown HTTPS CellML file', () => {
    // The format of an unknown file is used to determine its type, and a format may use the "https" scheme (e.g.,
    // "https://identifiers.org/combine.specifications/cellml.1.1").

    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_one_unknown_https_cellml_file.omex'));

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

  test('COMBINE archive with master file with duplicate location', () => {
    // The master attribute of an entry that is ignored because its location refers to the same file as another entry
    // still applies to that file.

    const file = new loc.File(utils.resourcePath('api/sed/master_file_with_duplicate_location.omex'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    assertIssues(loc, file, [
      [
        loc.Issue.Type.WARNING,
        "COMBINE archive: the file 'cellml_2_b.cellml' is the same as the file './cellml_2_b.cellml'. It has been ignored."
      ]
    ]);
    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(file.childFileFromFileName('./cellml_2_b.cellml')), true);
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

  test('COMBINE archive with no master file and one unknown versioned SED-ML file', () => {
    // The format of an unknown file is used to determine its type, and a format may be versioned and use the "https"
    // scheme (e.g., "https://identifiers.org/combine.specifications/sed-ml.level-1.version-3"). So, the unknown SED-ML
    // file is the master file, rather than the CellML file.

    const file = new loc.File(utils.resourcePath('api/sed/no_master_file_with_one_unknown_versioned_sedml_file.omex'));

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

    assert.strictEqual(document.hasIssues, false);

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

  test('Irretrievable file', () => {
    // Note: the file is an unknown file rather than an irretrievable file since our JavaScript bindings never retrieve
    //       the contents of a file.

    const file = new loc.File(utils.resourcePath('irretrievable_file.txt'));
    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [loc.Issue.Type.ERROR, 'A simulation experiment description cannot be created using an unknown file.']
    ]);
  });
});
