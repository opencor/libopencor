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

import assert from "node:assert";
import test from "node:test";

import libOpenCOR from "./libopencor.js";
import * as utils from "./utils.js";

const loc = await libOpenCOR();

test.describe("File type tests", () => {
  let dataset135OmexContentsPtr;
  let dataset135JsonContentsPtr;
  let dataset157OmexContentsPtr;
  let dataset157JsonContentsPtr;

  test.before(() => {
    dataset135OmexContentsPtr = utils.allocateMemory(
      loc,
      utils.DATASET_135_OMEX_CONTENTS,
    );
    dataset135JsonContentsPtr = utils.allocateMemory(
      loc,
      utils.DATASET_135_JSON_CONTENTS,
    );
    dataset157OmexContentsPtr = utils.allocateMemory(
      loc,
      utils.DATASET_157_OMEX_CONTENTS,
    );
    dataset157JsonContentsPtr = utils.allocateMemory(
      loc,
      utils.DATASET_157_JSON_CONTENTS,
    );
  });

  test.after(() => {
    utils.freeMemory(loc, dataset135OmexContentsPtr);
    utils.freeMemory(loc, dataset135JsonContentsPtr);
    utils.freeMemory(loc, dataset157OmexContentsPtr);
    utils.freeMemory(loc, dataset157JsonContentsPtr);
  });

  function doTestDataset(
    omexContentsPtr,
    omexContents,
    jsonContents,
    specificChildFileNames,
  ) {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(omexContentsPtr, omexContents.length);

    assert.strictEqual(file.hasChildFiles, true);
    assert.strictEqual(file.childFileCount, specificChildFileNames.length + 1);
    assert.strictEqual(
      file.childFileNames.size(),
      specificChildFileNames.length + 1,
    );
    assert.strictEqual(
      file.childFiles.size(),
      specificChildFileNames.length + 1,
    );

    let index = -1;
    const simulationFile = file.childFileFromFileName("simulation.json");

    assert.notStrictEqual(file.childFile(++index), null);
    assert.notStrictEqual(simulationFile, null);

    for (let i = 0; i < specificChildFileNames.length; ++i) {
      assert.notStrictEqual(file.childFile(++index), null);
      assert.notStrictEqual(
        file.childFileFromFileName(specificChildFileNames[i]),
        null,
      );
    }

    assert.strictEqual(file.childFile(++index), null);
    assert.strictEqual(file.childFileFromFileName(utils.UNKNOWN_FILE), null);

    assert.deepStrictEqual(simulationFile.contents(), jsonContents);

    file.delete();
  }

  test("No child files", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    assert.strictEqual(file.hasChildFiles, false);
    assert.strictEqual(file.childFileCount, 0);
    assert.strictEqual(file.childFileNames.size(), 0);
    assert.strictEqual(file.childFiles.size(), 0);
    assert.strictEqual(file.childFile(0), null);
    assert.strictEqual(file.childFileFromFileName(utils.UNKNOWN_FILE), null);

    file.delete();
  });

  test("Dataset 135", () => {
    doTestDataset(
      dataset135OmexContentsPtr,
      utils.DATASET_135_OMEX_CONTENTS,
      utils.DATASET_135_JSON_CONTENTS,
      ["HumanSAN_Fabbri_Fantini_Wilders_Severi_2017.cellml"],
    );
  });

  test("Dataset 157", () => {
    doTestDataset(
      dataset157OmexContentsPtr,
      utils.DATASET_157_OMEX_CONTENTS,
      utils.DATASET_157_JSON_CONTENTS,
      [
        "fabbri_et_al_based_composite_SAN_model.cellml",
        "fabbri_et_al_based_composite_SAN_model.sedml",
      ],
    );
  });
});
