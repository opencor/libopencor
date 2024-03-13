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
  let someAlgebraicContentsPtr;
  let someDaeContentsPtr;
  let someNlaContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    someAlgebraicContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_ALGEBRAIC_CONTENTS,
    );
    someDaeContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_DAE_CONTENTS,
    );
    someNlaContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_NLA_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someAlgebraicContentsPtr);
    utils.freeMemory(libopencor, someDaeContentsPtr);
    utils.freeMemory(libopencor, someNlaContentsPtr);
  });

  function cvodeExpectedSerialisation(source, parameters = {}) {
    const integrationMethod = parameters["KISAO:0000475"] || "BDF";
    const iterationType = parameters["KISAO:0000476"] || "Newton";
    const linearSolver = parameters["KISAO:0000477"] || "Dense";
    const preconditioner = parameters["KISAO:0000478"] || "Banded";
    const interpolateSolution = parameters["KISAO:0000481"] || "true";

    return (
      `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
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
  <listOfTasks>
    <task id="task1"/>
  </listOfTasks>
</sedML>
`
    );
  }

  function kinsolExpectedSerialisation(parameters = {}) {
    const linearSolver = parameters["KISAO:0000477"] || "Dense";

    return (
      `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="file.txt"/>
  </listOfModels>
  <listOfSimulations>
    <steadyState id="simulation1">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="` +
      linearSolver +
      `"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1"/>
  </listOfTasks>
</sedML>
`
    );
  }

  test("Local CellML file with base path", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt"),
    );
  });

  test("Local CellML file without base path", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise()).toBe(
      cvodeExpectedSerialisation("file:///some/path/file.txt"),
    );
  });

  test("Relative local CellML file with base path", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH + "/../..")).toBe(
      cvodeExpectedSerialisation("some/path/file.txt"),
    );
  });

  test("Relative local CellML file without base path", () => {
    const file = new libopencor.File("file.txt");

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise()).toBe(cvodeExpectedSerialisation("file.txt"));
  });

  test("Remote CellML file with base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.REMOTE_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml"),
    );
  });

  test("Remote CellML file without base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise()).toBe(
      cvodeExpectedSerialisation(
        "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml",
      ),
    );
  });

  test("Relative remote CellML file with base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.REMOTE_BASE_PATH + "/../..")).toBe(
      cvodeExpectedSerialisation("tests/res/cellml_2.cellml"),
    );
  });

  test("DAE model", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="file.txt"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
      <nlaAlgorithm xmlns="https://opencor.ws/libopencor" kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </nlaAlgorithm>
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someDaeContentsPtr, utils.SOME_DAE_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(expectedSerialisation);
  });

  test("NLA model", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      kinsolExpectedSerialisation(),
    );
  });

  test("Algebraic model", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="file.txt"/>
  </listOfModels>
  <listOfSimulations>
    <steadyState id="simulation1"/>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(
      someAlgebraicContentsPtr,
      utils.SOME_ALGEBRAIC_CONTENTS.length,
    );

    const sed = new libopencor.SedDocument(file);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(expectedSerialisation);
  });

  test("Fixed-step ODE solver", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
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
  <listOfTasks>
    <task id="task1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);

    simulation.setOdeSolver(new libopencor.SolverForwardEuler());

    expect(sed.serialise(utils.REMOTE_BASE_PATH)).toBe(expectedSerialisation);
  });

  test("CVODE solver with the Adams-Moulton integration method", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setIntegrationMethod(
      libopencor.SolverCvode.IntegrationMethod.ADAMS_MOULTON,
    );

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", {
        "KISAO:0000475": "Adams-Moulton",
      }),
    );
  });

  test("CVODE solver with a functional iteration type", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setIterationType(libopencor.SolverCvode.IterationType.FUNCTIONAL);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000476": "Functional" }),
    );
  });

  test("CVODE solver with a banded linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.BANDED);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000477": "Banded" }),
    );
  });

  test("CVODE solver with a diagonal linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.DIAGONAL);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000477": "Diagonal" }),
    );
  });

  test("CVODE solver with a GMRES linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.GMRES);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000477": "GMRES" }),
    );
  });

  test("CVODE solver with a BiCGStab linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.BICGSTAB);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000477": "BiCGStab" }),
    );
  });

  test("CVODE solver with a TFQMR linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.TFQMR);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000477": "TFQMR" }),
    );
  });

  test("CVODE solver with no preconditioner", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setPreconditioner(libopencor.SolverCvode.Preconditioner.NO);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000478": "No" }),
    );
  });

  test("CVODE solver with no interpolate solution", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.odeSolver();

    solver.setInterpolateSolution(false);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("file.txt", { "KISAO:0000481": "false" }),
    );
  });

  test("KINSOL solver with a banded linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.nlaSolver();

    solver.setLinearSolver(libopencor.SolverKinsol.LinearSolver.BANDED);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "Banded" }),
    );
  });

  test("KINSOL solver with a GMRES linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.nlaSolver();

    solver.setLinearSolver(libopencor.SolverKinsol.LinearSolver.GMRES);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "GMRES" }),
    );
  });

  test("KINSOL solver with a BiCGStab linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.nlaSolver();

    solver.setLinearSolver(libopencor.SolverKinsol.LinearSolver.BICGSTAB);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "BiCGStab" }),
    );
  });

  test("KINSOL solver with a TFQMR linear solver", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);
    const simulation = sed.simulations().get(0);
    const solver = simulation.nlaSolver();

    solver.setLinearSolver(libopencor.SolverKinsol.LinearSolver.TFQMR);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "TFQMR" }),
    );
  });

  test("One-step simulation", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="file.txt"/>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="simulation1" step="1"/>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const sed = new libopencor.SedDocument(file);

    sed.removeSimulation(sed.simulations().get(0));

    const simulation = new libopencor.SedSimulationOneStep(sed);

    sed.addSimulation(simulation);

    expect(sed.serialise(utils.LOCAL_BASE_PATH)).toBe(expectedSerialisation);
  });
});
