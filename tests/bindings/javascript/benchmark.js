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

import { performance } from 'node:perf_hooks';
import { resourceUsage } from 'node:process';

import libOpenCOR from './libopencor.js';
import * as utils from './utils.js';

const loc = await libOpenCOR();

const REPETITION_COUNT = 5;

function peakMemoryUsage() {
  // Note: maxRSS is in kilobytes.

  return 1024 * resourceUsage().maxRSS;
}

function registerFile(filePath) {
  const file = new loc.File(utils.resourcePath(filePath));

  file.setContents(utils.fileContents(file.path));

  return file;
}

function runBenchmark(name, resourcePath, odeSolver = null, numberOfSteps = 0) {
  // Create our document, using the given model, and customise its simulation, if needed.
  // Note: our JavaScript bindings rely on a virtual file system, so we need to register both the SED-ML file and the
  //       CellML file it references (which shares the same base name).

  registerFile(resourcePath.replace(/\.sedml$/, '.cellml'));

  const file = registerFile(resourcePath);
  const document = new loc.SedDocument(file);
  const simulation = document.simulations[0];

  if (odeSolver !== null) {
    simulation.odeSolver = odeSolver;
  }

  if (numberOfSteps > 0) {
    simulation.outputEndTime = (simulation.outputEndTime * numberOfSteps) / simulation.numberOfSteps;
    simulation.numberOfSteps = numberOfSteps;
  }

  // Instantiate our document and time how long it takes (this includes JIT compiling the model).

  const instantiateStart = performance.now();
  const instance = document.instantiate();
  const instantiateMs = performance.now() - instantiateStart;

  if (instance.hasIssues) {
    console.error(`ERROR: the ${name} benchmark could not be instantiated:`);

    for (let i = 0; i < instance.issueCount; ++i) {
      console.error(` - ${instance.issue(i).description}`);
    }

    return false;
  }

  // Warm up (i.e. a first, untimed, run) and then time several runs.

  instance.run();

  if (instance.hasIssues) {
    console.error(`ERROR: the ${name} benchmark could not be run.`);

    return false;
  }

  const times = [];

  for (let i = 0; i < REPETITION_COUNT; ++i) {
    const start = performance.now();

    instance.run();

    times.push(performance.now() - start);
  }

  times.sort((a, b) => a - b);

  const mean = times.reduce((sum, time) => sum + time, 0.0) / REPETITION_COUNT;
  const steps = simulation.numberOfSteps;
  const peakMemoryMb = peakMemoryUsage() / (1024.0 * 1024.0);

  console.log(
    `${name.padEnd(16)} | ${String(steps).padStart(7)} | ${instantiateMs.toFixed(3).padStart(9)} | ${times[0].toFixed(3).padStart(8)} | ${times[Math.floor(times.length / 2)].toFixed(3).padStart(8)} | ${mean.toFixed(3).padStart(11)} | ${((mean * 1.0e6) / steps).toFixed(1).padStart(14)} | ${peakMemoryMb.toFixed(1).padStart(9)}`
  );

  return true;
}

console.log('Benchmark        |   Steps | Inst (ms) | Min (ms) | Median (ms) | Mean (ms) | Mean (ns/step) | Peak (MB)');
console.log('-----------------+---------+-----------+----------+-------------+-----------+----------------+----------');

let ok = true;

ok = runBenchmark('cvode-50k', 'cellml_2.sedml') && ok;
ok = runBenchmark('cvode-1m', 'cellml_2.sedml', null, 1000000) && ok;
ok = runBenchmark('euler-1m', 'cellml_2.sedml', new loc.SolverForwardEuler(), 1000000) && ok;
ok = runBenchmark('heun-1m', 'cellml_2.sedml', new loc.SolverHeun(), 1000000) && ok;
ok = runBenchmark('rk2-1m', 'cellml_2.sedml', new loc.SolverSecondOrderRungeKutta(), 1000000) && ok;
ok = runBenchmark('rk4-1m', 'cellml_2.sedml', new loc.SolverFourthOrderRungeKutta(), 1000000) && ok;
ok = runBenchmark('dae-cvode', 'api/sed/dae/model.sedml') && ok;
ok = runBenchmark('tt04-3k', 'benchmark/tt04.sedml') && ok;
ok = runBenchmark('tt04-100k', 'benchmark/tt04.sedml', null, 100000) && ok;
ok = runBenchmark('hypercapnea', 'benchmark/hypercapnea.sedml') && ok;
ok = runBenchmark('hypercapnea-30k', 'benchmark/hypercapnea.sedml', null, 30000) && ok;

if (!ok) {
  process.exit(1);
}
