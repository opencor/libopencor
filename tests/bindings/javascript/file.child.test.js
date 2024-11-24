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

import libOpenCOR from "./libopencor.js";
import * as utils from "./utils.js";

const libopencor = await libOpenCOR();

describe("File type tests", () => {
  let dataset135OmexContentsPtr;
  let dataset135JsonContentsPtr;
  let dataset157OmexContentsPtr;
  let dataset157JsonContentsPtr;

  beforeAll(() => {
    dataset135OmexContentsPtr = utils.allocateMemory(
      libopencor,
      utils.DATASET_135_OMEX_CONTENTS,
    );
    dataset135JsonContentsPtr = utils.allocateMemory(
      libopencor,
      utils.DATASET_135_JSON_CONTENTS,
    );
    dataset157OmexContentsPtr = utils.allocateMemory(
      libopencor,
      utils.DATASET_157_OMEX_CONTENTS,
    );
    dataset157JsonContentsPtr = utils.allocateMemory(
      libopencor,
      utils.DATASET_157_JSON_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, dataset135OmexContentsPtr);
    utils.freeMemory(libopencor, dataset135JsonContentsPtr);
    utils.freeMemory(libopencor, dataset157OmexContentsPtr);
    utils.freeMemory(libopencor, dataset157JsonContentsPtr);
  });

  function doTestDataset(
    omexContentsPtr,
    omexContents,
    jsonContents,
    specificChildFileNames,
  ) {
    const file = new libopencor.File(utils.resourcePath(utils.COMBINE_ARCHIVE));

    file.setContents(omexContentsPtr, omexContents.length);

    expect(file.hasChildFiles()).toBe(true);
    expect(file.childFileCount()).toBe(specificChildFileNames.length + 1);
    expect(file.childFileNames().size()).toBe(
      specificChildFileNames.length + 1,
    );
    expect(file.childFiles().size()).toBe(specificChildFileNames.length + 1);

    const simulationFile = file.childFileFromFileName("simulation.json");

    expect(simulationFile).not.toBeNull();

    for (let i = 0; i < specificChildFileNames.length; ++i) {
      expect(
        file.childFileFromFileName(specificChildFileNames[i]),
      ).not.toBeNull();
    }

    expect(file.childFileFromFileName(utils.UNKNOWN_FILE)).toBeNull();

    expect(simulationFile.contents()).toStrictEqual(jsonContents);

    file.delete();
  }

  test("No child files", () => {
    const file = new libopencor.File(utils.resourcePath(utils.UNKNOWN_FILE));

    expect(file.hasChildFiles()).toBe(false);
    expect(file.childFileCount()).toBe(0);
    expect(file.childFileNames().size()).toBe(0);
    expect(file.childFiles().size()).toBe(0);
    expect(file.childFileFromFileName(utils.UNKNOWN_FILE)).toBeNull();

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
