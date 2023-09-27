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

    expect(solversInfo.size()).toBe(5);

    // CVODE.

    let solverInfo = solversInfo.get(0);

    expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solverInfo.name()).toBe("CVODE");
    expect(solverInfo.kisaoId()).toBe("KISAO:0000019");

    let properties = solverInfo.properties();

    expect(properties.size()).toBe(11);

    let property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGe0.value,
    );
    expect(property.name()).toBe("Maximum step");
    expect(property.kisaoId()).toBe("KISAO:0000467");
    expect(property.defaultValue()).toBe("0");
    expect(property.hasVoiUnit()).toBe(true);

    let listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    property = properties.get(1);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.IntegerGt0.value,
    );
    expect(property.name()).toBe("Maximum number of steps");
    expect(property.kisaoId()).toBe("KISAO:0000415");
    expect(property.defaultValue()).toBe("500");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    property = properties.get(2);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.List.value,
    );
    expect(property.name()).toBe("Integration method");
    expect(property.kisaoId()).toBe("KISAO:0000475");
    expect(property.defaultValue()).toBe("BDF");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(2);
    expect(listValues.get(0)).toBe("Adams-Moulton");
    expect(listValues.get(1)).toBe("BDF");

    property = properties.get(3);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.List.value,
    );
    expect(property.name()).toBe("Iteration type");
    expect(property.kisaoId()).toBe("KISAO:0000476");
    expect(property.defaultValue()).toBe("Newton");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(2);
    expect(listValues.get(0)).toBe("Functional");
    expect(listValues.get(1)).toBe("Newton");

    property = properties.get(4);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.List.value,
    );
    expect(property.name()).toBe("Linear solver");
    expect(property.kisaoId()).toBe("KISAO:0000477");
    expect(property.defaultValue()).toBe("Dense");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(6);
    expect(listValues.get(0)).toBe("Dense");
    expect(listValues.get(1)).toBe("Banded");
    expect(listValues.get(2)).toBe("Diagonal");
    expect(listValues.get(3)).toBe("GMRES");
    expect(listValues.get(4)).toBe("BiCGStab");
    expect(listValues.get(5)).toBe("TFQMR");

    property = properties.get(5);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.List.value,
    );
    expect(property.name()).toBe("Preconditioner");
    expect(property.kisaoId()).toBe("KISAO:0000478");
    expect(property.defaultValue()).toBe("Banded");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(2);
    expect(listValues.get(0)).toBe("None");
    expect(listValues.get(1)).toBe("Banded");

    property = properties.get(6);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.IntegerGe0.value,
    );
    expect(property.name()).toBe("Upper half-bandwidth");
    expect(property.kisaoId()).toBe("KISAO:0000479");
    expect(property.defaultValue()).toBe("0");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    property = properties.get(7);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.IntegerGe0.value,
    );
    expect(property.name()).toBe("Lower half-bandwidth");
    expect(property.kisaoId()).toBe("KISAO:0000480");
    expect(property.defaultValue()).toBe("0");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    property = properties.get(8);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGe0.value,
    );
    expect(property.name()).toBe("Relative tolerance");
    expect(property.kisaoId()).toBe("KISAO:0000209");
    expect(property.defaultValue()).toBe("1e-07");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    property = properties.get(9);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGe0.value,
    );
    expect(property.name()).toBe("Absolute tolerance");
    expect(property.kisaoId()).toBe("KISAO:0000211");
    expect(property.defaultValue()).toBe("1e-07");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    property = properties.get(10);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.Boolean.value,
    );
    expect(property.name()).toBe("Interpolate solution");
    expect(property.kisaoId()).toBe("KISAO:0000481");
    expect(property.defaultValue()).toBe("True");
    expect(property.hasVoiUnit()).toBe(false);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    // Forward Euler.

    solverInfo = solversInfo.get(1);

    expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solverInfo.name()).toBe("Forward Euler");
    expect(solverInfo.kisaoId()).toBe("KISAO:0000030");

    properties = solverInfo.properties();

    expect(properties.size()).toBe(1);

    property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGt0.value,
    );
    expect(property.name()).toBe("Step");
    expect(property.kisaoId()).toBe("KISAO:0000483");
    expect(property.defaultValue()).toBe("1");
    expect(property.hasVoiUnit()).toBe(true);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    // Fourth-order Runge-Kutta.

    solverInfo = solversInfo.get(2);

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
    expect(property.defaultValue()).toBe("1");
    expect(property.hasVoiUnit()).toBe(true);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    // Heun.

    solverInfo = solversInfo.get(3);

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
    expect(property.defaultValue()).toBe("1");
    expect(property.hasVoiUnit()).toBe(true);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);

    // Second-order Runge-Kutta.

    solverInfo = solversInfo.get(4);

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
    expect(property.defaultValue()).toBe("1");
    expect(property.hasVoiUnit()).toBe(true);

    listValues = property.listValues();

    expect(listValues.size()).toBe(0);
  });

  test("Unknown solver", () => {
    const solver = new libopencor.Solver("Unknown");

    expect(solver.isValid()).toBe(false);
  });

  test("CVODE by name", () => {
    const solver = new libopencor.Solver("CVODE");

    expect(solver.isValid()).toBe(true);
  });

  test("CVODE by KiSAO id", () => {
    const solver = new libopencor.Solver("KISAO:0000019");

    expect(solver.isValid()).toBe(true);
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
    expect(solver.property("Step")).toBe("1");
    expect(solver.property("KISAO:0000483")).toBe("1");

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
    expect(solver.property("Step")).toBe("1");
  });
});
