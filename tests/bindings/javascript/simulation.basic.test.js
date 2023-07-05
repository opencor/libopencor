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

describe("Simulation tests", () => {
  let someUknownContentsPtr;
  let someCellmlContentsPtr;
  let someSedmlContentsPtr;

  beforeAll(() => {
    someUknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS
    );
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS
    );
    someSedmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_CONTENTS
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someUknownContentsPtr);
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someSedmlContentsPtr);
  });

  test("Unknown file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someUknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length
    );
    const simulation = new libopencor.Simulation(file);

    expectIssues(
      [
        [
          libopencor.Issue.Type.ERROR,
          "A simulation cannot be created using an unknown file.",
        ],
      ],
      simulation
    );
  });

  test("CellML file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someCellmlContentsPtr,
      utils.SOME_CELLML_CONTENTS.length
    );
    const simulation = new libopencor.Simulation(file);

    expect(simulation.issueCount()).toBe(0);
  });

  test("SED-ML file", () => {
    const file = new libopencor.File(
      utils.LOCAL_FILE,
      someCellmlContentsPtr,
      utils.SOME_SEDML_CONTENTS.length
    );
    const simulation = new libopencor.Simulation(file);

    expectIssues(
      [
        [
          libopencor.Issue.Type.ERROR,
          "A simulation cannot currently be created using a SED-ML file.",
        ],
      ],
      simulation
    );
  });
});
