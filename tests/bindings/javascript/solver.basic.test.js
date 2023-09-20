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
    expect(solverInfo.kisaoId()).toBe("KISAO:0000030");

    const properties = solverInfo.properties();

    expect(properties.size()).toBe(1);

    const property = properties.get(0);

    expect(property.type().value).toBe(
      libopencor.SolverProperty.Type.DoubleGt0.value,
    );
    expect(property.name()).toBe("Step");
    expect(property.kisaoId()).toBe("KISAO:0000483");
    expect(property.defaultValue()).toBe("1.000000");
    expect(property.hasVoiUnit()).toBe(true);

    const listValues = property.listValues();

    expect(listValues.size()).toBe(0);
  });

  test("Unknown", () => {
    const odeSolver = new libopencor.SolverOde("Unknown");

    expect(odeSolver.isValid()).toBe(false);
  });

  test("Forward Euler by name", () => {
    const odeSolver = new libopencor.SolverOde("Forward Euler");

    expect(odeSolver.isValid()).toBe(true);
  });

  test("Forward Euler by KiSAO id", () => {
    const odeSolver = new libopencor.SolverOde("KISAO:0000030");

    expect(odeSolver.isValid()).toBe(true);
  });

  test("Properties", () => {
    const odeSolver = new libopencor.SolverOde("Forward Euler");
    const properties = odeSolver.properties();

    expect(odeSolver.properties().size()).toBe(1);
    expect(odeSolver.property("Step")).toBe("1.000000");
    expect(odeSolver.property("KISAO:0000483")).toBe("1.000000");

    odeSolver.setProperty("Step", "1.2345");

    expect(odeSolver.properties().size()).toBe(1);
    expect(odeSolver.property("KISAO:0000483")).toBe("1.2345");

    odeSolver.setProperty("KISAO:0000483", "7.89");

    expect(odeSolver.properties().size()).toBe(1);
    expect(odeSolver.property("Step")).toBe("7.89");

    odeSolver.setProperty("Unknown property", "1.23");

    expect(odeSolver.properties().size()).toBe(1);
    expect(odeSolver.property("Step")).toBe("7.89");
    expect(odeSolver.property("Unknown property")).toBe("");

    odeSolver.setProperties(properties);

    expect(odeSolver.properties().size()).toBe(1);
    expect(odeSolver.property("Step")).toBe("1.000000");
  });
});
