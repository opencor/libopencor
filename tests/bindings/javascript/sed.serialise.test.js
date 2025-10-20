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

import assert from 'node:assert'
import test from 'node:test'

import libOpenCOR from './libopencor.js'
import * as utils from './utils.js'
import { assertIssues } from './utils.js'

const loc = await libOpenCOR()

test.describe('Sed serialise tests', () => {
  let cellmlContentsPtr
  let sedmlContentsPtr
  let algebraicContentsPtr
  let daeContentsPtr
  let nlaContentsPtr
  let sedmlWithSimulationsContentsPtr

  test.before(() => {
    cellmlContentsPtr = utils.allocateMemory(loc, utils.CELLML_CONTENTS)
    sedmlContentsPtr = utils.allocateMemory(loc, utils.SEDML_CONTENTS)
    algebraicContentsPtr = utils.allocateMemory(loc, utils.ALGEBRAIC_CONTENTS)
    daeContentsPtr = utils.allocateMemory(loc, utils.DAE_CONTENTS)
    nlaContentsPtr = utils.allocateMemory(loc, utils.NLA_CONTENTS)
    sedmlWithSimulationsContentsPtr = utils.allocateMemory(loc, utils.SEDML_WITH_SIMULATIONS_CONTENTS)
  })

  test.beforeEach(() => {
    loc.FileManager.instance().reset()
  })

  test.after(() => {
    utils.freeMemory(loc, cellmlContentsPtr)
    utils.freeMemory(loc, sedmlContentsPtr)
    utils.freeMemory(loc, algebraicContentsPtr)
    utils.freeMemory(loc, daeContentsPtr)
    utils.freeMemory(loc, nlaContentsPtr)
    utils.freeMemory(loc, sedmlWithSimulationsContentsPtr)
  })

  function cvodeExpectedSerialisation(source, parameters = {}) {
    const integrationMethod = parameters['KISAO:0000475'] || 'BDF'
    const iterationType = parameters['KISAO:0000476'] || 'Newton'
    const linearSolver = parameters['KISAO:0000477'] || 'Dense'
    const preconditioner = parameters['KISAO:0000478'] || 'Banded'
    const interpolateSolution = parameters['KISAO:0000481'] || 'true'

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
`
  }

  function kinsolExpectedSerialisation(parameters = {}) {
    const linearSolver = parameters['KISAO:0000477'] || 'Dense'

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
`
  }

  test('Local CellML file with base path', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), cvodeExpectedSerialisation('cellml_2.cellml'))
  })

  test('Local CellML file without base path', () => {
    const file = new loc.File(utils.LOCAL_FILE)

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(document.serialise(), cvodeExpectedSerialisation('file:///some/path/file.txt'))
  })

  test('Relative local CellML file with base path', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(
      document.serialise(`${utils.RESOURCE_LOCATION}/../..`),
      cvodeExpectedSerialisation('some/path/cellml_2.cellml')
    )
  })

  test('Relative local CellML file without base path', () => {
    const file = new loc.File(utils.CELLML_FILE)

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(document.serialise(), cvodeExpectedSerialisation('cellml_2.cellml'))
  })

  test('Remote CellML file with base path', () => {
    const file = new loc.File(utils.REMOTE_FILE)

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(document.serialise(utils.REMOTE_BASE_PATH), cvodeExpectedSerialisation('cellml_2.cellml'))
  })

  test('Remote CellML file without base path', () => {
    const file = new loc.File(utils.REMOTE_FILE)

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(
      document.serialise(),
      cvodeExpectedSerialisation(
        'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml'
      )
    )
  })

  test('Relative remote CellML file with base path', () => {
    const file = new loc.File(utils.REMOTE_FILE)

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(
      document.serialise(`${utils.REMOTE_BASE_PATH}/../..`),
      cvodeExpectedSerialisation('tests/res/cellml_2.cellml')
    )
  })

  test('DAE model', () => {
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
`

    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(daeContentsPtr, utils.DAE_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), expectedSerialisation)
  })

  test('NLA model', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(nlaContentsPtr, utils.NLA_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), kinsolExpectedSerialisation())
  })

  test('Algebraic model', () => {
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
`

    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(algebraicContentsPtr, utils.ALGEBRAIC_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), expectedSerialisation)
  })

  function sedChangeExpectedSerialisation(component, variable, newValue) {
    return `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.sedml">
      <listOfChanges>
        <changeAttribute target="/cellml:model/cellml:component[@name='${component}']/cellml:variable[@name='${variable}']" newValue="${newValue}"/>
      </listOfChanges>
    </model>
  </listOfModels>
</sedML>
`
  }

  test('Model with changes', () => {
    const changeAttribute = new loc.SedChangeAttribute('component', 'variable', '123.456789')

    assert.strictEqual(
      changeAttribute.target,
      "/cellml:model/cellml:component[@name='component']/cellml:variable[@name='variable']"
    )
    assert.strictEqual(changeAttribute.componentName, 'component')
    assert.strictEqual(changeAttribute.variableName, 'variable')
    assert.strictEqual(changeAttribute.newValue, '123.456789')

    const document = new loc.SedDocument()
    const file = new loc.File(utils.SEDML_FILE)
    const model = new loc.SedModel(document, file)

    assert.strictEqual(model.addChange(changeAttribute), true)
    assert.strictEqual(document.addModel(model), true)

    assert.strictEqual(document.serialise(), sedChangeExpectedSerialisation('component', 'variable', '123.456789'))

    changeAttribute.componentName = 'new_component'
    changeAttribute.variableName = 'new_variable'
    changeAttribute.newValue = '987.654321'

    assert.strictEqual(
      changeAttribute.target,
      "/cellml:model/cellml:component[@name='new_component']/cellml:variable[@name='new_variable']"
    )
    assert.strictEqual(changeAttribute.componentName, 'new_component')
    assert.strictEqual(changeAttribute.variableName, 'new_variable')
    assert.strictEqual(changeAttribute.newValue, '987.654321')

    assert.strictEqual(
      document.serialise(),
      sedChangeExpectedSerialisation('new_component', 'new_variable', '987.654321')
    )
  })

  test('Fixed-step ODE solver', () => {
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
`

    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)

    simulation.odeSolver = new loc.SolverForwardEuler()

    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), expectedSerialisation)
  })

  test('CVODE solver with the Adams-Moulton integration method', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.integrationMethod = loc.SolverCvode.IntegrationMethod.ADAMS_MOULTON

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000475': 'Adams-Moulton'
      })
    )
  })

  test('CVODE solver with a functional iteration type', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.iterationType = loc.SolverCvode.IterationType.FUNCTIONAL

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000476': 'Functional'
      })
    )
  })

  test('CVODE solver with a banded linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000477': 'Banded'
      })
    )
  })

  test('CVODE solver with a diagonal linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.linearSolver = loc.SolverCvode.LinearSolver.DIAGONAL

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000477': 'Diagonal'
      })
    )
  })

  test('CVODE solver with a GMRES linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.linearSolver = loc.SolverCvode.LinearSolver.GMRES

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000477': 'GMRES'
      })
    )
  })

  test('CVODE solver with a BiCGStab linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.linearSolver = loc.SolverCvode.LinearSolver.BICGSTAB

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000477': 'BiCGStab'
      })
    )
  })

  test('CVODE solver with a TFQMR linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.linearSolver = loc.SolverCvode.LinearSolver.TFQMR

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000477': 'TFQMR'
      })
    )
  })

  test('CVODE solver with no preconditioner', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.preconditioner = loc.SolverCvode.Preconditioner.NO

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', { 'KISAO:0000478': 'No' })
    )
  })

  test('CVODE solver with no interpolate solution', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.odeSolver

    solver.interpolateSolution = false

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      cvodeExpectedSerialisation('cellml_2.cellml', {
        'KISAO:0000481': 'false'
      })
    )
  })

  test('KINSOL solver with a banded linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(nlaContentsPtr, utils.NLA_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.nlaSolver

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BANDED

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      kinsolExpectedSerialisation({ 'KISAO:0000477': 'Banded' })
    )
  })

  test('KINSOL solver with a GMRES linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(nlaContentsPtr, utils.NLA_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.nlaSolver

    solver.linearSolver = loc.SolverKinsol.LinearSolver.GMRES

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      kinsolExpectedSerialisation({ 'KISAO:0000477': 'GMRES' })
    )
  })

  test('KINSOL solver with a BiCGStab linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(nlaContentsPtr, utils.NLA_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.nlaSolver

    solver.linearSolver = loc.SolverKinsol.LinearSolver.BICGSTAB

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      kinsolExpectedSerialisation({ 'KISAO:0000477': 'BiCGStab' })
    )
  })

  test('KINSOL solver with a TFQMR linear solver', () => {
    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(nlaContentsPtr, utils.NLA_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = simulation.nlaSolver

    solver.linearSolver = loc.SolverKinsol.LinearSolver.TFQMR

    assert.strictEqual(
      document.serialise(utils.RESOURCE_LOCATION),
      kinsolExpectedSerialisation({ 'KISAO:0000477': 'TFQMR' })
    )
  })

  test('One-step simulation', () => {
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
`

    const file = new loc.File(utils.resourcePath(utils.CELLML_FILE))

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    document.removeSimulation(document.simulations.get(0))

    const simulation = new loc.SedOneStep(document)

    document.addSimulation(simulation)

    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), expectedSerialisation)
  })

  test('SED-ML file', () => {
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
`

    const file = new loc.File(utils.resourcePath(utils.SEDML_FILE))

    file.setContents(sedmlContentsPtr, utils.SEDML_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "The model 'cellml_2.cellml' could not be found in the file manager. It has been automatically added to it."
      ]
    ])
    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), expectedSerialisation)
  })

  test('SedSimulation', () => {
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
`

    const file = new loc.File(utils.resourcePath(utils.SEDML_FILE))

    file.setContents(sedmlWithSimulationsContentsPtr, utils.SEDML_WITH_SIMULATIONS_CONTENTS.length)

    const document = new loc.SedDocument(file)

    assertIssues(loc, document, [
      [loc.Issue.Type.WARNING, "The solver 'KISAO:1234567' is not recognised. The CVODE solver will be used instead."],
      [loc.Issue.Type.WARNING, "The parameter 'KISAO:1234567' is not recognised. It will be ignored."],
      [
        loc.Issue.Type.WARNING,
        "The step ('KISAO:0000483') cannot be equal to '-1.23'. It must be greater or equal to 0. A step of 1 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The step ('KISAO:0000483') cannot be equal to 'nan'. It must be greater or equal to 0. A step of 1 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The step ('KISAO:0000483') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A step of 1 will be used instead."
      ],
      [loc.Issue.Type.WARNING, "The parameter 'KISAO:1234567' is not recognised. It will be ignored."],
      [
        loc.Issue.Type.WARNING,
        "The relative tolerance ('KISAO:0000209') cannot be equal to '-1e-03'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The absolute tolerance ('KISAO:0000211') cannot be equal to '-1e-05'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum number of steps ('KISAO:0000415') cannot be equal to '-369'. It must be greater than 0. A maximum number of steps of 500 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum step ('KISAO:0000467') cannot be equal to '-0.01'. It must be greater or equal to 0. A maximum step of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '-1'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '-2'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The relative tolerance ('KISAO:0000209') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The absolute tolerance ('KISAO:0000211') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum number of steps ('KISAO:0000415') cannot be equal to 'NotANumber'. It must be greater than 0. A maximum number of steps of 500 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum step ('KISAO:0000467') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A maximum step of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The integration method ('KISAO:0000475') cannot be equal to 'Unknown'. It must be equal to 'BDF' or 'Adams-Moulton'. A BDF integration method will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The iteration type ('KISAO:0000476') cannot be equal to 'Unknown'. It must be equal to 'Functional' or 'Newton'. A Newton iteration type will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The preconditioner ('KISAO:0000478') cannot be equal to 'Unknown'. It must be equal to 'No' or 'Banded'. A Banded preconditioner will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The interpolate solution parameter ('KISAO:0000481') cannot be equal to 'True'. It must be equal to 'true' or 'false'. A value of true will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The relative tolerance ('KISAO:0000209') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The absolute tolerance ('KISAO:0000211') cannot be equal to '1.23e456789'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum number of steps ('KISAO:0000415') cannot be equal to '1234567890123'. It must be greater than 0. A maximum number of steps of 500 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum step ('KISAO:0000467') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A maximum step of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '1234567890123'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '1234567890123'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The linear solver ('KISAO:0000477') cannot be equal to 'Unknown'. It must be equal to 'Dense', 'Banded', 'Diagonal', 'GMRES', 'BiCGStab', or 'TFQMR'. A Dense linear solver will be used instead."
      ],
      [loc.Issue.Type.WARNING, "The parameter 'KISAO:1234567' is not recognised. It will be ignored."],
      [
        loc.Issue.Type.WARNING,
        "The maximum number of iterations ('KISAO:0000486') cannot be equal to '-123'. It must be greater than 0. A maximum number of iterations of 200 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '-1'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '-2'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum number of iterations ('KISAO:0000486') cannot be equal to 'NotANumber'. It must be greater than 0. A maximum number of iterations of 200 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The maximum number of iterations ('KISAO:0000486') cannot be equal to '1234567890123'. It must be greater than 0. A maximum number of iterations of 200 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '1234567890123'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '1234567890123'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "The linear solver ('KISAO:0000477') cannot be equal to 'Unknown'. It must be equal to 'Dense', 'Banded', 'GMRES', 'BiCGStab', or 'TFQMR'. A Dense linear solver will be used instead."
      ]
    ])
    assert.strictEqual(document.serialise(utils.RESOURCE_LOCATION), expectedSerialisation)
  })
})
