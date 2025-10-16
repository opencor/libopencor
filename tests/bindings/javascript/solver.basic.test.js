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

import assert from "node:assert";
import test from "node:test";

import libOpenCOR from "./libopencor.js";

const loc = await libOpenCOR();

test.describe("Solver basic tests", () => {
  test("CVODE solver", () => {
    const solver = new loc.SolverCvode();

    assert.strictEqual(solver.type.value, loc.Solver.Type.ODE.value);
    assert.strictEqual(solver.id, "KISAO:0000019");
    assert.strictEqual(solver.name, "CVODE");

    assert.strictEqual(solver.maximumStep, 0.0);
    assert.strictEqual(solver.maximumNumberOfSteps, 500);
    assert.strictEqual(solver.integrationMethod, loc.SolverCvode.IntegrationMethod.BDF);
    assert.strictEqual(solver.iterationType, loc.SolverCvode.IterationType.NEWTON);
    assert.strictEqual(solver.linearSolver, loc.SolverCvode.LinearSolver.DENSE);
    assert.strictEqual(solver.preconditioner, loc.SolverCvode.Preconditioner.BANDED);
    assert.strictEqual(solver.upperHalfBandwidth, 0);
    assert.strictEqual(solver.lowerHalfBandwidth, 0);
    assert.strictEqual(solver.relativeTolerance, 1e-7);
    assert.strictEqual(solver.absoluteTolerance, 1e-7);
    assert.strictEqual(solver.interpolateSolution, true);

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

    assert.strictEqual(solver.maximumStep, 1.23);
    assert.strictEqual(solver.maximumNumberOfSteps, 123);
    assert.strictEqual(solver.integrationMethod, loc.SolverCvode.IntegrationMethod.ADAMS_MOULTON);
    assert.strictEqual(solver.iterationType, loc.SolverCvode.IterationType.FUNCTIONAL);
    assert.strictEqual(solver.linearSolver, loc.SolverCvode.LinearSolver.GMRES);
    assert.strictEqual(solver.preconditioner, loc.SolverCvode.Preconditioner.NO);
    assert.strictEqual(solver.upperHalfBandwidth, 3);
    assert.strictEqual(solver.lowerHalfBandwidth, 5);
    assert.strictEqual(solver.relativeTolerance, 1.23e-5);
    assert.strictEqual(solver.absoluteTolerance, 3.45e-7);
    assert.strictEqual(solver.interpolateSolution, false);
  });

  test("Forward Euler solver", () => {
    const solver = new loc.SolverForwardEuler();

    assert.strictEqual(solver.type.value, loc.Solver.Type.ODE.value);
    assert.strictEqual(solver.id, "KISAO:0000030");
    assert.strictEqual(solver.name, "Forward Euler");

    assert.strictEqual(solver.step, 1.0);

    solver.step = 0.123;

    assert.strictEqual(solver.step, 0.123);
  });

  test("Fourth-order Runge-Kutta solver", () => {
    const solver = new loc.SolverFourthOrderRungeKutta();

    assert.strictEqual(solver.type.value, loc.Solver.Type.ODE.value);
    assert.strictEqual(solver.id, "KISAO:0000032");
    assert.strictEqual(solver.name, "Fourth-order Runge-Kutta");

    assert.strictEqual(solver.step, 1.0);

    solver.step = 0.123;

    assert.strictEqual(solver.step, 0.123);
  });

  test("Heun solver", () => {
    const solver = new loc.SolverHeun();

    assert.strictEqual(solver.type.value, loc.Solver.Type.ODE.value);
    assert.strictEqual(solver.id, "KISAO:0000301");
    assert.strictEqual(solver.name, "Heun");

    assert.strictEqual(solver.step, 1.0);

    solver.step = 0.123;

    assert.strictEqual(solver.step, 0.123);
  });

  test("KINSOL solver", () => {
    const solver = new loc.SolverKinsol();

    assert.strictEqual(solver.type.value, loc.Solver.Type.NLA.value);
    assert.strictEqual(solver.id, "KISAO:0000282");
    assert.strictEqual(solver.name, "KINSOL");

    assert.strictEqual(solver.maximumNumberOfIterations, 200);
    assert.strictEqual(solver.linearSolver, loc.SolverKinsol.LinearSolver.DENSE);
    assert.strictEqual(solver.upperHalfBandwidth, 0);
    assert.strictEqual(solver.lowerHalfBandwidth, 0);

    solver.maximumNumberOfIterations = 123;
    solver.linearSolver = loc.SolverKinsol.LinearSolver.GMRES;
    solver.upperHalfBandwidth = 3;
    solver.lowerHalfBandwidth = 5;

    assert.strictEqual(solver.maximumNumberOfIterations, 123);
    assert.strictEqual(solver.linearSolver, loc.SolverKinsol.LinearSolver.GMRES);
    assert.strictEqual(solver.upperHalfBandwidth, 3);
    assert.strictEqual(solver.lowerHalfBandwidth, 5);
  });

  test("Second-order Runge-Kutta solver", () => {
    const solver = new loc.SolverSecondOrderRungeKutta();

    assert.strictEqual(solver.type.value, loc.Solver.Type.ODE.value);
    assert.strictEqual(solver.id, "KISAO:0000381");
    assert.strictEqual(solver.name, "Second-order Runge-Kutta");

    assert.strictEqual(solver.step, 1.0);

    solver.step = 0.123;

    assert.strictEqual(solver.step, 0.123);
  });
});
