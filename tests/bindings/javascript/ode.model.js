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

import { expectValues } from "./utils.js";

export function run(
  libopencor,
  document,
  stateValues,
  rateValues,
  constantValues,
  computedConstantValues,
  algebraicValues,
) {
  const simulation = document.simulations().get(0);

  simulation.setOutputEndTime(50.0);
  simulation.setNumberOfSteps(50000);

  const instance = document.instantiate();

  instance.run();

  const instanceTask = instance.tasks().get(0);

  expectValues(
    libopencor,
    instanceTask,
    13000,
    stateValues,
    rateValues,
    constantValues,
    computedConstantValues,
    algebraicValues,
  );
}
