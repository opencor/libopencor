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

import test from 'node:test'

import libOpenCOR from './libopencor.js'
import * as odeModel from './ode.model.js'
import * as utils from './utils.js'
import { assertIssues } from './utils.js'

const loc = await libOpenCOR()

test.describe('Solver Second-Order Runge-Kutta tests', () => {
  let solverOdeContentsPtr

  test.before(() => {
    solverOdeContentsPtr = utils.allocateMemory(loc, utils.SOLVER_ODE_CONTENTS)
  })

  test.beforeEach(() => {
    loc.FileManager.instance().reset()
  })

  test.after(() => {
    utils.freeMemory(loc, solverOdeContentsPtr)
  })

  test('Step value with invalid number', () => {
    const file = new loc.File(utils.CELLML_FILE)

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = new loc.SolverSecondOrderRungeKutta()

    solver.step = 0.0

    simulation.odeSolver = solver

    const instance = document.instantiate()

    assertIssues(loc, instance, [[loc.Issue.Type.ERROR, 'The step cannot be equal to 0. It must be greater than 0.']])
  })

  test('Solve', () => {
    const file = new loc.File(utils.CELLML_FILE)

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length)

    const document = new loc.SedDocument(file)
    const simulation = document.simulations.get(0)
    const solver = new loc.SolverSecondOrderRungeKutta()

    solver.step = 0.0123

    simulation.odeSolver = solver

    odeModel.run(
      document,
      [-63.88652516147998, 0.13500864016199698, 0.9843341849003562, 0.7409712245332702],
      [7, 7, 7, 7],
      [49.725721814665114, -0.12819399753080235, -0.0509025737713709, 0.09865110169688351],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.982057548443994, -823.5169415955212, 789.7796140225834, 3.969928522897354, 0.11498460412672941,
        0.0028695894879267065, 0.9673480100796346, 0.5413381517778336, 0.056245845061545056
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7]
    )
  })
})
