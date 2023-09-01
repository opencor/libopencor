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
  test("Forward Euler method", () => {
    const solver = new libopencor.Solver(
      libopencor.Solver.Method.FORWARD_EULER,
    );

    expect(solver.type().value).toBe(libopencor.Solver.Type.ODE.value);
    expect(solver.method().value).toBe(
      libopencor.Solver.Method.FORWARD_EULER.value,
    );
    expect(solver.name()).toBe("Forward Euler");
  });
});
