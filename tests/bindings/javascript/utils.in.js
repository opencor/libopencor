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
import { readFileSync } from 'node:fs';

export const SOME_PATH = '/some/path';

export const RESOURCE_LOCATION = '@CMAKE_SOURCE_DIR@/tests/res';

export const UNKNOWN_FILE = 'unknown_file.txt';
export const ERROR_CELLML_FILE = 'error.cellml';
export const CELLML_FILE = 'cellml_2.cellml';
export const SEDML_FILE = 'cellml_2.sedml';
export const COMBINE_ARCHIVE = 'cellml_2.omex';

export const LOCAL_FILE = somePath('file.txt');

// Note: the three following URLs use http rather than https to help with coverage testing.
export const HTTP_REMOTE_CELLML_FILE =
  'http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml';
export const HTTP_REMOTE_SEDML_FILE =
  'http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml';
export const HTTP_REMOTE_COMBINE_ARCHIVE =
  'http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex';
export const REMOTE_BASE_PATH = 'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res';
export const REMOTE_FILE = 'https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml';
export const ENCODED_REMOTE_FILE =
  'https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO%20BG%20example%203.1.cellml';
export const NON_ENCODED_REMOTE_FILE =
  'https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml';

export const UNKNOWN_CONTENTS = stringToArrayBuffer('Some unknown contents...');
export const CELLML_CONTENTS = stringToArrayBuffer(`@CELLML_CONTENTS@`);
export const SEDML_CONTENTS = stringToArrayBuffer(`@SEDML_CONTENTS@`);
export const COMBINE_ARCHIVE_CONTENTS = stringToArrayBuffer(atob('@BASE64_COMBINE_ARCHIVE_CONTENTS@'));

export function assertIssues(loc, logger, expectedIssues) {
  const issues = logger.issues;

  assert.strictEqual(issues.size(), expectedIssues.length);

  for (let i = 0; i < issues.size(); ++i) {
    assert.strictEqual(issues.get(i).type.value, expectedIssues[i][0].value);
    assert.strictEqual(issues.get(i).description, expectedIssues[i][1]);

    if (issues.get(i).type === loc.Issue.Type.ERROR) {
      assert.strictEqual(issues.get(i).typeAsString, 'Error');
    } else {
      assert.strictEqual(issues.get(i).typeAsString, 'Warning');
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
    assertValue(instanceTask.stateAsArray(i)[index], stateValues[i], statePrecisions[i]);
  }

  for (let i = 0; i < instanceTask.rateCount; ++i) {
    assertValue(instanceTask.rateAsArray(i)[index], rateValues[i], ratePrecisions[i]);
  }

  for (let i = 0; i < instanceTask.constantCount; ++i) {
    assertValue(instanceTask.constantAsArray(i)[index], constantValues[i], constantPrecisions[i]);
  }

  for (let i = 0; i < instanceTask.computedConstantCount; ++i) {
    assertValue(
      instanceTask.computedConstantAsArray(i)[index],
      computedConstantValues[i],
      computedConstantPrecisions[i]
    );
  }

  for (let i = 0; i < instanceTask.algebraicVariableCount; ++i) {
    assertValue(instanceTask.algebraicVariableAsArray(i)[index], algebraicValues[i], algebraicPrecisions[i]);
  }
}

export function allocateMemory(module, arrayBuffer) {
  const memPtr = module._malloc(arrayBuffer.length);

  module.HEAPU8.set(arrayBuffer, memPtr);

  return memPtr;
}

export function freeMemory(module, memPtr) {
  module._free(memPtr);
}

function stringToArrayBuffer(string) {
  return Uint8Array.from(string, (c) => c.charCodeAt(0));
}

export function somePath(resourceRelativePath) {
  return `${SOME_PATH}/${resourceRelativePath}`;
}

export function resourcePath(resourceRelativePath) {
  return `${RESOURCE_LOCATION}/${resourceRelativePath}`;
}

export function fileContents(filePath) {
  return Uint8Array.from(readFileSync(filePath));
}
