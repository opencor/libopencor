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
        -63.886250327293745, 0.13500806843489388, 0.9843336119444306,
        0.7409718295171003,
      ],
      [3, 6, 6, 6],
      [
        49.725700307489824, -0.1281933409028202, -0.050902572494876855,
        0.09864999300596061,
      ],
      [3, 6, 5, 5],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981975098188123, -823.5166486227178, 789.7791369560812,
        3.9699027454989695, 0.11498635979013846, 0.002869628921262234,
        0.9673471419816907, 0.5413354590998287, 0.056246038290390175,
      ],
      [7, 4, 2, 3, 4, 5, 7, 6, 5, 6],
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
        -63.88639507849153, 0.1350083126473699, 0.9843340818649865,
        0.7409715439143221,
      ],
      [6, 7, 7, 7],
      [
        49.72570880449381, -0.12819363057743782, -0.050903078877701416,
        0.09865054326291173,
      ],
      [6, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.98201852354746, -823.5169497826655, 789.7794600735924,
        3.9699163220783995, 0.11498543510422823, 0.002869608152226214,
        0.9673475991991235, 0.5413368772943074, 0.05624593651946458,
      ],
      [7, 6, 5, 5, 7, 7, 7, 7, 7, 7],
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
        -63.887813934797826, 0.13501191292213002, 0.9843355542098364,
        0.7409686747551986,
      ],
      [2, 4, 5, 4],
      [
        49.72620260336583, -0.12819788722747028, -0.05089750341316691,
        0.09865693092730915,
      ],
      [2, 4, 4, 4],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.982444180439348, -823.5195918535778, 789.7821411983648,
        3.97004940067058, 0.11497637169423897, 0.0028694045813660754,
        0.9673520805329192, 0.5413507785037981, 0.056244938967039075,
      ],
      [7, 2, 2, 2, 3, 4, 6, 4, 4, 5],
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
        -63.886282061359374, 0.135008093417601, 0.9843339833821376,
        0.7409718122756039,
      ],
      [3, 6, 6, 6],
      [
        49.72538296330156, -0.12819336464938047, -0.050903478581860076,
        0.09865009949522247,
      ],
      [3, 6, 5, 6],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981984618407811, -823.5169163515275, 789.7796868336728,
        3.9699057219161293, 0.11498615706894559, 0.0028696243680162202,
        0.967347242218821, 0.5413357700131285, 0.056246015978951215,
      ],
      [7, 4, 3, 3, 4, 5, 7, 6, 5, 6],
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
        -54.95816860466446, 0.11471650712970222, 0.9713656209644033,
        0.756967274462986,
      ],
      [3, 5, 5, 6],
      [
        47.19692686937322, -0.10208187002903986, -0.0929754636050344,
        0.06297792796897843,
      ],
      [3, 5, 5, 5],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -13.303550581399337, -791.4336008281795, 757.5464682504468,
        3.1534773935628353, 0.1888245073098595, 0.004484319750222318,
        0.9238480455513784, 0.4546536035795939, 0.06288682169446458,
      ],
      [7, 3, 2, 2, 4, 5, 6, 5, 5, 6],
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
        -63.88448960386081, 0.13500371980870626, 0.9843320337747878,
        0.740975648022661,
      ],
      [2, 4, 6, 5],
      [
        49.72470056973946, -0.1281877408620835, -0.05090942244126474,
        0.0986375194818581,
      ],
      [2, 4, 3, 4],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981446881158242, -823.5145165709908, 789.777104114329,
        3.9697376035476495, 0.114997608072391, 0.0028698815635271745,
        0.9673415799887405, 0.5413182085040471, 0.05624727622548335,
      ],
      [7, 2, 1, 2, 3, 4, 6, 4, 4, 5],
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
        9.518467927238342, 0.36736556858518055, 0.01612044160716401,
        0.49114455278457375,
      ],
      [7, 7, 7, 7],
      [
        -0.8640672736946257, 0.06435448571982208, 0.0017177341020318897,
        -0.05271611541688017,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, 6.039440378171502, -5.198280618006445, 0.02299557473368303,
        0.11295819992492505, 6.787604864938159, 0.11266498023914563,
        0.018856763703178375, 0.03230635782390115, 0.14079354343581396,
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
        9.518405563384688, 0.3673701689782409, 0.016120565942342176,
        0.49114094269101,
      ],
      [7, 7, 7, 7],
      [
        -0.8640794309120806, 0.06435358502313818, 0.0017178663683860092,
        -0.05271535789078276,
      ],
      [7, 7, 7, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, 6.039421669015407, -5.198258417710488, 0.02299638327958176,
        0.11295872334992325, 6.7875813482457605, 0.11266462892857637,
        0.018856879084051228, 0.03230648942381701, 0.14079343368050753,
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
        9.5180969791175, 0.36739578686621527, 0.016121150676415584,
        0.4911219162005304,
      ],
      [5, 6, 6, 6],
      [
        -0.8641450065405636, 0.06434891371052368, 0.0017179872368887157,
        -0.05271147016114454,
      ],
      [5, 6, 2, 7],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, 6.03932909373525, -5.198099254585335, 0.02300043256789237,
        0.11296131335573398, 6.787464985864513, 0.11266289061538902,
        0.018857450013451658, 0.03230714060351441, 0.14079289059857297,
      ],
      [7, 5, 5, 6, 7, 5, 7, 7, 7, 7],
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
        -63.88610210255769, 0.13500770971929016, 0.9843329698410943,
        0.7409720924682706,
      ],
      [3, 6, 5, 6],
      [
        49.726333013075525, -0.12819299724421862, -0.05091538950854948,
        0.09865029693084587,
      ],
      [2, 6, 4, 6],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981930630767307, -823.5162090645707, 789.7777832194464,
        3.9698888431371406, 0.1149873066730832, 0.002869650188840514,
        0.967346673786178, 0.5413340068738324, 0.056246142503663964,
      ],
      [7, 4, 2, 2, 4, 5, 7, 6, 5, 6],
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
        -63.88580250259205, 0.13500689029927881, 0.9843332002100482,
        0.740972698029834,
      ],
      [4, 6, 6, 7],
      [
        49.725495533208736, -0.12819193324161038, -0.050904587288098005,
        0.09864807163623063,
      ],
      [5, 6, 6, 6],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981840750777614, -823.515649881427, 789.7781734161576,
        3.969860742950322, 0.1149892205886294, 0.0028696931765173905,
        0.9673457274239244, 0.5413310715560667, 0.05624635314583791,
      ],
      [7, 4, 3, 3, 5, 6, 7, 6, 6, 7],
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
        -63.88590096494604, 0.13500714432443245, 0.9843333109015502,
        0.7409725093281302,
      ],
      [3, 5, 5, 5],
      [
        49.72555013681926, -0.12819226308184883, -0.050905813346241904,
        0.09864850534796382,
      ],
      [3, 5, 5, 5],
      [1, 0, 0.3, 120, 36],
      [7, 7, 7, 7, 7],
      [-10.613, -115, 12],
      [7, 7, 7],
      [
        0, -15.981870289483812, -823.5158795070533, 789.7784045107686,
        3.969869977962206, 0.11498859158427523, 0.0028696790487148967,
        0.967346038445092, 0.5413320362365842, 0.056246283919026335,
      ],
      [7, 3, 2, 2, 4, 5, 6, 5, 5, 6],
    );

    document.delete();
    file.delete();
  });
});
