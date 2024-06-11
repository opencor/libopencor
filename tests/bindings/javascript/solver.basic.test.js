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
  test("CVODE solver", () => {
    const solver = new libopencor.SolverCvode();

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000019");
    expect(solver.name()).toBe("CVODE");

    expect(solver.maximumStep()).toBe(0.0);
    expect(solver.maximumNumberOfSteps()).toBe(500);
    expect(solver.integrationMethod()).toBe(
      libopencor.SolverCvode.IntegrationMethod.BDF,
    );
    expect(solver.iterationType()).toBe(
      libopencor.SolverCvode.IterationType.NEWTON,
    );
    expect(solver.linearSolver()).toBe(
      libopencor.SolverCvode.LinearSolver.DENSE,
    );
    expect(solver.preconditioner()).toBe(
      libopencor.SolverCvode.Preconditioner.BANDED,
    );
    expect(solver.upperHalfBandwidth()).toBe(0);
    expect(solver.lowerHalfBandwidth()).toBe(0);
    expect(solver.relativeTolerance()).toBe(1.0e-7);
    expect(solver.absoluteTolerance()).toBe(1.0e-7);
    expect(solver.interpolateSolution()).toBe(true);

    solver.setMaximumStep(1.23);
    solver.setMaximumNumberOfSteps(123);
    solver.setIntegrationMethod(
      libopencor.SolverCvode.IntegrationMethod.ADAMS_MOULTON,
    );
    solver.setIterationType(libopencor.SolverCvode.IterationType.FUNCTIONAL);
    solver.setLinearSolver(libopencor.SolverCvode.LinearSolver.GMRES);
    solver.setPreconditioner(libopencor.SolverCvode.Preconditioner.NO);
    solver.setUpperHalfBandwidth(3);
    solver.setLowerHalfBandwidth(5);
    solver.setRelativeTolerance(1.23e-5);
    solver.setAbsoluteTolerance(3.45e-7);
    solver.setInterpolateSolution(false);

    expect(solver.maximumStep()).toBe(1.23);
    expect(solver.maximumNumberOfSteps()).toBe(123);
    expect(solver.integrationMethod()).toBe(
      libopencor.SolverCvode.IntegrationMethod.ADAMS_MOULTON,
    );
    expect(solver.iterationType()).toBe(
      libopencor.SolverCvode.IterationType.FUNCTIONAL,
    );
    expect(solver.linearSolver()).toBe(
      libopencor.SolverCvode.LinearSolver.GMRES,
    );
    expect(solver.preconditioner()).toBe(
      libopencor.SolverCvode.Preconditioner.NO,
    );
    expect(solver.upperHalfBandwidth()).toBe(3);
    expect(solver.lowerHalfBandwidth()).toBe(5);
    expect(solver.relativeTolerance()).toBe(1.23e-5);
    expect(solver.absoluteTolerance()).toBe(3.45e-7);
    expect(solver.interpolateSolution()).toBe(false);

    solver.delete();
  });

  test("Forward Euler solver", () => {
    const solver = new libopencor.SolverForwardEuler();

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000030");
    expect(solver.name()).toBe("Forward Euler");

    expect(solver.step()).toBe(1.0);

    solver.setStep(0.123);

    expect(solver.step()).toBe(0.123);

    solver.delete();
  });

  test("Fourth-order Runge-Kutta solver", () => {
    const solver = new libopencor.SolverFourthOrderRungeKutta();

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000032");
    expect(solver.name()).toBe("Fourth-order Runge-Kutta");

    expect(solver.step()).toBe(1.0);

    solver.setStep(0.123);

    expect(solver.step()).toBe(0.123);

    solver.delete();
  });

  test("Heun solver", () => {
    const solver = new libopencor.SolverHeun();

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000301");
    expect(solver.name()).toBe("Heun");

    expect(solver.step()).toBe(1.0);

    solver.setStep(0.123);

    expect(solver.step()).toBe(0.123);

    solver.delete();
  });

  test("KINSOL solver", () => {
    const solver = new libopencor.SolverKinsol();

    expect(solver.type().value).toBe(libopencor.Solver.Type.NLA.value);
    expect(solver.id()).toBe("KISAO:0000282");
    expect(solver.name()).toBe("KINSOL");

    expect(solver.maximumNumberOfIterations()).toBe(200);
    expect(solver.linearSolver()).toBe(
      libopencor.SolverKinsol.LinearSolver.DENSE,
    );
    expect(solver.upperHalfBandwidth()).toBe(0);
    expect(solver.lowerHalfBandwidth()).toBe(0);

    solver.setMaximumNumberOfIterations(123);
    solver.setLinearSolver(libopencor.SolverKinsol.LinearSolver.GMRES);
    solver.setUpperHalfBandwidth(3);
    solver.setLowerHalfBandwidth(5);

    expect(solver.maximumNumberOfIterations()).toBe(123);
    expect(solver.linearSolver()).toBe(
      libopencor.SolverKinsol.LinearSolver.GMRES,
    );
    expect(solver.upperHalfBandwidth()).toBe(3);
    expect(solver.lowerHalfBandwidth()).toBe(5);

    solver.delete();
  });

  test("Second-order Runge-Kutta solver", () => {
    const solver = new libopencor.SolverSecondOrderRungeKutta();

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.id()).toBe("KISAO:0000381");
    expect(solver.name()).toBe("Second-order Runge-Kutta");

    expect(solver.step()).toBe(1.0);

    solver.setStep(0.123);

    expect(solver.step()).toBe(0.123);

    solver.delete();
  });
});
