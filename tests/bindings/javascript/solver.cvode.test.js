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
import * as odeModel from "./ode.model.js";
import * as utils from "./utils.js";
import { expectIssues } from "./utils.js";

const loc = await libOpenCOR();

describe("Solver CVODE tests", () => {
  let solverOdeContentsPtr;

  beforeAll(() => {
    solverOdeContentsPtr = utils.allocateMemory(loc, utils.SOLVER_ODE_CONTENTS);
  });

  afterAll(() => {
    utils.freeMemory(loc, solverOdeContentsPtr);
  });

  test("Maximum step value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.maximumStep = -1.234;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The maximum step cannot be equal to -1.234. It must be greater or equal to 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Maximum number of steps value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.maximumNumberOfSteps = 0;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The maximum number of steps cannot be equal to 0. It must be greater than 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and upper half-bandwidth value with number too small", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.upperHalfBandwidth = -1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and upper half-bandwidth value with number too big", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.upperHalfBandwidth = 4;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and lower half-bandwidth value with number too small", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.lowerHalfBandwidth = -1;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Banded linear solver and lower half-bandwidth value with number too big", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;
    solver.lowerHalfBandwidth = 4;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Relative tolerance value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.relativeTolerance = -1.234;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Absolute tolerance value with invalid number", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.absoluteTolerance = -1.234;

    const instance = document.instantiate();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.ERROR,
        "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("Solve", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);

    odeModel.run(
      document,
      [
        -63.886106129036406, 0.13500772470692476, 0.9843337155912821,
        0.7409722760053465,
      ],
      [7, 7, 7, 7],
      [
        49.72583819278412, -0.1281928932150167, -0.05090363134799315,
        0.09864930257002996,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.98193183871092, -823.5170686925285, 789.7796037360945,
        3.969889220790335, 0.11498728095120034, 0.002869649611111302,
        0.9673466865046488, 0.5413340463230938, 0.056246139672740336,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve without interpolate solution", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.interpolateSolution = false;

    odeModel.run(
      document,
      [
        -63.88639507923284, 0.13500831264924482, 0.9843340818657464,
        0.7409715439128075,
      ],
      [7, 7, 7, 7],
      [
        49.725708804678554, -0.12819363057965827, -0.05090307887503688,
        0.09865054326613137,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.982018523769852, -823.5169497839769, 789.7794600749353,
        3.9699163221479288, 0.11498543509949267, 0.002869608152119851,
        0.967347599201465, 0.5413368773015703, 0.05624593651894338,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with Adams-Moulton integration method", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.integrationMethod = loc.SolverCvode.IntegrationMethod.ADAMS_MOULTON;

    odeModel.run(
      document,
      [
        -63.886661458181024, 0.13500897393822361, 0.9843343344667064,
        0.7409710787418513,
      ],
      [7, 7, 7, 7],
      [
        49.72681387239589, -0.1281944327747464, -0.05090278303879249,
        0.09865164422001499,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.982098437454306, -823.517772548299, 789.7798205819753,
        3.969941306524383, 0.11498373346212507, 0.0028695699322143133,
        0.9673484405814371, 0.5413394871402794, 0.056245749235087565,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with functional iteration type", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.iterationType = loc.SolverCvode.IterationType.FUNCTIONAL;

    odeModel.run(
      document,
      [
        -63.886064143241185, 0.13500747020177464, 0.9843339160096614,
        0.7409722657222922,
      ],
      [7, 7, 7, 7],
      [
        49.725745713352005, -0.12819264218579152, -0.05090477732386291,
        0.09864917810281565,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981919242972355, -823.5165673477538, 789.7792460634204,
        3.9698852828412163, 0.11498754916442601, 0.0028696556353436722,
        0.9673465538835438, 0.5413336349689842, 0.056246169191984365,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with banded linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BANDED;

    odeModel.run(
      document,
      [
        -54.95769663027577, 0.11471551896322969, 0.9713647444625766,
        0.7569678854308425,
      ],
      [7, 7, 7, 7],
      [
        47.19661058955666, -0.10208043809229013, -0.09297814175229392,
        0.06297616024756474,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -13.303408989082731, -791.4305773063138, 757.5438469048962,
        3.1534355451295104, 0.1888294585042939, 0.004484425575674626,
        0.923844725011358, 0.45464907267569826, 0.06288719270767433,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with diagonal linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.DIAGONAL;

    odeModel.run(
      document,
      [
        -63.886372487723776, 0.13500839339062765, 0.9843340211966347,
        0.7409717426931475,
      ],
      [7, 7, 7, 7],
      [
        49.72597399381498, -0.12819098846660215, -0.05090306558144843,
        0.09865036279205125,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.982011746317133, -823.5175883208121, 789.7801354388632,
        3.9699142032327877, 0.11498557941594431, 0.00286961139356061,
        0.9673475278433488, 0.5413366559620783, 0.056245952402452934,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with GMRES linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.GMRES;

    odeModel.run(
      document,
      [
        9.51846792741973, 0.3673655685671932, 0.016120441606803777,
        0.4911445527956562,
      ],
      [7, 7, 7, 7],
      [
        -0.8640672736573222, 0.06435448572298622, 0.001717734101917398,
        -0.052716115419100616,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, 6.039440378225918, -5.198280618095664, 0.022995574731048997,
        0.11295819992340264, 6.787604865006558, 0.11266498024016741,
        0.018856763702842786, 0.03230635782351839, 0.1407935434361332,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with Bi-CGstab linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BICGSTAB;

    odeModel.run(
      document,
      [
        9.51840556729375, 0.3673701685232119, 0.01612056593462721,
        0.4911409429315981,
      ],
      [7, 7, 7, 7],
      [
        -0.8640794299937041, 0.0643535850994105, 0.0017178663652530986,
        -0.052715357939910126,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, 6.039421670188124, -5.19825841970766, 0.02299638321880334,
        0.11295872331711411, 6.7875813497198205, 0.11266462895059702,
        0.01885687907681896, 0.032306489415568115, 0.14079343368738714,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with TFQMR linear solver", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.TFQMR;

    odeModel.run(
      document,
      [
        9.518096198981056, 0.36739580534962885, 0.016121188857719522,
        0.4911219101036065,
      ],
      [7, 7, 7, 7],
      [
        -0.8641444918518275, 0.06434891303317661, 0.0017178076038649821,
        -0.05271146850152765,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, 6.039328859694316, -5.198100630380928, 0.02300059700386618,
        0.11296131990363434, 6.787464691689585, 0.11266288622076774,
        0.01885745145684819, 0.03230714224978534, 0.14079288922560218,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with GMRES linear solver and no preconditioner", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.GMRES;
    solver.preconditioner = loc.SolverCvode.Preconditioner.NO;

    odeModel.run(
      document,
      [
        -63.88634823717041, 0.13500834736004785, 0.9843336773958121,
        0.7409716717404242,
      ],
      [7, 7, 7, 7],
      [
        49.725850793094246, -0.12819367339268917, -0.05090392373588483,
        0.09865049266027853,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.982004471151122, -823.5170097263399, 789.7794133274365,
        3.969911928711907, 0.11498573433071144, 0.0028696148730459324,
        0.9673474512447651, 0.5413364183681688, 0.056245969452398896,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with Bi-CGstab linear solver and no preconditioner", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.BICGSTAB;
    solver.preconditioner = loc.SolverCvode.Preconditioner.NO;

    odeModel.run(
      document,
      [
        -63.886089564154645, 0.13500769053195763, 0.9843334956356238,
        0.7409722063620179,
      ],
      [7, 7, 7, 7],
      [
        49.72555254907632, -0.12819286849323652, -0.05090358167883548,
        0.09864924502317379,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981926869246394, -823.5165793237518, 789.7791303226917,
        3.9698876671299828, 0.11498738677073309, 0.0028696519878826116,
        0.9673466341809926, 0.5413338840293397, 0.05624615131912471,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });

  test("Solve with TFQMR linear solver and no preconditioner", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(solverOdeContentsPtr, utils.SOLVER_ODE_CONTENTS.length);

    const document = new loc.SedDocument(file);
    const simulation = document.simulations.get(0);
    const solver = simulation.odeSolver;

    solver.linearSolver = loc.SolverCvode.LinearSolver.TFQMR;
    solver.preconditioner = loc.SolverCvode.Preconditioner.NO;

    odeModel.run(
      document,
      [
        -63.88604118093512, 0.13500758313849645, 0.9843333599191683,
        0.7409722984967344,
      ],
      [7, 7, 7, 7],
      [
        49.72566119351199, -0.12819273665866548, -0.05090317673764379,
        0.09864904013329096,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981912354280537, -823.5164638625301, 789.7789229932755,
        3.969883129151409, 0.11498769585225826, 0.0028696589300411125,
        0.967346481351974, 0.5413334099967426, 0.056246185336258574,
      ],
      [7, 7, 7, 7, 7, 7, 7, 7, 7, 7],
    );

    document.delete();
    file.delete();
  });
});
