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

describe("File instantiate tests", () => {
    test("Instantiate non-retrievable file", () => {
        const file = new libopencor.File(utils.NON_RETRIEVABLE_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_RETRIEVABLE_FILE.value);
    });

    test("Instantiate unknown file", () => {
        const file = new libopencor.File(utils.UNKNOWN_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });

    test("Instantiate SBML file", () => {
        const file = new libopencor.File(utils.SBML_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });

    test("Instantiate error SED-ML file", () => {
        const file = new libopencor.File(utils.ERROR_SEDML_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });

    test("Instantiate CellML 1.x file", () => {
        const file = new libopencor.File(utils.CELLML_1_X_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.OK.value);
    });

    test("Instantiate SED-ML 1.x file", () => {
        const file = new libopencor.File(utils.SEDML_1_X_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });

    test("Instantiate COMBINE 1.x archive", () => {
        const file = new libopencor.File(utils.COMBINE_1_X_ARCHIVE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });

    test("Instantiate CellML 2 file", () => {
        const file = new libopencor.File(utils.CELLML_2_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.OK.value);
    });

    test("Instantiate SED-ML 2 file", () => {
        const file = new libopencor.File(utils.SEDML_2_FILE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });

    test("Instantiate COMBINE 2 archive", () => {
        const file = new libopencor.File(utils.COMBINE_2_ARCHIVE);

        expect(file.instantiate().value).toBe(libopencor.File.Status.NON_INSTANTIABLE_FILE.value);
    });
});
