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
import { assertIssues, assertValues } from './utils.js';

const loc = await libOpenCOR();

test.describe('Sed coverage tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  function sedChangesExpectedSerialisation(source, changes) {
    const modelEnd =
      changes === ''
        ? '/>'
        : `>
      <listOfChanges>${changes}      </listOfChanges>
    </model>`;

    return `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model" language="urn:sedml:language:cellml" source="file://${source}"${modelEnd}
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
  }

  function sedTaskExpectedSerialisation(withProperties) {
    return `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfTasks>
    <task id="task1"${withProperties ? ` modelReference="model1" simulationReference="simulation1"` : ``}/>
  </listOfTasks>
</sedML>
`;
  }

  test('Initialise', () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
`;
    const document = new loc.SedDocument();

    assert.strictEqual(document.serialise(), expectedSerialisation);
  });

  test('Models', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasModels, false);
    assert.strictEqual(document.modelCount, 0);
    assert.strictEqual(document.models.length, 0);
    assert.strictEqual(document.addModel(null), false);

    const file = new loc.File(utils.resourcePath('file.txt'));
    const model = new loc.SedModel(document, file);

    assert.deepStrictEqual(model.file, file);

    assert.strictEqual(document.addModel(model), true);

    assert.strictEqual(document.hasModels, true);
    assert.strictEqual(document.modelCount, 1);
    assert.strictEqual(document.models.length, 1);
    assert.deepStrictEqual(document.models[0], model);
    assert.deepStrictEqual(document.model(0), model);
    assert.deepStrictEqual(document.model(1), null);

    assert.strictEqual(document.addModel(model), false);
    assert.strictEqual(document.removeModel(model), true);

    assert.strictEqual(document.hasModels, false);
    assert.strictEqual(document.modelCount, 0);
    assert.strictEqual(document.models.length, 0);

    assert.strictEqual(document.removeModel(null), false);
    assert.strictEqual(document.removeAllModels(), false);

    assert.strictEqual(document.addModel(model), true);
    assert.strictEqual(document.removeAllModels(), true);

    assert.strictEqual(model.hasChanges, false);
    assert.strictEqual(model.changeCount, 0);
    assert.strictEqual(model.changes.length, 0);
    assert.strictEqual(model.addChange(null), false);
    assert.strictEqual(model.removeAllChanges(), false);

    const changeAttribute = new loc.SedChangeAttribute('component', 'variable', '1.23');

    assert.strictEqual(model.addChange(changeAttribute), true);

    assert.strictEqual(model.hasChanges, true);
    assert.strictEqual(model.changeCount, 1);
    assert.strictEqual(model.changes.length, 1);
    assert.deepStrictEqual(model.changes[0], changeAttribute);
    assert.deepStrictEqual(model.change(0), changeAttribute);
    assert.deepStrictEqual(model.change(1), null);

    assert.strictEqual(model.addChange(changeAttribute), false);
    assert.strictEqual(model.removeChange(changeAttribute), true);

    assert.strictEqual(model.addChange(changeAttribute), true);
    assert.strictEqual(model.removeAllChanges(), true);

    assert.strictEqual(model.hasChanges, false);
    assert.strictEqual(model.changeCount, 0);
    assert.strictEqual(model.changes.length, 0);

    assert.strictEqual(model.removeChange(null), false);
  });

  test('Changes', () => {
    let file = new loc.File(utils.resourcePath('api/sed/sed_changes.omex'));

    file.setContents(utils.fileContents(file.path));

    let document = new loc.SedDocument(file);

    assert.strictEqual(document.hasIssues, false);
    assert.strictEqual(
      document.serialise(),
      sedChangesExpectedSerialisation(
        file.childFile(1).path,
        `
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='beta']" newValue="3.0"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='rho']" newValue="13.0"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='sigma']" newValue="9.0"/>
`
      )
    );

    file = new loc.File(utils.resourcePath('api/sed/invalid_sed_changes.omex'));

    file.setContents(utils.fileContents(file.path));

    document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target 'invalidTarget'."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the new value 'invalidNewValue' for the change of type 'changeAttribute' is not a valid double value."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name=''."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName'."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name=''."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName'."
      ],
      [
        loc.Issue.Type.ERROR,
        "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName']Invalid'."
      ]
    ]);
    assert.strictEqual(
      document.serialise(),
      sedChangesExpectedSerialisation(
        file.childFile(1).path,
        `
        <changeAttribute target="/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName']" newValue="1.23"/>
`
      )
    );

    file = new loc.File(utils.resourcePath('api/sed/unsupported_sed_changes.omex'));

    file.setContents(utils.fileContents(file.path));

    document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'addXML' has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'changeXML' has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'removeXML' has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'computeChange' has been ignored."
      ]
    ]);
    assert.strictEqual(document.serialise(), sedChangesExpectedSerialisation(file.childFile(1).path, ''));
  });

  test('Simulations', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasSimulations, false);
    assert.strictEqual(document.simulationCount, 0);
    assert.strictEqual(document.simulations.length, 0);
    assert.strictEqual(document.addSimulation(null), false);

    const uniform_time_course = new loc.SedUniformTimeCourse(document);
    const one_step = new loc.SedOneStep(document);
    const steady_state = new loc.SedSteadyState(document);
    const analysis = new loc.SedAnalysis(document);

    assert.strictEqual(document.addSimulation(uniform_time_course), true);
    assert.strictEqual(document.addSimulation(one_step), true);
    assert.strictEqual(document.addSimulation(steady_state), true);
    assert.strictEqual(document.addSimulation(analysis), true);

    assert.strictEqual(document.hasSimulations, true);
    assert.strictEqual(document.simulationCount, 4);
    assert.strictEqual(document.simulations.length, 4);
    assert.deepStrictEqual(document.simulations[0], uniform_time_course);
    assert.deepStrictEqual(document.simulations[1], one_step);
    assert.deepStrictEqual(document.simulations[2], steady_state);
    assert.deepStrictEqual(document.simulations[3], analysis);
    assert.deepStrictEqual(document.simulation(0), uniform_time_course);
    assert.deepStrictEqual(document.simulation(1), one_step);
    assert.deepStrictEqual(document.simulation(2), steady_state);
    assert.deepStrictEqual(document.simulation(3), analysis);
    assert.deepStrictEqual(document.simulation(4), null);

    assert.strictEqual(document.addSimulation(uniform_time_course), false);
    assert.strictEqual(document.removeSimulation(uniform_time_course), true);

    assert.strictEqual(document.addSimulation(one_step), false);
    assert.strictEqual(document.removeSimulation(one_step), true);

    assert.strictEqual(document.addSimulation(steady_state), false);
    assert.strictEqual(document.removeSimulation(steady_state), true);

    assert.strictEqual(document.addSimulation(analysis), false);
    assert.strictEqual(document.removeSimulation(analysis), true);

    assert.strictEqual(document.hasSimulations, false);
    assert.strictEqual(document.simulationCount, 0);
    assert.strictEqual(document.simulations.length, 0);

    assert.strictEqual(document.removeSimulation(null), false);
    assert.strictEqual(document.removeAllSimulations(), false);

    assert.strictEqual(document.addSimulation(uniform_time_course), true);
    assert.strictEqual(document.removeAllSimulations(), true);
  });

  test('Tasks', () => {
    const document = new loc.SedDocument();

    assert.strictEqual(document.hasTasks, false);
    assert.strictEqual(document.taskCount, 0);
    assert.strictEqual(document.tasks.length, 0);
    assert.strictEqual(document.addTask(null), false);

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));
    const model = new loc.SedModel(document, file);
    const simulation = new loc.SedUniformTimeCourse(document);
    const task = new loc.SedTask(document, model, simulation);

    assert.deepStrictEqual(task.model, model);
    assert.deepStrictEqual(task.simulation, simulation);

    assert.strictEqual(document.addTask(task), true);

    assert.strictEqual(document.hasTasks, true);
    assert.strictEqual(document.taskCount, 1);
    assert.strictEqual(document.tasks.length, 1);
    assert.deepStrictEqual(document.tasks[0], task);
    assert.deepStrictEqual(document.task(0), task);
    assert.deepStrictEqual(document.task(1), null);

    assert.strictEqual(document.serialise(), sedTaskExpectedSerialisation(true));

    task.model = null;
    task.simulation = null;

    assert.strictEqual(task.model, null);
    assert.strictEqual(task.simulation, null);

    assert.strictEqual(document.serialise(), sedTaskExpectedSerialisation(false));

    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "Task: task 'task1' requires a model."],
      [loc.Issue.Type.ERROR, "Task: task 'task1' requires a simulation."]
    ]);

    assert.strictEqual(document.addTask(task), false);
    assert.strictEqual(document.removeTask(task), true);

    assert.strictEqual(document.hasTasks, false);
    assert.strictEqual(document.taskCount, 0);
    assert.strictEqual(document.tasks.length, 0);

    assert.strictEqual(document.removeTask(null), false);
    assert.strictEqual(document.removeAllTasks(), false);

    assert.strictEqual(document.addTask(task), true);
    assert.strictEqual(document.removeAllTasks(), true);
  });

  test('ODE solver', () => {
    const document = new loc.SedDocument();
    const simulation = new loc.SedUniformTimeCourse(document);

    assert.strictEqual(simulation.odeSolver, null);

    const solver = new loc.SolverCvode();

    simulation.odeSolver = solver;

    assert.deepStrictEqual(simulation.odeSolver, solver);

    simulation.odeSolver = null;

    assert.strictEqual(simulation.odeSolver, null);
  });

  test('NLA solver', () => {
    const document = new loc.SedDocument();
    const simulation = new loc.SedUniformTimeCourse(document);

    assert.strictEqual(simulation.nlaSolver, null);

    const solver = new loc.SolverKinsol();

    simulation.nlaSolver = solver;

    assert.deepStrictEqual(simulation.nlaSolver, solver);

    simulation.nlaSolver = null;

    assert.strictEqual(simulation.nlaSolver, null);
  });

  test('SedBase', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = new loc.SedOneStep(document);

    assert.strictEqual(simulation.id, 'simulation2');
    assert.strictEqual(loc.SedBase.isValidId('my_simulation'), true);

    simulation.id = 'my_simulation';

    assert.strictEqual(simulation.id, 'my_simulation');

    // An id that is not a valid SED-ML identifier is rejected, leaving the id unchanged.
    // Note: a property setter cannot return whether the id was set, so a RangeError is thrown instead.

    for (const id of ['', 'my simulation', '1simulation', 'simulation-1', 'simulation\x01', '\udcff']) {
      assert.strictEqual(loc.SedBase.isValidId(id), false);
      assert.throws(
        () => {
          simulation.id = id;
        },
        { name: 'RangeError', message: 'The id is not a valid SED-ML identifier.' }
      );
      assert.strictEqual(simulation.id, 'my_simulation');
    }

    // A string with a lone surrogate that doesn't stand for an invalid byte (i.e. other than U+DC80 to U+DCFF) is
    // rejected.
    // Note: this has no C++ equivalent since a std::string cannot contain a lone surrogate.

    assert.throws(() => {
      loc.SedBase.isValidId('\ud800');
    }, TypeError);
    assert.throws(() => {
      simulation.isDuplicateId('\ud800');
    }, TypeError);
    assert.throws(() => {
      simulation.id = '\ud800';
    }, TypeError);
    assert.strictEqual(simulation.id, 'my_simulation');
  });

  test('SED ids', () => {
    // An id must be unique within the simulation experiment description for which an object was created.
    // Note: a property setter cannot return whether the id was set, so a RangeError is thrown instead.

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const model = document.models[0];
    const simulation = document.simulations[0];
    const task = document.tasks[0];

    assert.strictEqual(model.id, 'model1');
    assert.strictEqual(simulation.id, 'simulation1');
    assert.strictEqual(task.id, 'task1');

    // A new object is not given the id of an existing object, but it can be given an id that is not used anymore.

    model.id = 'model2';

    const otherModel = new loc.SedModel(document, file);

    assert.strictEqual(otherModel.id, 'model1');

    // An id that is already used by another object, whatever its type, is a duplicate and is therefore rejected,
    // leaving the id unchanged, even though it is a valid SED-ML identifier. The id of an object is, however, not a
    // duplicate, so an object can be given its own id.

    for (const id of ['model2', 'simulation1', 'task1']) {
      assert.strictEqual(loc.SedBase.isValidId(id), true);
      assert.strictEqual(otherModel.isDuplicateId(id), true);
      assert.throws(
        () => {
          otherModel.id = id;
        },
        { name: 'RangeError', message: 'The id is already used.' }
      );
      assert.strictEqual(otherModel.id, 'model1');
    }

    assert.strictEqual(otherModel.isDuplicateId('model1'), false);

    otherModel.id = 'model1';

    assert.strictEqual(otherModel.isDuplicateId('my_model'), false);

    // A removed object releases its id, which can then be given to another object, in which case the id of the removed
    // object is a duplicate and the removed object can only be added back once it has been given another id.

    assert.strictEqual(document.removeModel(model), true);
    assert.strictEqual(model.isDuplicateId('model2'), false);

    otherModel.id = 'model2';

    assert.strictEqual(model.isDuplicateId('model2'), true);
    assert.throws(
      () => {
        model.id = 'model2';
      },
      { name: 'RangeError', message: 'The id is already used.' }
    );
    assert.strictEqual(document.addModel(model), false);

    model.id = 'my_model';

    assert.strictEqual(document.addModel(model), true);
    assert.strictEqual(otherModel.isDuplicateId('my_model'), true);
    assert.throws(
      () => {
        otherModel.id = 'my_model';
      },
      { name: 'RangeError', message: 'The id is already used.' }
    );
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].isAliasOf(model), true);

    // A destroyed object releases its id, even if it was never added to its simulation experiment description, so that
    // its id can then be given to another object.

    const transientModel = new loc.SedModel(document, file);

    assert.strictEqual(transientModel.id, 'model1');
    assert.strictEqual(otherModel.isDuplicateId('model1'), true);

    transientModel.delete();

    assert.strictEqual(otherModel.isDuplicateId('model1'), false);

    otherModel.id = 'model1';

    assert.strictEqual(otherModel.id, 'model1');

    // An object can only be added to the simulation experiment description for which it was created.

    const otherDocument = new loc.SedDocument();

    assert.strictEqual(otherDocument.addModel(otherModel), false);
    assert.strictEqual(otherDocument.addSimulation(simulation), false);
    assert.strictEqual(otherDocument.addTask(task), false);
    assert.strictEqual(otherDocument.hasModels, false);
    assert.strictEqual(otherDocument.hasSimulations, false);
    assert.strictEqual(otherDocument.hasTasks, false);

    // An object which simulation experiment description doesn't exist anymore has no duplicate id, so it can be given
    // any valid id, but it cannot be added to another simulation experiment description.

    const orphanSimulation = new loc.SedOneStep(otherDocument);

    otherDocument.delete();

    assert.strictEqual(orphanSimulation.id, 'simulation1');
    assert.strictEqual(orphanSimulation.isDuplicateId('my_simulation'), false);

    orphanSimulation.id = 'my_simulation';

    assert.strictEqual(orphanSimulation.id, 'my_simulation');
    assert.strictEqual(document.addSimulation(orphanSimulation), false);
  });

  test('SedChangeAttribute', () => {
    // A change attribute can only be created with component and variable names that are valid CellML identifiers and
    // with a new value that is a valid double value.
    // Note: a constructor cannot return null, so a RangeError is thrown instead.

    assert.throws(() => {
      new loc.SedChangeAttribute('my component', 'variable', '1.23');
    }, RangeError);
    assert.throws(() => {
      new loc.SedChangeAttribute('component', '1variable', '1.23');
    }, RangeError);
    assert.throws(() => {
      new loc.SedChangeAttribute('component', 'variable', 'abc');
    }, RangeError);

    const changeAttribute = new loc.SedChangeAttribute('component', 'variable', '1.23');

    changeAttribute.componentName = 'new_component';
    changeAttribute.variableName = 'new_variable';

    // A new value can be any decimal number, which value must be finite once rounded to the nearest double value.

    for (const newValue of ['1.', '.5', '+1.5e-3', '-1E+3', '1e-400', '4.56']) {
      changeAttribute.newValue = newValue;

      assert.strictEqual(changeAttribute.newValue, newValue);
    }

    // A component name, variable name, or new value that is not valid is rejected, leaving it unchanged.
    // Note: a property setter cannot return whether the value was set, so a RangeError is thrown instead.

    for (const name of ['', 'my component', '1component', '_1', 'component\x01', '\udcff']) {
      assert.throws(() => {
        changeAttribute.componentName = name;
      }, RangeError);
      assert.throws(() => {
        changeAttribute.variableName = name;
      }, RangeError);
    }

    for (const newValue of ['', 'abc', '1.23abc', '1.23\x01', '\udcff', '.', '1e400', 'inf', 'nan', '0x1', ' 1']) {
      assert.throws(() => {
        changeAttribute.newValue = newValue;
      }, RangeError);
    }

    assert.strictEqual(changeAttribute.componentName, 'new_component');
    assert.strictEqual(changeAttribute.variableName, 'new_variable');
    assert.strictEqual(changeAttribute.newValue, '4.56');
    assert.strictEqual(
      changeAttribute.target,
      "/cellml:model/cellml:component[@name='new_component']/cellml:variable[@name='new_variable']"
    );
  });

  test('SedOneStep', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = new loc.SedOneStep(document);

    assert.strictEqual(simulation.step, 1.0);

    simulation.step = 1.23;

    assert.strictEqual(simulation.step, 1.23);
  });

  test('SedUniformTimeCourse', () => {
    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = new loc.SedUniformTimeCourse(document);

    assert.strictEqual(simulation.initialTime, 0.0);
    assert.strictEqual(simulation.outputStartTime, 0.0);
    assert.strictEqual(simulation.outputEndTime, 1000.0);
    assert.strictEqual(simulation.numberOfSteps, 1000);

    simulation.initialTime = 1.23;
    simulation.outputStartTime = 4.56;
    simulation.outputEndTime = 7.89;
    simulation.numberOfSteps = 10;

    assert.strictEqual(simulation.initialTime, 1.23);
    assert.strictEqual(simulation.outputStartTime, 4.56);
    assert.strictEqual(simulation.outputEndTime, 7.89);
    assert.strictEqual(simulation.numberOfSteps, 10);
  });

  test('SedInstanceAndSedInstanceTaskDifferentialModel', () => {
    const file = new loc.File(utils.resourcePath('api/solver/ode.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const solver = document.simulations[0].odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.upperHalfBandwidth = -1;

    const instance = document.instantiate();
    const instanceTask = instance.tasks[0];

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | CVODE: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.'
      ]
    ]);

    assert.strictEqual(instance.hasTasks, true);
    assert.strictEqual(instance.taskCount, 1);
    assert.deepStrictEqual(instance.task(0), instanceTask);
    assert.strictEqual(instance.task(1), null);

    assert.strictEqual(instanceTask.voi.length, 0);
    assert.strictEqual(instanceTask.voiName, 'environment/time');
    assert.strictEqual(instanceTask.voiUnit, 'millisecond');

    assert.strictEqual(instanceTask.stateCount, 4);
    assert.strictEqual(instanceTask.state(0).length, 0);
    assert.strictEqual(instanceTask.state(4).length, 0);
    assert.strictEqual(instanceTask.stateName(0), 'membrane/V');
    assert.strictEqual(instanceTask.stateName(4), '');
    assert.strictEqual(instanceTask.stateUnit(0), 'millivolt');
    assert.strictEqual(instanceTask.stateUnit(4), '');

    assert.strictEqual(instanceTask.rateCount, 4);
    assert.strictEqual(instanceTask.rate(0).length, 0);
    assert.strictEqual(instanceTask.rate(4).length, 0);
    assert.strictEqual(instanceTask.rateName(0), "membrane/V'");
    assert.strictEqual(instanceTask.rateName(4), '');
    assert.strictEqual(instanceTask.rateUnit(0), 'millivolt/millisecond');
    assert.strictEqual(instanceTask.rateUnit(4), '');

    assert.strictEqual(instanceTask.constantCount, 5);
    assert.strictEqual(instanceTask.constant(0).length, 0);
    assert.strictEqual(instanceTask.constant(5).length, 0);
    assert.strictEqual(instanceTask.constantName(0), 'membrane/Cm');
    assert.strictEqual(instanceTask.constantName(5), '');
    assert.strictEqual(instanceTask.constantUnit(0), 'microF_per_cm2');
    assert.strictEqual(instanceTask.constantUnit(5), '');

    assert.strictEqual(instanceTask.computedConstantCount, 3);
    assert.strictEqual(instanceTask.computedConstant(0).length, 0);
    assert.strictEqual(instanceTask.computedConstant(3).length, 0);
    assert.strictEqual(instanceTask.computedConstantName(0), 'leakage_current/E_L');
    assert.strictEqual(instanceTask.computedConstantName(3), '');
    assert.strictEqual(instanceTask.computedConstantUnit(0), 'millivolt');
    assert.strictEqual(instanceTask.computedConstantUnit(3), '');

    assert.strictEqual(instanceTask.algebraicVariableCount, 10);
    assert.strictEqual(instanceTask.algebraicVariable(0).length, 0);
    assert.strictEqual(instanceTask.algebraicVariable(10).length, 0);
    assert.strictEqual(instanceTask.algebraicVariableName(0), 'membrane/i_Stim');
    assert.strictEqual(instanceTask.algebraicVariableName(10), '');
    assert.strictEqual(instanceTask.algebraicVariableUnit(0), 'microA_per_cm2');
    assert.strictEqual(instanceTask.algebraicVariableUnit(10), '');

    instance.run();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task | CVODE: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.'
      ]
    ]);
  });

  test('SedInstanceAndSedInstanceTaskNonDifferentialModel', () => {
    const file = new loc.File(utils.resourcePath('api/solver/nla1.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    const instance = document.instantiate();
    const instanceTask = instance.tasks[0];

    assert.strictEqual(instanceTask.voi.length, 0);
    assert.strictEqual(instanceTask.voiName, '');
    assert.strictEqual(instanceTask.voiUnit, '');

    assert.strictEqual(instanceTask.stateCount, 0);
    assert.strictEqual(instanceTask.state(0).length, 0);
    assert.strictEqual(instanceTask.stateName(0), '');
    assert.strictEqual(instanceTask.stateUnit(0), '');

    assert.strictEqual(instanceTask.rateCount, 0);
    assert.strictEqual(instanceTask.rate(0).length, 0);
    assert.strictEqual(instanceTask.rateName(0), '');
    assert.strictEqual(instanceTask.rateUnit(0), '');
  });

  test('SedDocument', () => {
    // Note: the three following URLs use http rather than https to help with coverage testing.

    let file = new loc.File('http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml');

    new loc.SedDocument(file);

    file = new loc.File('http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml');

    new loc.SedDocument(file);

    file = new loc.File('http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex');

    new loc.SedDocument(file);
  });

  test('Solver', () => {
    // Get the duplicate() method of different solvers to be covered.

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);

    document.simulations[0].odeSolver = new loc.SolverForwardEuler();

    let instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    document.simulations[0].odeSolver = new loc.SolverFourthOrderRungeKutta();

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    document.simulations[0].odeSolver = new loc.SolverHeun();

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    document.simulations[0].odeSolver = new loc.SolverSecondOrderRungeKutta();

    instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);
  });

  test('Math', () => {
    const file = new loc.File(utils.resourcePath('api/sed/math.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();
    const instanceTask = instance.tasks[0];

    assert.strictEqual(instanceTask.constantCount, 0);
    assert.strictEqual(instanceTask.computedConstantCount, 37);
    assert.strictEqual(instanceTask.algebraicVariableCount, 0);

    instance.run();

    assertValues(
      instanceTask,
      0,
      [],
      [],
      [],
      [],
      [],
      [],
      [
        243.0,
        3.0,
        7.0,
        20.085536923187668,
        1.0986122886681098,
        0.47712125471966244,
        4.0,
        3.0,
        3.0,
        5.0,
        3.0,
        0.1411200080598672,
        -0.9899924966004454,
        -0.1425465430742778,
        -1.0101086659079939,
        7.086167395737187,
        -7.015252551434534,
        10.017874927409903,
        10.067661995777765,
        0.9950547536867305,
        0.0993279274194332,
        0.09982156966882273,
        1.0049698233136892,
        0.3046926540153975,
        1.2661036727794992,
        1.2490457723982544,
        1.2309594173407747,
        0.3398369094541219,
        0.3217505543966422,
        1.8184464592320668,
        1.762747174039086,
        0.30951960420311175,
        1.8738202425274144,
        0.32745015023725843,
        0.34657359027997264,
        Infinity,
        NaN
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
      [],
      []
    );
  });

  test('Math with VOI', () => {
    // Note: unlike in the math test, our mathematical functions are evaluated at run time rather than possibly at
    //       compile time. Also, LLVM may optimise 2^t to exp2(t), in which case exp2() must be available to our model.

    const outputEndTime = 10.0;
    const numberOfSteps = 10;

    const file = new loc.File(utils.resourcePath('api/sed/math_with_voi.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];

    simulation.outputEndTime = outputEndTime;
    simulation.numberOfSteps = numberOfSteps;

    const instance = document.instantiate();
    const instanceTask = instance.tasks[0];

    assert.strictEqual(instanceTask.stateCount, 1);
    assert.strictEqual(instanceTask.constantCount, 0);
    assert.strictEqual(instanceTask.computedConstantCount, 0);
    assert.strictEqual(instanceTask.algebraicVariableCount, 35);

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    // At t = 0, our algebraic variables have the same values as the computed constants of the math test.

    assertValues(
      instanceTask,
      0,
      [0.0],
      [7],
      [1.0],
      [7],
      [],
      [],
      [],
      [],
      [
        243.0, 3.0, 7.0, 20.085536923187668, 1.0986122886681098, 0.47712125471966244, 4.0, 3.0, 3.0, 5.0, 3.0,
        0.1411200080598672, -0.9899924966004454, -0.1425465430742778, -1.0101086659079939, 7.086167395737187,
        -7.015252551434534, 10.017874927409903, 10.067661995777765, 0.9950547536867305, 0.0993279274194332,
        0.09982156966882273, 1.0049698233136892, 0.3046926540153975, 1.2661036727794992, 1.2490457723982544,
        1.2309594173407747, 0.3398369094541219, 0.3217505543966422, 1.8184464592320668, 1.762747174039086,
        0.30951960420311175, 1.8738202425274144, 0.32745015023725843, 0.34657359027997264
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    );

    // At t = 10, x = (2^10-1)/ln(2).

    assert.ok(Math.abs(instanceTask.state(0)[numberOfSteps] - 1475.8770268294097) < 0.01);
  });

  test('KINSOL with Inf and/or NaN values', () => {
    const file = new loc.File(utils.resourcePath('api/sed/kinsol_with_inf_and_or_nan_values.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        'Task instance | KINSOL: the NLA system could not be solved (it contains some Inf and/or NaN values).'
      ]
    ]);
  });

  test('KINSOL with no solution', () => {
    const file = new loc.File(utils.resourcePath('api/sed/kinsol_with_no_solution.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    assertIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "Task instance | KINSOL: the linear solver's setup function failed in an unrecoverable manner."
      ]
    ]);
  });

  test('KINSOL with no solution over time', () => {
    // The first NLA system of our model only has a solution until t = ln(2) while its second NLA system always has a
    // solution, so our simulation should fail at t = ln(2), whether we use a fixed-step ODE solver or CVODE.
    // Note #1: we cannot check the full description of CVODE's error since it includes the step size, which may vary
    //          from one platform to another.
    // Note #2: KINSOL reuses the Jacobian of an NLA system from one solve to the next, which, with CVODE (which solves
    //          our NLA systems many times per step), results in KINSOL failing because of its line search rather than
    //          because of its maximum number of iterations.

    const expectedIssues = [
      [loc.Issue.Type.ERROR, 'Task | KINSOL: the maximum number of iterations was reached before convergence.']
    ];
    const cvodeKinsolError =
      'Task | KINSOL: the line search algorithm was unable to find an iterate sufficiently distinct from the current iterate.';
    const cvodeErrorStart = 'Task | CVODE: at t = 0.6931';
    const lastValidIndex = 6;

    const file = new loc.File(utils.resourcePath('api/sed/kinsol_with_no_solution_over_time.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations[0];
    const forwardEuler = new loc.SolverForwardEuler();

    simulation.outputEndTime = 2.0;
    simulation.numberOfSteps = 20;

    forwardEuler.step = 0.01;

    simulation.odeSolver = forwardEuler;

    let instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    assertIssues(loc, instance, expectedIssues);
    assert.strictEqual(Number.isNaN(instance.tasks[0].voi[lastValidIndex]), false);
    assert.strictEqual(Number.isNaN(instance.tasks[0].voi[lastValidIndex + 1]), true);

    simulation.odeSolver = new loc.SolverCvode();

    instance = document.instantiate();

    assert.strictEqual(instance.hasIssues, false);

    instance.run();

    const issues = instance.issues;

    assert.strictEqual(issues.length, 2);
    assert.strictEqual(issues[0].description, cvodeKinsolError);
    assert.ok(issues[1].description.startsWith(cvodeErrorStart));
    assert.strictEqual(Number.isNaN(instance.tasks[0].voi[lastValidIndex]), false);
    assert.strictEqual(Number.isNaN(instance.tasks[0].voi[lastValidIndex + 1]), true);
  });

  test('KINSOL with no solution when initialising', () => {
    // The first NLA system of our model (a^2+x = 1) has no solution for x > 1, so if the initial value of x gets set to
    // 2 after our instance has been created, then our model cannot be (re)initialised when running our instance,
    // whether we use a fixed-step ODE solver or CVODE. Our simulation should therefore stop there and we should have
    // no results to report.

    const expectedIssues = [
      [loc.Issue.Type.ERROR, "Task | KINSOL: the linear solver's setup function failed in an unrecoverable manner."]
    ];
    const numberOfSteps = 20;

    const file = new loc.File(utils.resourcePath('api/sed/kinsol_with_no_solution_over_time.cellml'));

    file.setContents(utils.fileContents(file.path));

    const forwardEuler = new loc.SolverForwardEuler();

    forwardEuler.step = 0.01;

    for (const odeSolver of [forwardEuler, new loc.SolverCvode()]) {
      const document = new loc.SedDocument(file);
      const simulation = document.simulations[0];

      simulation.outputEndTime = 2.0;
      simulation.numberOfSteps = numberOfSteps;
      simulation.odeSolver = odeSolver;

      const instance = document.instantiate();

      assert.strictEqual(instance.hasIssues, false);

      document.model(0).addChange(new loc.SedChangeAttribute('my_component', 'x', '2.0'));

      assert.strictEqual(instance.run(), 0.0);
      assertIssues(loc, instance, expectedIssues);
      assert.strictEqual(instance.progress, 0.0);

      const instanceTask = instance.tasks[0];

      assert.strictEqual(instanceTask.voi.length, numberOfSteps + 1);
      assert.strictEqual(
        instanceTask.voi.every((value) => Number.isNaN(value)),
        true
      );
      assert.strictEqual(
        instanceTask.state(0).every((value) => Number.isNaN(value)),
        true
      );
    }
  });

  test('SED-ML file with nlaAlgorithm and NLA algorithm', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_nla_algorithm_and_nla_algorithm.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the NLA solver is already set for the simulation 'simulation1'. The one specified in the nlaAlgorithm element will be ignored."
      ]
    ]);
  });

  test('SED-ML file with several nlaAlgorithms', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_several_nla_algorithms.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: multiple nlaAlgorithm elements have been found for the simulation 'simulation1'. The first one will be used."
      ]
    ]);
  });

  test('SED-ML file with unknown NLA algorithm', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_unknown_nla_algorithm.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the NLA solver 'KISAO:0000000' in simulation 'simulation1' is not recognised. The KINSOL solver will be used instead."
      ]
    ]);
  });

  test('SED-ML file branch coverage', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_branch_coverage.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    new loc.SedDocument(sedmlFile);
  });

  test('SED-ML file with no NLA algorithm', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_no_nla_algorithm.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the NLA solver '' in simulation 'simulation1' is not recognised. The KINSOL solver will be used instead."
      ]
    ]);
  });

  test('Legacy SED-ML file with NLA algorithm and NLA solver', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_nla_algorithm_and_nla_solver.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the NLA solver 'KINSOL' is specified in both the algorithm element and in the annotation of the simulation 'simulation1'. The one specified in the algorithm element will be used."
      ]
    ]);
  });

  test('Legacy SED-ML file with several NLA solvers', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_several_nla_solvers.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: multiple NLA solvers have been found in the annotation of the simulation 'simulation1'. The first one will be used."
      ]
    ]);
  });

  test('Legacy SED-ML file with unknown namespace', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_unknown_namespace.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    new loc.SedDocument(sedmlFile);
  });

  test('Legacy SED-ML file with unknown element', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_unknown_element.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    new loc.SedDocument(sedmlFile);
  });

  test('Legacy SED-ML file with unknown NLA solver', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_unknown_nla_solver.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the NLA solver 'UNKNOWN' in simulation 'simulation1' is not recognised. The KINSOL solver will be used instead."
      ]
    ]);
  });

  test('Legacy SED-ML file with unknown property namespace', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_unknown_property_namespace.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    new loc.SedDocument(sedmlFile);
  });

  test('Legacy SED-ML file with unknown property element', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_unknown_property_element.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    new loc.SedDocument(sedmlFile);
  });

  test('Legacy SED-ML file with unknown property', () => {
    const cellmlFile = new loc.File(utils.resourcePath('api/sed/dae/model.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const sedmlFile = new loc.File(utils.resourcePath('api/sed/dae/model_legacy_unknown_property.sedml'));

    sedmlFile.setContents(utils.fileContents(sedmlFile.path));

    const document = new loc.SedDocument(sedmlFile);

    assertIssues(loc, document, [
      [loc.Issue.Type.WARNING, "SED-ML file: the NLA solver property 'Unknown' is not recognised. It will be ignored."]
    ]);
  });

  test('SED-ML file with bare filename', () => {
    const file = new loc.File('cellml_2.sedml');

    file.setContents(utils.fileContents(utils.resourcePath('cellml_2.sedml')));
  });

  test('SED-ML file with invalid ids and change names', () => {
    // libSEDML doesn't reject an id that is not a valid SED-ML identifier nor a change with component and/or variable
    // names that are not valid CellML identifiers, so such an id is replaced with the one generated by libOpenCOR and
    // such a change is ignored.

    const cellmlFile = new loc.File(utils.resourcePath('cellml_2.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const file = new loc.File(utils.resourcePath('my_model.sedml'));

    file.setContents(
      new TextEncoder().encode(`<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="my model" language="urn:sedml:language:cellml" source="cellml_2.cellml">
      <listOfChanges>
        <changeAttribute target="/cellml:model/cellml:component[@name='my component']/cellml:variable[@name='x']" newValue="1.23"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='1x']" newValue="4.56"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='x']" newValue="7.89"/>
      </listOfChanges>
    </model>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="my simulation" step="1"/>
  </listOfSimulations>
</sedML>
`)
    );

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the id 'my model' of the model is not a valid SED-ML identifier. The id 'model1' has been used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the component name 'my component' for the change of type 'changeAttribute' is not a valid CellML identifier. The change has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the variable name '1x' for the change of type 'changeAttribute' is not a valid CellML identifier. The change has been ignored."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the id 'my simulation' of the simulation is not a valid SED-ML identifier. The id 'simulation1' has been used instead."
      ]
    ]);
    assert.strictEqual(document.models.length, 1);
    assert.strictEqual(document.models[0].file.isAliasOf(cellmlFile), true);
    assert.strictEqual(document.models[0].id, 'model1');
    assert.strictEqual(document.models[0].changeCount, 1);
    assert.strictEqual(document.simulations.length, 1);
    assert.strictEqual(document.simulations[0].id, 'simulation1');
  });

  test('SED-ML file with invalid ids and clashing valid ids', () => {
    // An id that is not a valid SED-ML identifier is replaced with the one generated by libOpenCOR, which must not be
    // the id of another model or simulation (e.g., "model1" and "simulation1" below).

    const cellmlFile = new loc.File(utils.resourcePath('cellml_2.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const file = new loc.File(utils.resourcePath('my_model.sedml'));

    file.setContents(
      new TextEncoder().encode(`<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="my model" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="my simulation" step="1"/>
    <oneStep id="simulation1" step="1"/>
  </listOfSimulations>
</sedML>
`)
    );

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the id 'my model' of the model is not a valid SED-ML identifier. The id 'model2' has been used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the id 'my simulation' of the simulation is not a valid SED-ML identifier. The id 'simulation2' has been used instead."
      ]
    ]);
    assert.strictEqual(document.models.length, 2);
    assert.strictEqual(document.models[0].file.isAliasOf(cellmlFile), true);
    assert.strictEqual(document.models[0].id, 'model2');
    assert.strictEqual(document.models[1].file.isAliasOf(cellmlFile), true);
    assert.strictEqual(document.models[1].id, 'model1');
    assert.strictEqual(document.simulations.length, 2);
    assert.strictEqual(document.simulations[0].id, 'simulation2');
    assert.strictEqual(document.simulations[1].id, 'simulation1');
  });

  test('SED-ML file with duplicate ids', () => {
    // An id that is already used is replaced with the one generated by libOpenCOR. This includes a model and a
    // simulation with the same id since all the ids of a SED-ML file share the same namespace.

    const cellmlFile = new loc.File(utils.resourcePath('cellml_2.cellml'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));

    const file = new loc.File(utils.resourcePath('my_model.sedml'));

    file.setContents(
      new TextEncoder().encode(`<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="my_id" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
    <model id="my_id" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="my_id" step="1"/>
    <oneStep id="my_simulation" step="1"/>
  </listOfSimulations>
</sedML>
`)
    );

    const document = new loc.SedDocument(file);

    assertIssues(loc, document, [
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the id 'my_id' of the model is already used. The id 'model1' has been used instead."
      ],
      [
        loc.Issue.Type.WARNING,
        "SED-ML file: the id 'my_id' of the simulation is already used. The id 'simulation1' has been used instead."
      ]
    ]);
    assert.strictEqual(document.models.length, 2);
    assert.strictEqual(document.models[0].id, 'my_id');
    assert.strictEqual(document.models[1].id, 'model1');
    assert.strictEqual(document.simulations.length, 2);
    assert.strictEqual(document.simulations[0].id, 'simulation1');
    assert.strictEqual(document.simulations[1].id, 'my_simulation');

    // The ids of the SED-ML file are only reserved while it is being imported.

    assert.strictEqual(new loc.SedOneStep(document).id, 'simulation2');
  });

  test('Simulation results are of Float64Array type', () => {
    // Note: this has no C++ equivalent since the type of our simulation results is known at compile time in C++.

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    assert.strictEqual(instance.hasIssues, false);

    const instanceTask = instance.tasks[0];

    assert.strictEqual(instanceTask.voi instanceof Float64Array, true);
    assert.strictEqual(instanceTask.state(0) instanceof Float64Array, true);
    assert.strictEqual(instanceTask.rate(0) instanceof Float64Array, true);
    assert.strictEqual(instanceTask.constant(0) instanceof Float64Array, true);
    assert.strictEqual(instanceTask.computedConstant(0) instanceof Float64Array, true);
    assert.strictEqual(instanceTask.algebraicVariable(0) instanceof Float64Array, true);
  });
});
