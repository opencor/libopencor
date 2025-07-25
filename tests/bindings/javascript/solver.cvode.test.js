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
        -63.886159985315, 0.13500778169453354, 0.9843339334765916,
        0.7409721973331527,
      ],
      [
        49.7258320571268, -0.1281929914713642, -0.0509042846657264,
        0.09864944517007285,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.9819479955945, -823.5173033961034, 789.7796294137273,
        3.9698942721018833, 0.11498693690799075, 0.0028696418836892607,
        0.9673468566204497, 0.541334573977819, 0.056246101807655983,
      ],
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
        -63.886394790001404, 0.13500831191545926, 0.9843340815693653,
        0.740971544500966,
      ],
      [
        49.72570873084606, -0.12819362970827197, -0.050903079906656115,
        0.0986505420292319,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.98201843700042, -823.5169492599671, 789.7794595380366,
        3.9699162950201745, 0.11498543694712618, 0.0028696081936188957,
        0.9673475982878923, 0.5413368744678356, 0.05624593672229455,
      ],
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
        -63.885147069044045, 0.13500515575967986, 0.984332804274149,
        0.7409741271906708,
      ],
      [
        49.725268194897005, -0.12818990011283393, -0.05090765954377009,
        0.09864507193574261,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.981644120713213, -823.5148905214032, 789.7772006167944,
        3.969799268430244, 0.11499340776446455, 0.0028697872227174265,
        0.96734365697891, 0.5413246499790578, 0.056246813969560716,
      ],
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
        -63.886185091440865, 0.1350077551142661, 0.9843341557301484,
        0.7409720344203582,
      ],
      [
        49.72565458284975, -0.1281929944435145, -0.050904854055039875,
        0.09864974480416816,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.981955527432259, -823.516851602185, 789.7796209726483,
        3.969896626867014, 0.11498677652607421, 0.002869638281412006,
        0.9673469359228645, 0.541334819954094, 0.05624608415613736,
      ],
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
        -54.957826774042424, 0.11471588859484515, 0.9713649644644408,
        0.7569677713299503,
      ],
      [
        47.19680266736277, -0.10208088272886769, -0.09297788112466333,
        0.06297649006484933,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -13.303448032212726, -791.4316384041682, 757.5451605467405,
        3.1534470845376794, 0.18882809323273037, 0.004484396394767784,
        0.9238456406412249, 0.45465032204134687, 0.06288709040305587,
      ],
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
        -63.886267686782496, 0.13500903358822702, 0.9843319263703944,
        0.7409725160144607,
      ],
      [
        49.72788975022893, -0.1281942515600365, -0.050856703121688204,
        0.09864950572274683,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.981980306034748, -823.5198889073449, 789.7804574667246,
        3.9699043736886597, 0.11498624889550198, 0.0028696264304987657,
        0.9673471968144537, 0.5413356291787568, 0.05624602608536063,
      ],
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
        9.518467927248489, 0.36736556858353575, 0.016120441607140608,
        0.49114455278518776,
      ],
      [
        -0.8640672736928493, 0.06435448572009964, 0.001717734102104748,
        -0.05271611541699119,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, 6.039440378174546, -5.198280618011185, 0.022995574733481804,
        0.1129581999248399, 6.787604864941985, 0.11266498023920278,
        0.018856763703159602, 0.03230635782387974, 0.14079354343583184,
      ],
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
        9.518405564444732, 0.36737016878342227, 0.01612056594028319,
        0.49114094275753223,
      ],
      [
        -0.864079430585231, 0.06435358505305869, 0.0017178663671092527,
        -0.05271535790427197,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, 6.03942166933342, -5.198258418306283, 0.022996383258770837,
        0.11295872334102618, 6.787581348645491, 0.11266462893454784,
        0.018856879082090005, 0.032306489421580105, 0.14079343368237313,
      ],
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
        9.51810028209421, 0.3673955287897394, 0.016121174402154714,
        0.491122096081656,
      ],
      [
        -0.8641436531715911, 0.064348984825804, 0.0017177344065279015,
        -0.05271151548616605,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, 6.039330084628262, -5.1980999523652835, 0.023000518571717903,
        0.11296128563294316, 6.787466231355669, 0.11266290922153573,
        0.018857443902335787, 0.032307133633459384, 0.14079289641151857,
      ],
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
        -63.88621828644202, 0.13500801537987772, 0.984333588680053,
        0.7409719543040226,
      ],
      [
        49.72574979057412, -0.12819328089532123, -0.05090508974417475,
        0.09864995636149537,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.981965485932607, -823.516855668815, 789.7792656687827,
        3.969899740308024, 0.11498656447148192, 0.0028696335185336534,
        0.9673470407752258, 0.54133514518082, 0.05624606081753184,
      ],
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
        -63.885833430328, 0.13500702541713172, 0.9843332636114134,
        0.7409727199159764,
      ],
      [
        49.725499450764794, -0.12819208373134217, -0.050904238793864534,
        0.09864814181298392,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.9818500290984, -823.5160828073203, 789.7786385808596,
        3.969863643733743, 0.11498902301345183, 0.00286968873886518,
        0.9673458251182259, 0.5413313745691624, 0.056246331401187635,
      ],
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
        -63.88634204321998, 0.1350083015642865, 0.9843339064650298,
        0.7409717377125247,
      ],
      [
        49.72564395744342, -0.1281935856667793, -0.050903667054764026,
        0.09865026984162611,
      ],
      [1, 0, 0.3, 120, 36],
      [-10.613, -115, 12],
      [
        0, -15.982002612965994, -823.5172357956985, 789.7797925147979,
        3.9699113477656347, 0.1149857738982704, 0.0028696157617586833,
        0.9673474316803248, 0.5413363576831676, 0.0562459738072084,
      ],
    );

    document.delete();
    file.delete();
  });
});
