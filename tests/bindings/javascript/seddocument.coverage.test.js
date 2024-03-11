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

describe("SedDocument coverage tests", () => {
  test("Initialise", () => {
    const expectedSerialisation = `<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
`;
    const sed = new libopencor.SedDocument();

    expect(sed.serialise()).toBe(expectedSerialisation);
  });

  test("Model", () => {
    const sed = new libopencor.SedDocument();

    expect(sed.models().size()).toBe(0);
    expect(sed.addModel(null)).toBe(false);

    const file = new libopencor.File(utils.LOCAL_FILE);
    const model = new libopencor.SedModel(file, sed);

    expect(sed.addModel(model)).toBe(true);

    expect(sed.models().size()).toBe(1);
    expect(sed.models().get(0)).toStrictEqual(model);

    expect(sed.addModel(model)).toBe(false);
    expect(sed.removeModel(model)).toBe(true);

    expect(sed.models().size()).toBe(0);

    expect(sed.removeModel(null)).toBe(false);
  });

  test("Simulations", () => {
    const sed = new libopencor.SedDocument();

    expect(sed.simulations().size()).toBe(0);
    expect(sed.addSimulation(null)).toBe(false);

    const simulation = new libopencor.SedSimulationUniformTimeCourse(sed);

    expect(sed.addSimulation(simulation)).toBe(true);

    expect(sed.simulations().size()).toBe(1);
    expect(sed.simulations().get(0)).toStrictEqual(simulation);

    expect(sed.addSimulation(simulation)).toBe(false);
    expect(sed.removeSimulation(simulation)).toBe(true);

    expect(sed.simulations().size()).toBe(0);

    expect(sed.removeSimulation(null)).toBe(false);
  });

  test("ODE solver", () => {
    const sed = new libopencor.SedDocument();
    const simulation = new libopencor.SedSimulationUniformTimeCourse(sed);

    expect(simulation.odeSolver()).toBe(null);

    const solver = new libopencor.SolverCvode();

    simulation.setOdeSolver(solver);

    expect(simulation.odeSolver()).toStrictEqual(solver);

    simulation.setOdeSolver(null);

    expect(simulation.odeSolver()).toBe(null);
  });

  test("NLA solver", () => {
    const sed = new libopencor.SedDocument();
    const simulation = new libopencor.SedSimulationUniformTimeCourse(sed);

    expect(simulation.nlaSolver()).toBe(null);

    const solver = new libopencor.SolverKinsol();

    simulation.setNlaSolver(solver);

    expect(simulation.nlaSolver()).toStrictEqual(solver);

    simulation.setNlaSolver(null);

    expect(simulation.nlaSolver()).toBe(null);
  });

  test("SedSimulationOneStep", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = new libopencor.SedSimulationOneStep(sed);

    expect(simulation.step()).toBe(1.0);

    simulation.setStep(1.23);

    expect(simulation.step()).toBe(1.23);
  });

  test("SedSimulationUniformTimeCourse", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);
    const sed = new libopencor.SedDocument(file);
    const simulation = new libopencor.SedSimulationUniformTimeCourse(sed);

    expect(simulation.initialTime()).toBe(0.0);
    expect(simulation.outputStartTime()).toBe(0.0);
    expect(simulation.outputEndTime()).toBe(1000.0);
    expect(simulation.numberOfSteps()).toBe(1000);

    simulation.setInitialTime(1.23);
    simulation.setOutputStartTime(4.56);
    simulation.setOutputEndTime(7.89);
    simulation.setNumberOfSteps(10);

    expect(simulation.initialTime()).toBe(1.23);
    expect(simulation.outputStartTime()).toBe(4.56);
    expect(simulation.outputEndTime()).toBe(7.89);
    expect(simulation.numberOfSteps()).toBe(10);
  });
});
