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

describe("SedDocument serialise tests", () => {
  let someCellmlContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
  });

  test("Local CellML file with base path", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="file.txt"/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument();

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);
    sed.initialise(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(expectedSerialisation);
  });

  test("Local CellML file without base path", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="file:///some/path/file.txt"/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument();

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);
    sed.initialise(file);

    expect(sed.serialise()).toBe(expectedSerialisation);
  });

  test("Relative local CellML file with base path", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="some/path/file.txt"/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument();

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);
    sed.initialise(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH + "/../..")).toBe(
      expectedSerialisation,
    );
  });

  test("Relative local CellML file without base path", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="file.txt"/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File("file.txt");
    const sed = new libopencor.SedDocument();

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);
    sed.initialise(file);

    expect(sed.serialise()).toBe(expectedSerialisation);
  });

  test("Remote CellML file with base path", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File(utils.REMOTE_FILE);
    const sed = new libopencor.SedDocument();

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);
    sed.initialise(file);

    expect(sed.serialise(utils.REMOTE_BASE_PATH)).toBe(expectedSerialisation);
  });

  test("Remote CellML file without base path", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File(utils.REMOTE_FILE);
    const sed = new libopencor.SedDocument();

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);
    sed.initialise(file);

    expect(sed.serialise()).toBe(expectedSerialisation);
  });

  test("Relative remote CellML file with base path", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model000000001" language="urn:sedml:language:cellml" source="tests/res/cellml_2.cellml"/>
  </listOfModels>
</sed>
`;
    const file = new libopencor.File(utils.REMOTE_FILE);
    const sed = new libopencor.SedDocument();

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);
    sed.initialise(file);

    expect(sed.serialise(utils.REMOTE_BASE_PATH + "/../..")).toBe(
      expectedSerialisation,
    );
  });
});
