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
import { expectIssues } from "./utils.js";

const libopencor = await libOpenCOR();

describe("SedDocument basic tests", () => {
  let someCellmlContentsPtr;
  let someSedmlContentsPtr;
  let someUnknownContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    someSedmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_CONTENTS,
    );
    someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someSedmlContentsPtr);
    utils.freeMemory(libopencor, someUnknownContentsPtr);
  });

  test("Unknown file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );
    const sed = new libopencor.SedDocument(file);

    expectIssues(
      [
        [
          libopencor.Issue.Type.ERROR,
          "A simulation experiment description cannot be created using an unknown file.",
        ],
      ],
      sed,
    );
  });

  test("CellML file", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="..."/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someCellmlContentsPtr,
      utils.SOME_CELLML_CONTENTS.length,
    );
    const sed = new libopencor.SedDocument(file);

    expect(sed.hasIssues()).toBe(false);
    expect(sed.serialise()).toBe(expectedSerialisation);
  });

  test("SED-ML file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someSedmlContentsPtr,
      utils.SOME_SEDML_CONTENTS.length,
    );
    const sed = new libopencor.SedDocument(file);

    expectIssues(
      [
        [
          libopencor.Issue.Type.ERROR,
          "A simulation experiment description cannot currently be created using a SED-ML file.",
        ],
      ],
      sed,
    );
  });
});
