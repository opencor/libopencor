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

const UNIX_LOCAL_FILE = "/some/path/file.txt";

describe("File basic tests", () => {
    let contentsPtr;
    let contentsLength;

    beforeAll(async () => {
        const contents = await utils.CONTENTS.arrayBuffer();

        contentsLength = contents.byteLength;
        contentsPtr = libopencor._malloc(contentsLength);

        const contentsMem = new Uint8Array(libopencor.HEAPU8.buffer, contentsPtr, contentsLength);

        contentsMem.set(new Uint8Array(contents));
    });

    afterAll(() => {
        libopencor._free(contentsPtr);
    });

    test("Local virtual file", async () => {
        const file = new libopencor.File(UNIX_LOCAL_FILE, contentsPtr, contentsLength);

        expect(file.type().value).toBe(libopencor.File.Type.UNRESOLVED.value);
        expect(file.fileName()).toBe(UNIX_LOCAL_FILE);
        expect(file.url()).toBe("");
        expect(file.isVirtual()).toBe(true);
    });

    test("Remote virtual file", () => {
        const file = new libopencor.File(utils.REMOTE_FILE, contentsPtr, contentsLength);

        expect(file.type().value).toBe(libopencor.File.Type.UNRESOLVED.value);
        expect(file.fileName()).toBe("");
        expect(file.url()).toBe(utils.REMOTE_FILE);
        expect(file.isVirtual()).toBe(true);
    });
});
