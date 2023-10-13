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
import { expectHiddenProperties } from "./utils.js";

export const libopencor = await libOpenCOR();

const noProperties = new libopencor.StringStringMap();
const noHiddenProperties = [];

export function checkCvodeSolver(solverInfo) {
  // Properties.

  expect(solverInfo.type().value).toBe(libopencor.Solver.Type.ODE.value);
  expect(solverInfo.id()).toBe("KISAO:0000019");
  expect(solverInfo.name()).toBe("CVODE");

  const solverInfoProperties = solverInfo.properties();

  expect(solverInfoProperties.size()).toBe(11);

  let property = solverInfoProperties.get(0);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000467");
  expect(property.name()).toBe("Maximum step");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(true);

  let listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(1);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGt0.value,
  );
  expect(property.id()).toBe("KISAO:0000415");
  expect(property.name()).toBe("Maximum number of steps");
  expect(property.defaultValue()).toBe("500");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(2);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000475");
  expect(property.name()).toBe("Integration method");
  expect(property.defaultValue()).toBe("BDF");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("Adams-Moulton");
  expect(listValues.get(1)).toBe("BDF");

  property = solverInfoProperties.get(3);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000476");
  expect(property.name()).toBe("Iteration type");
  expect(property.defaultValue()).toBe("Newton");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("Functional");
  expect(listValues.get(1)).toBe("Newton");

  property = solverInfoProperties.get(4);

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

  property = solverInfoProperties.get(5);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000478");
  expect(property.name()).toBe("Preconditioner");
  expect(property.defaultValue()).toBe("Banded");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("None");
  expect(listValues.get(1)).toBe("Banded");

  property = solverInfoProperties.get(6);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000479");
  expect(property.name()).toBe("Upper half-bandwidth");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(7);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000480");
  expect(property.name()).toBe("Lower half-bandwidth");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(8);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000209");
  expect(property.name()).toBe("Relative tolerance");
  expect(property.defaultValue()).toBe("1e-07");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(9);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000211");
  expect(property.name()).toBe("Absolute tolerance");
  expect(property.defaultValue()).toBe("1e-07");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(10);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.Boolean.value,
  );
  expect(property.id()).toBe("KISAO:0000481");
  expect(property.name()).toBe("Interpolate solution");
  expect(property.defaultValue()).toBe("True");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  // Hidden properties.

  const hiddenPropertiesForNewton = noHiddenProperties;
  const hiddenPropertiesForNewtonDense = [
    "KISAO:0000478",
    "KISAO:0000479",
    "KISAO:0000480",
  ];
  const hiddenPropertiesForNewtonBanded = ["KISAO:0000478"];
  const hiddenPropertiesForNewtonGmres = noHiddenProperties;
  const hiddenPropertiesForNewtonGmresNone = ["KISAO:0000479", "KISAO:0000480"];
  const hiddenPropertiesForNewtonGmresBanded = noHiddenProperties;
  const hiddenPropertiesForNewtonBicgstab = noHiddenProperties;
  const hiddenPropertiesForNewtonBicgstabNone =
    hiddenPropertiesForNewtonGmresNone;
  const hiddenPropertiesForNewtonBicgstabBanded = noHiddenProperties;
  const hiddenPropertiesForNewtonTfqmr = noHiddenProperties;
  const hiddenPropertiesForNewtonTfqmrNone = hiddenPropertiesForNewtonGmresNone;
  const hiddenPropertiesForNewtonTfqmrBanded = noHiddenProperties;
  const hiddenPropertiesForNewtonDiagonal = hiddenPropertiesForNewtonDense;
  const hiddenPropertiesForFunctional = [
    "KISAO:0000477",
    "KISAO:0000478",
    "KISAO:0000479",
    "KISAO:0000480",
  ];

  expectHiddenProperties(
    solverInfo.hiddenProperties(noProperties),
    noHiddenProperties,
  );

  let properties = new libopencor.StringStringMap();

  properties.set("Iteration type", "Newton");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewton,
  );

  properties.set("KISAO:0000477", "Dense");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonDense,
  );

  properties.set("Linear solver", "Banded");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonDense,
  );

  properties.set("KISAO:0000477", "Banded");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBanded,
  );

  properties.set("KISAO:0000477", "Diagonal");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonDiagonal,
  );

  properties.set("KISAO:0000477", "GMRES");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonGmres,
  );

  properties.set("KISAO:0000477", "BiCGStab");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBicgstab,
  );

  properties.set("KISAO:0000477", "TFQMR");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonTfqmr,
  );

  properties.set("KISAO:0000477", "GMRES");
  properties.set("Preconditioner", "None");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonGmresNone,
  );

  properties.set("KISAO:0000477", "BiCGStab");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBicgstabNone,
  );

  properties.set("KISAO:0000477", "TFQMR");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonTfqmrNone,
  );

  properties.set("KISAO:0000477", "GMRES");
  properties.set("KISAO:0000478", "Banded");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonGmresBanded,
  );

  properties.set("KISAO:0000477", "BiCGStab");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBicgstabBanded,
  );

  properties.set("KISAO:0000477", "TFQMR");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonTfqmrBanded,
  );

  properties.set("KISAO:0000476", "Functional");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForFunctional,
  );

  properties.set("Iteration type", "Newton");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForFunctional,
  );
}

export function checkForwardEulerSolver(solverInfo) {
  // Properties.

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

  // Hidden properties.

  expectHiddenProperties(
    solverInfo.hiddenProperties(noProperties),
    noHiddenProperties,
  );
}

export function checkFourthOrderRungeKuttaSolver(solverInfo) {
  // Properties.

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

  // Hidden properties.

  expectHiddenProperties(
    solverInfo.hiddenProperties(noProperties),
    noHiddenProperties,
  );
}

export function checkHeunSolver(solverInfo) {
  // Properties.

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

  // Hidden properties.

  expectHiddenProperties(
    solverInfo.hiddenProperties(noProperties),
    noHiddenProperties,
  );
}

export function checkKinsolSolver(solverInfo) {
  // Properties.

  expect(solverInfo.type().value).toBe(libopencor.Solver.Type.NLA.value);
  expect(solverInfo.id()).toBe("KISAO:0000282");
  expect(solverInfo.name()).toBe("KINSOL");

  const solverInfoProperties = solverInfo.properties();

  expect(solverInfoProperties.size()).toBe(11);

  let property = solverInfoProperties.get(0);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000467");
  expect(property.name()).toBe("Maximum step");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(true);

  let listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(1);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGt0.value,
  );
  expect(property.id()).toBe("KISAO:0000415");
  expect(property.name()).toBe("Maximum number of steps");
  expect(property.defaultValue()).toBe("500");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(2);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000475");
  expect(property.name()).toBe("Integration method");
  expect(property.defaultValue()).toBe("BDF");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("Adams-Moulton");
  expect(listValues.get(1)).toBe("BDF");

  property = solverInfoProperties.get(3);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000476");
  expect(property.name()).toBe("Iteration type");
  expect(property.defaultValue()).toBe("Newton");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("Functional");
  expect(listValues.get(1)).toBe("Newton");

  property = solverInfoProperties.get(4);

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

  property = solverInfoProperties.get(5);

  expect(property.type().value).toBe(libopencor.SolverProperty.Type.List.value);
  expect(property.id()).toBe("KISAO:0000478");
  expect(property.name()).toBe("Preconditioner");
  expect(property.defaultValue()).toBe("Banded");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(2);
  expect(listValues.get(0)).toBe("None");
  expect(listValues.get(1)).toBe("Banded");

  property = solverInfoProperties.get(6);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000479");
  expect(property.name()).toBe("Upper half-bandwidth");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(7);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.IntegerGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000480");
  expect(property.name()).toBe("Lower half-bandwidth");
  expect(property.defaultValue()).toBe("0");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(8);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000209");
  expect(property.name()).toBe("Relative tolerance");
  expect(property.defaultValue()).toBe("1e-07");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(9);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.DoubleGe0.value,
  );
  expect(property.id()).toBe("KISAO:0000211");
  expect(property.name()).toBe("Absolute tolerance");
  expect(property.defaultValue()).toBe("1e-07");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  property = solverInfoProperties.get(10);

  expect(property.type().value).toBe(
    libopencor.SolverProperty.Type.Boolean.value,
  );
  expect(property.id()).toBe("KISAO:0000481");
  expect(property.name()).toBe("Interpolate solution");
  expect(property.defaultValue()).toBe("True");
  expect(property.hasVoiUnit()).toBe(false);

  listValues = property.listValues();

  expect(listValues.size()).toBe(0);

  // Hidden properties.

  const hiddenPropertiesForNewton = noHiddenProperties;
  const hiddenPropertiesForNewtonDense = [
    "KISAO:0000478",
    "KISAO:0000479",
    "KISAO:0000480",
  ];
  const hiddenPropertiesForNewtonBanded = ["KISAO:0000478"];
  const hiddenPropertiesForNewtonGmres = noHiddenProperties;
  const hiddenPropertiesForNewtonGmresNone = ["KISAO:0000479", "KISAO:0000480"];
  const hiddenPropertiesForNewtonGmresBanded = noHiddenProperties;
  const hiddenPropertiesForNewtonBicgstab = noHiddenProperties;
  const hiddenPropertiesForNewtonBicgstabNone =
    hiddenPropertiesForNewtonGmresNone;
  const hiddenPropertiesForNewtonBicgstabBanded = noHiddenProperties;
  const hiddenPropertiesForNewtonTfqmr = noHiddenProperties;
  const hiddenPropertiesForNewtonTfqmrNone = hiddenPropertiesForNewtonGmresNone;
  const hiddenPropertiesForNewtonTfqmrBanded = noHiddenProperties;
  const hiddenPropertiesForNewtonDiagonal = hiddenPropertiesForNewtonDense;
  const hiddenPropertiesForFunctional = [
    "KISAO:0000477",
    "KISAO:0000478",
    "KISAO:0000479",
    "KISAO:0000480",
  ];

  expectHiddenProperties(
    solverInfo.hiddenProperties(noProperties),
    noHiddenProperties,
  );

  let properties = new libopencor.StringStringMap();

  properties.set("Iteration type", "Newton");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewton,
  );

  properties.set("KISAO:0000477", "Dense");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonDense,
  );

  properties.set("Linear solver", "Banded");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonDense,
  );

  properties.set("KISAO:0000477", "Banded");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBanded,
  );

  properties.set("KISAO:0000477", "Diagonal");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonDiagonal,
  );

  properties.set("KISAO:0000477", "GMRES");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonGmres,
  );

  properties.set("KISAO:0000477", "BiCGStab");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBicgstab,
  );

  properties.set("KISAO:0000477", "TFQMR");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonTfqmr,
  );

  properties.set("KISAO:0000477", "GMRES");
  properties.set("Preconditioner", "None");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonGmresNone,
  );

  properties.set("KISAO:0000477", "BiCGStab");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBicgstabNone,
  );

  properties.set("KISAO:0000477", "TFQMR");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonTfqmrNone,
  );

  properties.set("KISAO:0000477", "GMRES");
  properties.set("KISAO:0000478", "Banded");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonGmresBanded,
  );

  properties.set("KISAO:0000477", "BiCGStab");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonBicgstabBanded,
  );

  properties.set("KISAO:0000477", "TFQMR");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForNewtonTfqmrBanded,
  );

  properties.set("KISAO:0000476", "Functional");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForFunctional,
  );

  properties.set("Iteration type", "Newton");

  expectHiddenProperties(
    solverInfo.hiddenProperties(properties),
    hiddenPropertiesForFunctional,
  );
}

export function checkSecondOrderRungeKuttaSolver(solverInfo) {
  // Properties.

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

  // Hidden properties.

  expectHiddenProperties(
    solverInfo.hiddenProperties(noProperties),
    noHiddenProperties,
  );
}
