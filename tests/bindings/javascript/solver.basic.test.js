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

    expect(solversInfo.size()).toBe(1);

    const solverInfo = solversInfo.get(0);

    expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solverInfo.name()).toBe("Forward Euler");

    const properties = solverInfo.properties();

    expect(properties.size()).toBe(1);

    const property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGt0.value,
    );
    expect(property.name()).toBe("Step");
    expect(property.defaultValue()).toBe("1.000000");
    expect(property.hasVoiUnit()).toBe(true);

    const listValues = property.listValues();

    expect(listValues.size()).toBe(0);
  });

  test("Unknown", () => {
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

  test("Properties", () => {
    const solver = new libopencor.Solver("Forward Euler");

    expect(solver.properties().size()).toBe(0);

    solver.setProperty("Property #1", "1.0");
    solver.setProperty("Property #2", "2.0");
    solver.setProperty("Property #3", "3.0");

    expect(solver.properties().size()).toBe(3);

    const properties = solver.properties();

    solver.setProperty("Property #4", "4.0");
    solver.setProperty("Property #5", "5.0");

    expect(solver.properties().size()).toBe(5);

    solver.setProperties(properties);

    expect(solver.properties().size()).toBe(3);

    expect(solver.property("Property #1")).toBe("1.0");
    expect(solver.property("Property #2")).toBe("2.0");
    expect(solver.property("Property #3")).toBe("3.0");
  });
});
