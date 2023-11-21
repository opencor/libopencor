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

  function expectedSerialisation(source, parameters = {}) {
    const integrationMethod = parameters["KISAO:0000475"] || "BDF";
    const iterationType = parameters["KISAO:0000476"] || "Newton";
    const linearSolver = parameters["KISAO:0000477"] || "Dense";
    const preconditioner = parameters["KISAO:0000478"] || "Banded";
    const interpolateSolution = parameters["KISAO:0000481"] || "true";

    return (
      `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="` +
      source +
      `"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="` +
      integrationMethod +
      `"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="` +
      iterationType +
      `"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="` +
      linearSolver +
      `"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="` +
      preconditioner +
      `"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="` +
      interpolateSolution +
      `"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
</sed>
`
    );
  }

  test("Local CellML file with base path", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt"),
    );
  });

  test("Local CellML file without base path", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise()).toBe(
      expectedSerialisation("file:///some/path/file.txt"),
    );
  });

  test("Relative local CellML file with base path", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH + "/../..")).toBe(
      expectedSerialisation("some/path/file.txt"),
    );
  });

  test("Relative local CellML file without base path", () => {
    const file = new libopencor.File("file.txt");

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise()).toBe(expectedSerialisation("file.txt"));
  });

  test("Remote CellML file with base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.REMOTE_BASE_PATH)).toBe(
      expectedSerialisation("cellml_2.cellml"),
    );
  });

  test("Remote CellML file without base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise()).toBe(
      expectedSerialisation(
        "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml",
      ),
    );
  });

  test("Relative remote CellML file with base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.REMOTE_BASE_PATH + "/../..")).toBe(
      expectedSerialisation("tests/res/cellml_2.cellml"),
    );
  });

  test("Fixed-step ODE solver", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000030">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
</sed>
`;

    const file = new libopencor.File(utils.REMOTE_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);

    simulation.setOdeSolver(new libopencor.SolverForwardEuler());

    expect(sed.serialise(utils.REMOTE_BASE_PATH)).toBe(expectedSerialisation);
  });

  test("CVODE solver with the Adams-Moulton integration method", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setIntegrationMethod(
      libopencor.SolverCvode.IntegrationMethod.ADAMS_MOULTON,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000475": "Adams-Moulton" }),
    );
  });

  test("CVODE solver with a functional iteration type", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setIterationType(
      libopencor.SolverCvode.IterationType.FUNCTIONAL,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000476": "Functional" }),
    );
  });

  test("CVODE solver with a banded linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(
      libopencor.SolverCvode.LinearSolver.BANDED,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000477": "Banded" }),
    );
  });

  test("CVODE solver with a diagonal linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(
      libopencor.SolverCvode.LinearSolver.DIAGONAL,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000477": "Diagonal" }),
    );
  });

  test("CVODE solver with a GMRES linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(
      libopencor.SolverCvode.LinearSolver.GMRES,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000477": "GMRES" }),
    );
  });

  test("CVODE solver with a BiCGStab linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(
      libopencor.SolverCvode.LinearSolver.BICGSTAB,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000477": "BiCGStab" }),
    );
  });

  test("CVODE solver with a TFQMR linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(
      libopencor.SolverCvode.LinearSolver.TFQMR,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000477": "TFQMR" }),
    );
  });

  test("CVODE solver with no preconditioner", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setPreconditioner(
      libopencor.SolverCvode.Preconditioner.NO,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000478": "No" }),
    );
  });

  test("CVODE solver with no interpolate solution", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setInterpolateSolution(false);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      expectedSerialisation("file.txt", { "KISAO:0000481": "false" }),
    );
  });
});
