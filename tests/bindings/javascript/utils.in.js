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

import assert from 'node:assert';
import { setMaxListeners } from 'node:events';
import { readFileSync } from 'node:fs';

setMaxListeners(Infinity);

export const RESOURCE_LOCATION = '@CMAKE_SOURCE_DIR@/tests/res';

export const REMOTE_BASE_PATH = 'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res';
export const REMOTE_FILE = 'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml';

export function assertIssues(loc, logger, expectedIssues) {
  const issues = logger.issues;

  assert.strictEqual(issues.length, expectedIssues.length);

  for (const [i, issue] of Array.from(issues).entries()) {
    assert.strictEqual(issue.type.value, expectedIssues[i][0].value);
    assert.strictEqual(issue.description, expectedIssues[i][1]);

    if (issue.type === loc.Issue.Type.ERROR) {
      assert.strictEqual(issue.typeAsString, 'Error');
    } else {
      assert.strictEqual(issue.typeAsString, 'Warning');
    }
  }
}

export function assertValue(value, expectedValue, precision) {
  if (expectedValue === Infinity) {
    assert.strictEqual(value, Infinity);
  } else if (Number.isNaN(expectedValue)) {
    assert.ok(Number.isNaN(value));
  } else {
    assert.ok(Math.abs(value - expectedValue) < 0.5 * 10 ** -precision);
  }
}

export function assertValues(
  instanceTask,
  index,
  stateValues,
  statePrecisions,
  rateValues,
  ratePrecisions,
  constantValues,
  constantPrecisions,
  computedConstantValues,
  computedConstantPrecisions,
  algebraicValues,
  algebraicPrecisions
) {
  for (let i = 0; i < instanceTask.stateCount; ++i) {
    assertValue(instanceTask.state(i)[index], stateValues[i], statePrecisions[i]);
  }

  for (let i = 0; i < instanceTask.rateCount; ++i) {
    assertValue(instanceTask.rate(i)[index], rateValues[i], ratePrecisions[i]);
  }

  for (let i = 0; i < instanceTask.constantCount; ++i) {
    assertValue(instanceTask.constant(i)[index], constantValues[i], constantPrecisions[i]);
  }

  for (let i = 0; i < instanceTask.computedConstantCount; ++i) {
    assertValue(instanceTask.computedConstant(i)[index], computedConstantValues[i], computedConstantPrecisions[i]);
  }

  for (let i = 0; i < instanceTask.algebraicVariableCount; ++i) {
    assertValue(instanceTask.algebraicVariable(i)[index], algebraicValues[i], algebraicPrecisions[i]);
  }
}

export function resourcePath(resourceRelativePath = '') {
  return `${RESOURCE_LOCATION}/${resourceRelativePath}`;
}

export function fileContents(filePath) {
  return Uint8Array.from(readFileSync(filePath));
}
