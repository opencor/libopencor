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
import {
  libopencor,
  checkCvodeSolver,
  checkForwardEulerSolver,
  checkFourthOrderRungeKuttaSolver,
  checkHeunSolver,
  checkKinsolSolver,
  checkSecondOrderRungeKuttaSolver,
} from "./solvers.js";

describe("Solver basic tests", () => {
  test("Solvers info", () => {
    const solversInfo = libopencor.Solver.solversInfo();

    expect(solversInfo.size()).toBe(6);

    checkCvodeSolver(solversInfo.get(0));
    checkForwardEulerSolver(solversInfo.get(1));
    checkFourthOrderRungeKuttaSolver(solversInfo.get(2));
    checkHeunSolver(solversInfo.get(3));
    checkKinsolSolver(solversInfo.get(4));
    checkSecondOrderRungeKuttaSolver(solversInfo.get(5));
  });

  test("Unknown solver", () => {
    const solver = new libopencor.Solver("Unknown");

    expect(solver.isValid()).toBe(false);
  });

  test("CVODE by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000019");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000019");
    expect(solver.name()).toBe("CVODE");

    expect(solver.isValid()).toBe(true);
  });

  test("CVODE by name", () => {
    const solver = new libopencor.Solver("CVODE");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000019");
    expect(solver.name()).toBe("CVODE");

    expect(solver.isValid()).toBe(true);
  });

  test("Forward Euler by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000030");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000030");
    expect(solver.name()).toBe("Forward Euler");

    expect(solver.isValid()).toBe(true);
  });

  test("Forward Euler by name", () => {
    const solver = new libopencor.Solver("Forward Euler");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000030");
    expect(solver.name()).toBe("Forward Euler");

    expect(solver.isValid()).toBe(true);
  });

  test("Fourth-order Runge-Kutta by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000032");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000032");
    expect(solver.name()).toBe("Fourth-order Runge-Kutta");

    expect(solver.isValid()).toBe(true);
  });

  test("Fourth-order Runge-Kutta by name", () => {
    const solver = new libopencor.Solver("Fourth-order Runge-Kutta");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000032");
    expect(solver.name()).toBe("Fourth-order Runge-Kutta");

    expect(solver.isValid()).toBe(true);
  });

  test("Heun by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000301");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000301");
    expect(solver.name()).toBe("Heun");

    expect(solver.isValid()).toBe(true);
  });

  test("Heun by name", () => {
    const solver = new libopencor.Solver("Heun");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000301");
    expect(solver.name()).toBe("Heun");

    expect(solver.isValid()).toBe(true);
  });

  test("KINSOL by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000282");

    expect(solver.type().value).toBe(libopencor.Solver.Type.NLA.value);
    expect(solver.id()).toBe("KISAO:0000282");
    expect(solver.name()).toBe("KINSOL");

    expect(solver.isValid()).toBe(true);
  });

  test("KINSOL by name", () => {
    const solver = new libopencor.Solver("KINSOL");

    expect(solver.type().value).toBe(libopencor.Solver.Type.NLA.value);
    expect(solver.id()).toBe("KISAO:0000282");
    expect(solver.name()).toBe("KINSOL");

    expect(solver.isValid()).toBe(true);
  });

  test("Second-order Runge-Kutta by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000381");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000381");
    expect(solver.name()).toBe("Second-order Runge-Kutta");

    expect(solver.isValid()).toBe(true);
  });

  test("Second-order Runge-Kutta by name", () => {
    const solver = new libopencor.Solver("Second-order Runge-Kutta");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000381");
    expect(solver.name()).toBe("Second-order Runge-Kutta");

    expect(solver.isValid()).toBe(true);
  });

  test("Properties", () => {
    const solver = new libopencor.Solver("Forward Euler");
    let properties = solver.properties();

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("KISAO:0000483")).toBe("1");
    expect(solver.property("Step")).toBe("1");

    solver.setProperty("Step", "1.23");

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("KISAO:0000483")).toBe("1.23");

    solver.setProperty("KISAO:0000483", "7.89");

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("7.89");

    solver.setProperty("Unknown", "1.23");

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("7.89");
    expect(solver.property("Unknown")).toBe("");

    properties.set("Step", "1.23");

    expect(properties.size()).toBe(2);

    solver.setProperties(properties);

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("1");

    properties.set("Unknown", "1.23");

    expect(properties.size()).toBe(3);

    solver.setProperties(properties);

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("1");

    properties.set("KISAO:0000483", "1.23");

    expect(properties.size()).toBe(3);

    solver.setProperties(properties);

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("1.23");

    // Note: here, we want to remove "KISAO:0000483" entry from properties, but it cannot be done using the JavaScript
    // bindings, so we create a new Properties object instead and add the "Unknonwn" and "Step" entries to it.

    properties = new libopencor.StringStringMap();

    properties.set("Unknown", "1.23");
    properties.set("Step", "1.23");

    expect(properties.size()).toBe(2);

    solver.setProperties(properties);

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("1.23");
  });
});
