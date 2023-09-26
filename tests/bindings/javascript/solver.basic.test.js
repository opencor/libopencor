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

const libopencor = await libOpenCOR();

describe("Solver basic tests", () => {
  test("Solvers info", () => {
    const solversInfo = libopencor.Solver.solversInfo();

    expect(solversInfo.size()).toBe(4);

    // Forward Euler.

    let solverInfo = solversInfo.get(0);

    expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solverInfo.name()).toBe("Forward Euler");
    expect(solverInfo.kisaoId()).toBe("KISAO:0000030");

    let properties = solverInfo.properties();

    expect(properties.size()).toBe(1);

    let property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGt0.value,
    );
    expect(property.name()).toBe("Step");
    expect(property.kisaoId()).toBe("KISAO:0000483");
    expect(property.defaultValue()).toBe("1.000000");
    expect(property.hasVoiUnit()).toBe(true);

    let listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    // Fourth-order Runge-Kutta.

    solverInfo = solversInfo.get(1);

    expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solverInfo.name()).toBe("Fourth-order Runge-Kutta");
    expect(solverInfo.kisaoId()).toBe("KISAO:0000032");

    properties = solverInfo.properties();

    expect(properties.size()).toBe(1);

    property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGt0.value,
    );
    expect(property.name()).toBe("Step");
    expect(property.kisaoId()).toBe("KISAO:0000483");
    expect(property.defaultValue()).toBe("1.000000");
    expect(property.hasVoiUnit()).toBe(true);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    // Heun.

    solverInfo = solversInfo.get(2);

    expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solverInfo.name()).toBe("Heun");
    expect(solverInfo.kisaoId()).toBe("KISAO:0000301");

    properties = solverInfo.properties();

    expect(properties.size()).toBe(1);

    property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGt0.value,
    );
    expect(property.name()).toBe("Step");
    expect(property.kisaoId()).toBe("KISAO:0000483");
    expect(property.defaultValue()).toBe("1.000000");
    expect(property.hasVoiUnit()).toBe(true);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    // Second-order Runge-Kutta.

    solverInfo = solversInfo.get(3);

    expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solverInfo.name()).toBe("Second-order Runge-Kutta");
    expect(solverInfo.kisaoId()).toBe("KISAO:0000381");

    properties = solverInfo.properties();

    expect(properties.size()).toBe(1);

    property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGt0.value,
    );
    expect(property.name()).toBe("Step");
    expect(property.kisaoId()).toBe("KISAO:0000483");
    expect(property.defaultValue()).toBe("1.000000");
    expect(property.hasVoiUnit()).toBe(true);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);
  });

  test("Unknown solver", () => {
    const solver = new libopencor.Solver("Unknown");

    expect(solver.isValid()).toBe(false);
  });

  test("Forward Euler by name", () => {
    const solver = new libopencor.Solver("Forward Euler");

    expect(solver.isValid()).toBe(true);
  });

  test("Forward Euler by KiSAO id", () => {
    const solver = new libopencor.Solver("KISAO:0000030");

    expect(solver.isValid()).toBe(true);
  });

  test("Fourth-order Runge-Kutta by name", () => {
    const solver = new libopencor.Solver("Fourth-order Runge-Kutta");

    expect(solver.isValid()).toBe(true);
  });

  test("Fourth-order Runge-Kutta by KiSAO id", () => {
    const solver = new libopencor.Solver("KISAO:0000032");

    expect(solver.isValid()).toBe(true);
  });

  test("Heun by name", () => {
    const solver = new libopencor.Solver("Heun");

    expect(solver.isValid()).toBe(true);
  });

  test("Heun by KiSAO id", () => {
    const solver = new libopencor.Solver("KISAO:0000301");

    expect(solver.isValid()).toBe(true);
  });

  test("Second-order Runge-Kutta by name", () => {
    const solver = new libopencor.Solver("Second-order Runge-Kutta");

    expect(solver.isValid()).toBe(true);
  });

  test("Second-order Runge-Kutta by KiSAO id", () => {
    const solver = new libopencor.Solver("KISAO:0000381");

    expect(solver.isValid()).toBe(true);
  });

  test("Properties", () => {
    const solver = new libopencor.Solver("Forward Euler");
    const properties = solver.properties();

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("1.000000");
    expect(solver.property("KISAO:0000483")).toBe("1.000000");

    solver.setProperty("Step", "1.2345");

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("KISAO:0000483")).toBe("1.2345");

    solver.setProperty("KISAO:0000483", "7.89");

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("7.89");

    solver.setProperty("Unknown property", "1.23");

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("7.89");
    expect(solver.property("Unknown property")).toBe("");

    solver.setProperties(properties);

    expect(solver.properties().size()).toBe(1);
    expect(solver.property("Step")).toBe("1.000000");
  });
});
