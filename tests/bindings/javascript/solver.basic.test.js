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

const loc = await libOpenCOR();

describe("Solver basic tests", () => {
  test("CVODE solver", () => {
    const solver = new loc.SolverCvode();

    expect(solver.type.value).toBe(loc.Solver.Type.ODE.value);
    expect(solver.id).toBe("KISAO:0000019");
    expect(solver.name).toBe("CVODE");

    expect(solver.maximumStep).toBe(0.0);
    expect(solver.maximumNumberOfSteps).toBe(500);
    expect(solver.integrationMethod).toBe(
      loc.SolverCvode.IntegrationMethod.BDF,
    );
    expect(solver.iterationType).toBe(loc.SolverCvode.IterationType.NEWTON);
    expect(solver.linearSolver).toBe(loc.SolverCvode.LinearSolver.DENSE);
    expect(solver.preconditioner).toBe(loc.SolverCvode.Preconditioner.BANDED);
    expect(solver.upperHalfBandwidth).toBe(0);
    expect(solver.lowerHalfBandwidth).toBe(0);
    expect(solver.relativeTolerance).toBe(1.0e-7);
    expect(solver.absoluteTolerance).toBe(1.0e-7);
    expect(solver.interpolateSolution).toBe(true);

    solver.maximumStep = 1.23;
    solver.maximumNumberOfSteps = 123;
    solver.integrationMethod = loc.SolverCvode.IntegrationMethod.ADAMS_MOULTON;
    solver.iterationType = loc.SolverCvode.IterationType.FUNCTIONAL;
    solver.linearSolver = loc.SolverCvode.LinearSolver.GMRES;
    solver.preconditioner = loc.SolverCvode.Preconditioner.NO;
    solver.upperHalfBandwidth = 3;
    solver.lowerHalfBandwidth = 5;
    solver.relativeTolerance = 1.23e-5;
    solver.absoluteTolerance = 3.45e-7;
    solver.interpolateSolution = false;

    expect(solver.maximumStep).toBe(1.23);
    expect(solver.maximumNumberOfSteps).toBe(123);
    expect(solver.integrationMethod).toBe(
      loc.SolverCvode.IntegrationMethod.ADAMS_MOULTON,
    );
    expect(solver.iterationType).toBe(loc.SolverCvode.IterationType.FUNCTIONAL);
    expect(solver.linearSolver).toBe(loc.SolverCvode.LinearSolver.GMRES);
    expect(solver.preconditioner).toBe(loc.SolverCvode.Preconditioner.NO);
    expect(solver.upperHalfBandwidth).toBe(3);
    expect(solver.lowerHalfBandwidth).toBe(5);
    expect(solver.relativeTolerance).toBe(1.23e-5);
    expect(solver.absoluteTolerance).toBe(3.45e-7);
    expect(solver.interpolateSolution).toBe(false);

    solver.delete();
  });

  test("Forward Euler solver", () => {
    const solver = new loc.SolverForwardEuler();

    expect(solver.type.value).toBe(loc.Solver.Type.ODE.value);
    expect(solver.id).toBe("KISAO:0000030");
    expect(solver.name).toBe("Forward Euler");

    expect(solver.step).toBe(1.0);

    solver.step = 0.123;

    expect(solver.step).toBe(0.123);

    solver.delete();
  });

  test("Fourth-order Runge-Kutta solver", () => {
    const solver = new loc.SolverFourthOrderRungeKutta();

    expect(solver.type.value).toBe(loc.Solver.Type.ODE.value);
    expect(solver.id).toBe("KISAO:0000032");
    expect(solver.name).toBe("Fourth-order Runge-Kutta");

    expect(solver.step).toBe(1.0);

    solver.step = 0.123;

    expect(solver.step).toBe(0.123);

    solver.delete();
  });

  test("Heun solver", () => {
    const solver = new loc.SolverHeun();

    expect(solver.type.value).toBe(loc.Solver.Type.ODE.value);
    expect(solver.id).toBe("KISAO:0000301");
    expect(solver.name).toBe("Heun");

    expect(solver.step).toBe(1.0);

    solver.step = 0.123;

    expect(solver.step).toBe(0.123);

    solver.delete();
  });

  test("KINSOL solver", () => {
    const solver = new loc.SolverKinsol();

    expect(solver.type.value).toBe(loc.Solver.Type.NLA.value);
    expect(solver.id).toBe("KISAO:0000282");
    expect(solver.name).toBe("KINSOL");

    expect(solver.maximumNumberOfIterations).toBe(200);
    expect(solver.linearSolver).toBe(loc.SolverKinsol.LinearSolver.DENSE);
    expect(solver.upperHalfBandwidth).toBe(0);
    expect(solver.lowerHalfBandwidth).toBe(0);

    solver.maximumNumberOfIterations = 123;
    solver.linearSolver = loc.SolverKinsol.LinearSolver.GMRES;
    solver.upperHalfBandwidth = 3;
    solver.lowerHalfBandwidth = 5;

    expect(solver.maximumNumberOfIterations).toBe(123);
    expect(solver.linearSolver).toBe(loc.SolverKinsol.LinearSolver.GMRES);
    expect(solver.upperHalfBandwidth).toBe(3);
    expect(solver.lowerHalfBandwidth).toBe(5);

    solver.delete();
  });

  test("Second-order Runge-Kutta solver", () => {
    const solver = new loc.SolverSecondOrderRungeKutta();

    expect(solver.type.value).toBe(loc.Solver.Type.ODE.value);
    expect(solver.id).toBe("KISAO:0000381");
    expect(solver.name).toBe("Second-order Runge-Kutta");

    expect(solver.step).toBe(1.0);

    solver.step = 0.123;

    expect(solver.step).toBe(0.123);

    solver.delete();
  });
});
