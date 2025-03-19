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

describe("Sed serialise tests", () => {
  let someCellmlContentsPtr;
  let someSedmlContentsPtr;
  let someAlgebraicContentsPtr;
  let someDaeContentsPtr;
  let someNlaContentsPtr;
  let someSedmlWithSimulationsContentsPtr;

  beforeAll(() => {
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    someSedmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_CONTENTS,
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
    someSedmlWithSimulationsContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_WITH_SIMULATIONS_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someSedmlContentsPtr);
    utils.freeMemory(libopencor, someAlgebraicContentsPtr);
    utils.freeMemory(libopencor, someDaeContentsPtr);
    utils.freeMemory(libopencor, someNlaContentsPtr);
    utils.freeMemory(libopencor, someSedmlWithSimulationsContentsPtr);
  });

  function cvodeExpectedSerialisation(source, parameters = {}) {
    const integrationMethod = parameters["KISAO:0000475"] || "BDF";
    const iterationType = parameters["KISAO:0000476"] || "Newton";
    const linearSolver = parameters["KISAO:0000477"] || "Dense";
    const preconditioner = parameters["KISAO:0000478"] || "Banded";
    const interpolateSolution = parameters["KISAO:0000481"] || "true";

    return `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="${source}"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="${integrationMethod}"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="${iterationType}"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="${linearSolver}"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="${preconditioner}"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="${interpolateSolution}"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
`;
  }

  function kinsolExpectedSerialisation(parameters = {}) {
    const linearSolver = parameters["KISAO:0000477"] || "Dense";

    return `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <steadyState id="simulation1">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="${linearSolver}"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
`;
  }

  test("Local CellML file with base path", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml"),
    );

    document.delete();
    file.delete();
  });

  test("Local CellML file without base path", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise()).toBe(
      cvodeExpectedSerialisation("file:///some/path/file.txt"),
    );

    document.delete();
    file.delete();
  });

  test("Relative local CellML file with base path", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise(utils.RESOURCE_LOCATION + "/../..")).toBe(
      cvodeExpectedSerialisation("some/path/cellml_2.cellml"),
    );

    document.delete();
    file.delete();
  });

  test("Relative local CellML file without base path", () => {
    const file = new libopencor.File(utils.CELLML_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise()).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml"),
    );

    document.delete();
    file.delete();
  });

  test("Remote CellML file with base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise(utils.REMOTE_BASE_PATH)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml"),
    );

    document.delete();
    file.delete();
  });

  test("Remote CellML file without base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise()).toBe(
      cvodeExpectedSerialisation(
        "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml",
      ),
    );

    document.delete();
    file.delete();
  });

  test("Relative remote CellML file with base path", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise(utils.REMOTE_BASE_PATH + "/../..")).toBe(
      cvodeExpectedSerialisation("tests/res/cellml_2.cellml"),
    );

    document.delete();
    file.delete();
  });

  test("DAE model", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
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
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someDaeContentsPtr, utils.SOME_DAE_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      expectedSerialisation,
    );

    document.delete();
    file.delete();
  });

  test("NLA model", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      kinsolExpectedSerialisation(),
    );

    document.delete();
    file.delete();
  });

  test("Algebraic model", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <steadyState id="simulation1"/>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(
      someAlgebraicContentsPtr,
      utils.SOME_ALGEBRAIC_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      expectedSerialisation,
    );

    document.delete();
    file.delete();
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
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.odeSolver = new libopencor.SolverForwardEuler();

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      expectedSerialisation,
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with the Adams-Moulton integration method", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.integrationMethod =
      libopencor.SolverCvode.IntegrationMethod.ADAMS_MOULTON;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000475": "Adams-Moulton",
      }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with a functional iteration type", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.iterationType = libopencor.SolverCvode.IterationType.FUNCTIONAL;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000476": "Functional",
      }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with a banded linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = libopencor.SolverCvode.LinearSolver.BANDED;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000477": "Banded",
      }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with a diagonal linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = libopencor.SolverCvode.LinearSolver.DIAGONAL;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000477": "Diagonal",
      }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with a GMRES linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = libopencor.SolverCvode.LinearSolver.GMRES;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000477": "GMRES",
      }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with a BiCGStab linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = libopencor.SolverCvode.LinearSolver.BICGSTAB;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000477": "BiCGStab",
      }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with a TFQMR linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = libopencor.SolverCvode.LinearSolver.TFQMR;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000477": "TFQMR",
      }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with no preconditioner", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.preconditioner = libopencor.SolverCvode.Preconditioner.NO;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", { "KISAO:0000478": "No" }),
    );

    document.delete();
    file.delete();
  });

  test("CVODE solver with no interpolate solution", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.interpolateSolution = false;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      cvodeExpectedSerialisation("cellml_2.cellml", {
        "KISAO:0000481": "false",
      }),
    );

    document.delete();
    file.delete();
  });

  test("KINSOL solver with a banded linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = libopencor.SolverKinsol.LinearSolver.BANDED;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "Banded" }),
    );

    document.delete();
    file.delete();
  });

  test("KINSOL solver with a GMRES linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = libopencor.SolverKinsol.LinearSolver.GMRES;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "GMRES" }),
    );

    document.delete();
    file.delete();
  });

  test("KINSOL solver with a BiCGStab linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = libopencor.SolverKinsol.LinearSolver.BICGSTAB;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "BiCGStab" }),
    );

    document.delete();
    file.delete();
  });

  test("KINSOL solver with a TFQMR linear solver", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someNlaContentsPtr, utils.SOME_NLA_CONTENTS.length);

    const document = new libopencor.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.nlaSolver;

    solver.linearSolver = libopencor.SolverKinsol.LinearSolver.TFQMR;

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      kinsolExpectedSerialisation({ "KISAO:0000477": "TFQMR" }),
    );

    document.delete();
    file.delete();
  });

  test("One-step simulation", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="simulation1" step="1"/>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    document.removeSimulation(document.simulations.get(0));

    const simulation = new libopencor.SedOneStep(document);

    document.addSimulation(simulation);

    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      expectedSerialisation,
    );

    simulation.delete();
    document.delete();
    file.delete();
  });

  test("SED-ML file", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="50" numberOfSteps="50000">
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
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
`;

    const file = new libopencor.File(utils.resourcePath(utils.SEDML_FILE));

    file.setContents(someSedmlContentsPtr, utils.SOME_SEDML_CONTENTS.length);

    let document = new libopencor.SedDocument(file);

    expectIssues(libopencor, document, [
      [
        libopencor.Issue.Type.WARNING,
        "The model 'cellml_2.cellml' could not be found in the file manager. It has been automatically added to it.",
      ],
    ]);
    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      expectedSerialisation,
    );

    file.delete();
    document.delete();
  });

  test("SedSimulation", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfSimulations>
    <oneStep id="oneStep1" step="3.69">
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
    </oneStep>
    <oneStep id="oneStep2" step="3.69">
      <algorithm kisaoID="KISAO:0000030">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1.23"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep3" step="3.69">
      <algorithm kisaoID="KISAO:0000032">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep4" step="3.69">
      <algorithm kisaoID="KISAO:0000301">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep5" step="3.69">
      <algorithm kisaoID="KISAO:0000381">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep6" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="0.001"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-05"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="369"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0.01"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="1"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="2"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="false"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep7" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="Adams-Moulton"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Functional"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="No"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="false"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep8" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Diagonal"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep9" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="GMRES"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep10" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="BiCGStab"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep11" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="TFQMR"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep12" step="3.69">
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
    </oneStep>
    <steadyState id="steadyState1">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="1"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="2"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="123"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState2">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState3">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="GMRES"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState4">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="BiCGStab"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState5">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="TFQMR"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState6">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <analysis id="analysis">
      <algorithm kisaoID="KISAO:0000030">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1.23"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </analysis>
  </listOfSimulations>
</sedML>
`;

    const file = new libopencor.File(utils.resourcePath(utils.SEDML_FILE));

    file.setContents(
      someSedmlWithSimulationsContentsPtr,
      utils.SOME_SEDML_WITH_SIMULATIONS_CONTENTS.length,
    );

    let document = new libopencor.SedDocument(file);

    expectIssues(libopencor, document, [
      [
        libopencor.Issue.Type.WARNING,
        "The solver 'KISAO:1234567' is not recognised. The CVODE solver will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The parameter 'KISAO:1234567' is not recognised. It will be ignored.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The step ('KISAO:0000483') cannot be equal to '-1.23'. It must be greater or equal to 0. A step of 1 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The step ('KISAO:0000483') cannot be equal to 'nan'. It must be greater or equal to 0. A step of 1 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The step ('KISAO:0000483') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A step of 1 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The parameter 'KISAO:1234567' is not recognised. It will be ignored.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The relative tolerance ('KISAO:0000209') cannot be equal to '-1e-03'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The absolute tolerance ('KISAO:0000211') cannot be equal to '-1e-05'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum number of steps ('KISAO:0000415') cannot be equal to '-369'. It must be greater than 0. A maximum number of steps of 500 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum step ('KISAO:0000467') cannot be equal to '-0.01'. It must be greater or equal to 0. A maximum step of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '-1'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '-2'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The relative tolerance ('KISAO:0000209') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The absolute tolerance ('KISAO:0000211') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum number of steps ('KISAO:0000415') cannot be equal to 'NotANumber'. It must be greater than 0. A maximum number of steps of 500 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum step ('KISAO:0000467') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A maximum step of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The integration method ('KISAO:0000475') cannot be equal to 'Unknown'. It must be equal to 'BDF' or 'Adams-Moulton'. A BDF integration method will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The iteration type ('KISAO:0000476') cannot be equal to 'Unknown'. It must be equal to 'Functional' or 'Newton'. A Newton iteration type will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The preconditioner ('KISAO:0000478') cannot be equal to 'Unknown'. It must be equal to 'No' or 'Banded'. A Banded preconditioner will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The interpolate solution parameter ('KISAO:0000481') cannot be equal to 'True'. It must be equal to 'true' or 'false'. A value of true will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The relative tolerance ('KISAO:0000209') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The absolute tolerance ('KISAO:0000211') cannot be equal to '1.23e456789'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum number of steps ('KISAO:0000415') cannot be equal to '1234567890123'. It must be greater than 0. A maximum number of steps of 500 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum step ('KISAO:0000467') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A maximum step of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '1234567890123'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '1234567890123'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The linear solver ('KISAO:0000477') cannot be equal to 'Unknown'. It must be equal to 'Dense', 'Banded', 'Diagonal', 'GMRES', 'BiCGStab', or 'TFQMR'. A Dense linear solver will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The parameter 'KISAO:1234567' is not recognised. It will be ignored.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum number of iterations ('KISAO:0000486') cannot be equal to '-123'. It must be greater than 0. A maximum number of iterations of 200 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '-1'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '-2'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum number of iterations ('KISAO:0000486') cannot be equal to 'NotANumber'. It must be greater than 0. A maximum number of iterations of 200 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The maximum number of iterations ('KISAO:0000486') cannot be equal to '1234567890123'. It must be greater than 0. A maximum number of iterations of 200 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '1234567890123'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '1234567890123'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead.",
      ],
      [
        libopencor.Issue.Type.WARNING,
        "The linear solver ('KISAO:0000477') cannot be equal to 'Unknown'. It must be equal to 'Dense', 'Banded', 'GMRES', 'BiCGStab', or 'TFQMR'. A Dense linear solver will be used instead.",
      ],
    ]);
    expect(document.serialise(utils.RESOURCE_LOCATION)).toBe(
      expectedSerialisation,
    );

    file.delete();
    document.delete();
  });
});
