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

const loc = await libOpenCOR();

test.describe('Sed concurrent tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Parallel independent instances', () => {
    const INSTANCE_COUNT = 3;
    const SIMULATION_PROPERTY = 10000;

    const instances = [];

    for (let i = 0; i < INSTANCE_COUNT; ++i) {
      const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

      file.setContents(utils.fileContents(file.path));

      const document = new loc.SedDocument(file);
      const simulation = document.simulations.get(0);

      simulation.numberOfSteps = SIMULATION_PROPERTY;
      simulation.outputEndTime = SIMULATION_PROPERTY;

      instances.push(document.instantiate());
    }

    for (const instance of instances) {
      assert.strictEqual(instance.startRun(), true);
    }

    for (const instance of instances) {
      assert.ok(instance.waitForRun() > 0.0);
    }

    for (const instance of instances) {
      assert.strictEqual(instance.progress, 1.0);
      assert.strictEqual(instance.hasIssues, false);
    }
  });

  test('Parallel shared document', () => {
    const SIMULATION_PROPERTY = 10000;

    const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

    file.setContents(utils.fileContents(file.path));

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);

    simulation.numberOfSteps = SIMULATION_PROPERTY;
    simulation.outputEndTime = SIMULATION_PROPERTY;

    const instance1 = document.instantiate();
    const instance2 = document.instantiate();
    const instance3 = document.instantiate();

    assert.strictEqual(instance1.startRun(), true);
    assert.strictEqual(instance2.startRun(), true);
    assert.strictEqual(instance3.startRun(), true);

    assert.ok(instance1.waitForRun() > 0.0);
    assert.ok(instance2.waitForRun() > 0.0);
    assert.ok(instance3.waitForRun() > 0.0);

    assert.strictEqual(instance1.progress, 1.0);
    assert.strictEqual(instance2.progress, 1.0);
    assert.strictEqual(instance3.progress, 1.0);

    assert.strictEqual(instance1.hasIssues, false);
    assert.strictEqual(instance2.hasIssues, false);
    assert.strictEqual(instance3.hasIssues, false);
  });

  test('Parallel async lifecycle', () => {
    const INSTANCE_COUNT = 3;
    const SIMULATION_PROPERTY = 10000;

    const instances = [];

    for (let i = 0; i < INSTANCE_COUNT; ++i) {
      const file = new loc.File(utils.resourcePath('cellml_2.cellml'));

      file.setContents(utils.fileContents(file.path));

      const document = new loc.SedDocument(file);
      const simulation = document.simulations.get(0);

      simulation.numberOfSteps = SIMULATION_PROPERTY;
      simulation.outputEndTime = SIMULATION_PROPERTY;

      instances.push(document.instantiate());
    }

    for (const instance of instances) {
      assert.strictEqual(instance.startRun(), true);
    }

    for (const instance of instances) {
      assert.ok(instance.waitForRun() > 0.0);
    }

    for (const instance of instances) {
      assert.strictEqual(instance.status, loc.SedInstance.Status.IDLE);
      assert.strictEqual(instance.hasIssues, false);
    }
  });
});
