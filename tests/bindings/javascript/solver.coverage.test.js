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
import * as utils from "./utils.js";
import { expectIssues } from "./utils.js";

const loc = await libOpenCOR();

describe("Solver coverage tests", () => {
  let algebraicSedChangesContentsPtr;
  let odeSedChangesContentsPtr;

  beforeAll(() => {
    algebraicSedChangesContentsPtr = utils.allocateMemory(
      loc,
      utils.ALGEBRAIC_SED_CHANGES_CONTENTS,
    );
    odeSedChangesContentsPtr = utils.allocateMemory(
      loc,
      utils.ODE_SED_CHANGES_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(loc, algebraicSedChangesContentsPtr);
    utils.freeMemory(loc, odeSedChangesContentsPtr);
  });

  test("ODE changes", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      odeSedChangesContentsPtr,
      utils.ODE_SED_CHANGES_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    expectIssues(loc, instance, [
      [
        loc.Issue.Type.WARNING,
        "The variable of integration 'time' in component 'environment'cannot be changed. Only state variables and constants can be changed.",
      ],
      [
        loc.Issue.Type.WARNING,
        "The variable 'membrane' in component 'X'could not be found and therefore could not be changed.",
      ],
      [
        loc.Issue.Type.WARNING,
        "The computed constant 'E_Na' in component 'sodium_channel' cannot be changed. Only state variables and constants can be changed.",
      ],
      [
        loc.Issue.Type.WARNING,
        "The algebraic variable 'i_Stim' in component 'membrane' cannot be changed. Only state variables and constants can be changed.",
      ],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  /*---GRY--- ENABLE THIS TEST ONCE WE CAN INTERPRET NLA-BASED MODELS.
  test("Algebraic changes", () => {
    // We want to solve a system of three unknowns:
    // Variables:
    //  • a: 0
    //  • x: 1 ->  3
    //  • y: 1 -> -5
    //  • z: 1 ->  7
    // Equations:
    //  • k = a
    //  •  x +  y +  z +  k =  5
    //  • 6x - 4y + 5z - 3k = 73
    //  • 5x + 2y + 2z - 5k = 19
    //
    // When a = 0, we have:
    //  • x =  3
    //  • y = -5
    //  • z =  7
    // but our SED-ML file has a change attribute that sets a to 13, so we should now have:
    //  • x =  100/3  =  33.333333333333336
    //  • y = -356/27 = -13.185185185185185
    //  • z = -760/27 = -28.148148148148145

    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      algebraicSedChangesContentsPtr,
      utils.ALGEBRAIC_SED_CHANGES_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    const instanceTask = instance.tasks.get(0);

    expect(instanceTask.stateCount).toBe(0);
    expect(instanceTask.rateCount).toBe(0);
    expect(instanceTask.constantCount).toBe(0);
    expect(instanceTask.computedConstantCount).toBe(0);
    expect(instanceTask.algebraicCount).toBe(3);

    expect(instanceTask.algebraic(0).get(0)).toBeCloseTo(33.33333, 5);
    expect(instanceTask.algebraic(1).get(0)).toBeCloseTo(-28.14815, 5);
    expect(instanceTask.algebraic(2).get(0)).toBeCloseTo(-13.18519, 5);

    instance.delete();
    document.delete();
    file.delete();
  });
  */
});
