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

const libopencor = await libOpenCOR();

describe("File coverage tests", () => {
    test("Underconstrained CellML file", () => {
        const file = new libopencor.File(utils.resourcePath("underconstrained.cellml"));

        expect(file.resolve().value).toBe(libopencor.File.Status.OK.value);
        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });

    test("Remote file", () => {
        const file = new libopencor.File(utils.REMOTE_FILE);

        expect(file.resolve().value).toBe(libopencor.File.Status.OK.value);
        expect(file.instantiate().value).toBe(libopencor.File.Status.OK.value);
    });

    test("Not remote file", () => {
        const file = new libopencor.File(utils.NOT_REMOTE_FILE);

        expect(file.resolve().value).toBe(libopencor.File.Status.NON_RETRIEVABLE_FILE.value);
        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_RETRIEVABLE_FILE.value);
    });
});
