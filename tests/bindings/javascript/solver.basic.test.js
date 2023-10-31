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
  test("CVODE by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000019");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000019");
    expect(solver.name()).toBe("CVODE");

    expect(solver).not.toBe(null);
  });

  test("CVODE by name", () => {
    const solver = new libopencor.Solver("CVODE");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000019");
    expect(solver.name()).toBe("CVODE");

    expect(solver).not.toBe(null);
  });

  test("Forward Euler by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000030");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000030");
    expect(solver.name()).toBe("Forward Euler");

    expect(solver).not.toBe(null);
  });

  test("Forward Euler by name", () => {
    const solver = new libopencor.Solver("Forward Euler");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000030");
    expect(solver.name()).toBe("Forward Euler");

    expect(solver).not.toBe(null);
  });

  test("Fourth-order Runge-Kutta by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000032");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000032");
    expect(solver.name()).toBe("Fourth-order Runge-Kutta");

    expect(solver).not.toBe(null);
  });

  test("Fourth-order Runge-Kutta by name", () => {
    const solver = new libopencor.Solver("Fourth-order Runge-Kutta");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000032");
    expect(solver.name()).toBe("Fourth-order Runge-Kutta");

    expect(solver).not.toBe(null);
  });

  test("Heun by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000301");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000301");
    expect(solver.name()).toBe("Heun");

    expect(solver).not.toBe(null);
  });

  test("Heun by name", () => {
    const solver = new libopencor.Solver("Heun");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000301");
    expect(solver.name()).toBe("Heun");

    expect(solver).not.toBe(null);
  });

  test("KINSOL by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000282");

    expect(solver.type().value).toBe(libopencor.Solver.Type.NLA.value);
    expect(solver.id()).toBe("KISAO:0000282");
    expect(solver.name()).toBe("KINSOL");

    expect(solver).not.toBe(null);
  });

  test("KINSOL by name", () => {
    const solver = new libopencor.Solver("KINSOL");

    expect(solver.type().value).toBe(libopencor.Solver.Type.NLA.value);
    expect(solver.id()).toBe("KISAO:0000282");
    expect(solver.name()).toBe("KINSOL");

    expect(solver).not.toBe(null);
  });

  test("Second-order Runge-Kutta by (KiSAO) id", () => {
    const solver = new libopencor.Solver("KISAO:0000381");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000381");
    expect(solver.name()).toBe("Second-order Runge-Kutta");

    expect(solver).not.toBe(null);
  });

  test("Second-order Runge-Kutta by name", () => {
    const solver = new libopencor.Solver("Second-order Runge-Kutta");

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000381");
    expect(solver.name()).toBe("Second-order Runge-Kutta");

    expect(solver).not.toBe(null);
  });
});
