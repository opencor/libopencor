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

export function checkCvodeSolver(solverInfo) {
  expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
  expect(solverInfo.id()).toBe("KISAO:0000019");
  expect(solverInfo.name()).toBe("CVODE");

  const properties = solverInfo.properties();

  expect(properties.size()).toBe(11);

  let property = properties.get(0);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000467");
  expect(property.name()).toBe("Maximum step");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(true);

  let listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = properties.get(1);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGt0.value,
  );
  expect(property.id()).toBe("KISAO:0000415");
  expect(property.name()).toBe("Maximum number of steps");
  expect(property.defaultValue()).toBe("500");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = properties.get(2);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000475");
  expect(property.name()).toBe("Integration method");
  expect(property.defaultValue()).toBe("BDF");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("Adams-Moulton");
  expect(listValues.get(1)).toBe("BDF");

  property = properties.get(3);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000476");
  expect(property.name()).toBe("Iteration type");
  expect(property.defaultValue()).toBe("Newton");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("Functional");
  expect(listValues.get(1)).toBe("Newton");

  property = properties.get(4);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000477");
  expect(property.name()).toBe("Linear solver");
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

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000478");
  expect(property.name()).toBe("Preconditioner");
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
  expect(property.id()).toBe("KISAO:0000479");
  expect(property.name()).toBe("Upper half-bandwidth");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = properties.get(7);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000480");
  expect(property.name()).toBe("Lower half-bandwidth");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = properties.get(8);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000209");
  expect(property.name()).toBe("Relative tolerance");
  expect(property.defaultValue()).toBe("1e-07");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = properties.get(9);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000211");
  expect(property.name()).toBe("Absolute tolerance");
  expect(property.defaultValue()).toBe("1e-07");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = properties.get(10);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.Boolean.value,
  );
  expect(property.id()).toBe("KISAO:0000481");
  expect(property.name()).toBe("Interpolate solution");
  expect(property.defaultValue()).toBe("True");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);
}

export function checkForwardEulerSolver(solverInfo) {
  expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
  expect(solverInfo.id()).toBe("KISAO:0000030");
  expect(solverInfo.name()).toBe("Forward Euler");

  const properties = solverInfo.properties();

  expect(properties.size()).toBe(1);

  const property = properties.get(0);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGt0.value,
  );
  expect(property.id()).toBe("KISAO:0000483");
  expect(property.name()).toBe("Step");
  expect(property.defaultValue()).toBe("1");
  expect(property.hasVoiUnit()).toBe(true);

  const listValues = property.listValues();

  expect(listValues.size()).toBe(0);
}

export function checkFourthOrderRungeKuttaSolver(solverInfo) {
  expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
  expect(solverInfo.id()).toBe("KISAO:0000032");
  expect(solverInfo.name()).toBe("Fourth-order Runge-Kutta");

  const properties = solverInfo.properties();

  expect(properties.size()).toBe(1);

  const property = properties.get(0);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGt0.value,
  );
  expect(property.id()).toBe("KISAO:0000483");
  expect(property.name()).toBe("Step");
  expect(property.defaultValue()).toBe("1");
  expect(property.hasVoiUnit()).toBe(true);

  const listValues = property.listValues();

  expect(listValues.size()).toBe(0);
}

export function checkHeunSolver(solverInfo) {
  expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
  expect(solverInfo.id()).toBe("KISAO:0000301");
  expect(solverInfo.name()).toBe("Heun");

  const properties = solverInfo.properties();

  expect(properties.size()).toBe(1);

  const property = properties.get(0);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGt0.value,
  );
  expect(property.id()).toBe("KISAO:0000483");
  expect(property.name()).toBe("Step");
  expect(property.defaultValue()).toBe("1");
  expect(property.hasVoiUnit()).toBe(true);

  const listValues = property.listValues();

  expect(listValues.size()).toBe(0);
}

export function checkSecondOrderRungeKuttaSolver(solverInfo) {
  expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
  expect(solverInfo.id()).toBe("KISAO:0000381");
  expect(solverInfo.name()).toBe("Second-order Runge-Kutta");

  const properties = solverInfo.properties();

  expect(properties.size()).toBe(1);

  const property = properties.get(0);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGt0.value,
  );
  expect(property.id()).toBe("KISAO:0000483");
  expect(property.name()).toBe("Step");
  expect(property.defaultValue()).toBe("1");
  expect(property.hasVoiUnit()).toBe(true);

  const listValues = property.listValues();

  expect(listValues.size()).toBe(0);
}
