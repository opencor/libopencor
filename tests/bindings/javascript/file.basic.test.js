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

const libopencor = await libOpenCOR();

const expectedUnknownFileIssues = [
  [
    libopencor.Issue.Type.ERROR,
    "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
  ],
];

describe("File basic tests", () => {
  let someUnknownContentsPtr;

  beforeAll(() => {
    someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someUnknownContentsPtr);
  });

  test("Local file", () => {
    const file = new libopencor.File(utils.LOCAL_FILE);

    expect(file.type.value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.fileName).toBe(utils.LOCAL_FILE);
    expect(file.url).toBe("");
    expect(file.path).toBe(utils.LOCAL_FILE);
    expect(file.contents()).toStrictEqual(utils.NO_CONTENTS);
    expectIssues(libopencor, file, expectedUnknownFileIssues);

    file.setContents(
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.type.value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.contents()).toStrictEqual(utils.SOME_UNKNOWN_CONTENTS);
    expectIssues(libopencor, file, expectedUnknownFileIssues);

    file.delete();
  });

  test("Remote file", () => {
    const file = new libopencor.File(utils.REMOTE_FILE);

    expect(file.type.value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.fileName).toBe("/some/path/file");
    expect(file.url).toBe(utils.REMOTE_FILE);
    expect(file.path).toBe(utils.REMOTE_FILE);
    expect(file.contents()).toStrictEqual(utils.NO_CONTENTS);
    expectIssues(libopencor, file, expectedUnknownFileIssues);

    file.setContents(
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    expect(file.type.value).toBe(libopencor.File.Type.UNKNOWN_FILE.value);
    expect(file.contents()).toStrictEqual(utils.SOME_UNKNOWN_CONTENTS);
    expectIssues(libopencor, file, expectedUnknownFileIssues);

    file.delete();
  });

  test("File manager", () => {
    const fileManager = libopencor.FileManager.instance();

    expect(fileManager.hasFiles).toBe(false);
    expect(fileManager.fileCount).toBe(0);
    expect(fileManager.files.size()).toBe(0);
    expect(fileManager.file(0)).toStrictEqual(null);
    expect(fileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE)).toStrictEqual(
      null,
    );

    const localFile = new libopencor.File(utils.LOCAL_FILE);
    const sameFileManager = libopencor.FileManager.instance();

    expect(sameFileManager.hasFiles).toBe(true);
    expect(sameFileManager.fileCount).toBe(1);
    expect(sameFileManager.files.size()).toBe(1);
    expect(fileManager.file(0)).toStrictEqual(localFile);
    expect(
      sameFileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE),
    ).toStrictEqual(localFile);

    const remoteFile = new libopencor.File(utils.REMOTE_FILE);

    expect(fileManager.hasFiles).toBe(true);
    expect(fileManager.fileCount).toBe(2);
    expect(fileManager.files.size()).toBe(2);
    expect(fileManager.file(1)).toStrictEqual(remoteFile);
    expect(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE)).toStrictEqual(
      remoteFile,
    );

    sameFileManager.unmanage(localFile);

    expect(sameFileManager.hasFiles).toBe(true);
    expect(sameFileManager.fileCount).toBe(1);
    expect(sameFileManager.files.size()).toBe(1);
    expect(fileManager.file(1)).toStrictEqual(null);
    expect(
      sameFileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE),
    ).toStrictEqual(null);

    sameFileManager.manage(localFile);

    expect(sameFileManager.hasFiles).toBe(true);
    expect(sameFileManager.fileCount).toBe(2);
    expect(sameFileManager.files.size()).toBe(2);
    expect(fileManager.file(1)).toStrictEqual(localFile);
    expect(
      sameFileManager.fileFromFileNameOrUrl(utils.LOCAL_FILE),
    ).toStrictEqual(localFile);

    fileManager.reset();

    expect(fileManager.hasFiles).toBe(false);
    expect(fileManager.fileCount).toBe(0);
    expect(fileManager.files.size()).toBe(0);
    expect(fileManager.file(0)).toStrictEqual(null);
    expect(fileManager.file(1)).toStrictEqual(null);
    expect(fileManager.fileFromFileNameOrUrl(utils.REMOTE_FILE)).toStrictEqual(
      null,
    );
    expect(fileManager.fileFromFileNameOrUrl(utils.UNKNOWN_FILE)).toStrictEqual(
      null,
    );

    localFile.delete();
    remoteFile.delete();
  });
});
