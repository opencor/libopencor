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

describe("Sed basic tests", () => {
  let someUnknownContentsPtr;
  let someCellmlContentsPtr;
  let someSedmlContentsPtr;
  let someSedmlWithAbsoluteCellmlFileContentsPtr;
  let someSedmlWithRemoteCellmlFileContentsPtr;
  let someCombineArchiveContentsPtr;
  let someCombineArchiveWithNoManifestFileContentsPtr;
  let someCombineArchiveWithNoMasterFileContentsPtr;
  let someCombineArchiveWithSbmlFileAsMasterFileContentsPtr;

  beforeAll(() => {
    someUnknownContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_UNKNOWN_CONTENTS,
    );
    someCellmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_CELLML_CONTENTS,
    );
    someSedmlContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_CONTENTS,
    );
    someSedmlWithAbsoluteCellmlFileContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS,
    );
    someSedmlWithRemoteCellmlFileContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS,
    );
    someCombineArchiveContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS,
    );
    someCombineArchiveWithNoManifestFileContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_COMBINE_ARCHIVE_WITH_NO_MANIFEST_FILE_CONTENTS,
    );
    someCombineArchiveWithNoMasterFileContentsPtr = utils.allocateMemory(
      libopencor,
      utils.SOME_COMBINE_ARCHIVE_WITH_NO_MASTER_FILE_CONTENTS,
    );
    someCombineArchiveWithSbmlFileAsMasterFileContentsPtr =
      utils.allocateMemory(
        libopencor,
        utils.SOME_COMBINE_ARCHIVE_WITH_SBML_FILE_AS_MASTER_FILE_CONTENTS,
      );
  });

  afterAll(() => {
    utils.freeMemory(libopencor, someUnknownContentsPtr);
    utils.freeMemory(libopencor, someCellmlContentsPtr);
    utils.freeMemory(libopencor, someSedmlContentsPtr);
    utils.freeMemory(libopencor, someSedmlWithAbsoluteCellmlFileContentsPtr);
    utils.freeMemory(libopencor, someSedmlWithRemoteCellmlFileContentsPtr);
    utils.freeMemory(libopencor, someCombineArchiveContentsPtr);
    utils.freeMemory(
      libopencor,
      someCombineArchiveWithNoManifestFileContentsPtr,
    );
    utils.freeMemory(libopencor, someCombineArchiveWithNoMasterFileContentsPtr);
    utils.freeMemory(
      libopencor,
      someCombineArchiveWithSbmlFileAsMasterFileContentsPtr,
    );
  });

  test("No file", () => {
    const document = new libopencor.SedDocument();

    expect(document.hasIssues()).toBe(false);

    document.delete();
  });

  test("Unknown file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.UNKNOWN_FILE));

    file.setContents(
      someUnknownContentsPtr,
      utils.SOME_UNKNOWN_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);

    expectIssues(libopencor, document, [
      [
        libopencor.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using an unknown file.",
      ],
    ]);

    document.delete();
    file.delete();
  });

  test("CellML file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.CELLML_FILE));

    file.setContents(someCellmlContentsPtr, utils.SOME_CELLML_CONTENTS.length);

    const document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(false);

    document.delete();
    file.delete();
  });

  test("SED-ML file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.SEDML_FILE));

    file.setContents(someSedmlContentsPtr, utils.SOME_SEDML_CONTENTS.length);

    let document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(true);

    document.delete();

    const neededFile = new libopencor.File(
      utils.resourcePath(utils.CELLML_FILE),
    );

    document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(false);

    document.delete();
    neededFile.delete();
    file.delete();
  });

  test("SED-ML file with absolute CellML file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.SEDML_FILE));

    file.setContents(
      someSedmlWithAbsoluteCellmlFileContentsPtr,
      utils.SOME_SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS.length,
    );

    let document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(true);

    document.delete();

    const neededFile = new libopencor.File(utils.LOCAL_FILE);

    document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(false);

    document.delete();
    neededFile.delete();
    file.delete();
  });

  test("SED-ML file with remote CellML file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.SEDML_FILE));

    file.setContents(
      someSedmlWithRemoteCellmlFileContentsPtr,
      utils.SOME_SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS.length,
    );

    let document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(true);

    document.delete();

    const neededFile = new libopencor.File(utils.REMOTE_FILE);

    document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(false);

    document.delete();
    neededFile.delete();
    file.delete();
  });

  test("COMBINE archive", () => {
    const file = new libopencor.File(utils.resourcePath(utils.COMBINE_ARCHIVE));

    file.setContents(
      someCombineArchiveContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);

    expect(document.hasIssues()).toBe(false);

    document.delete();
    file.delete();
  });

  test("COMBINE archive with no manifest file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.COMBINE_ARCHIVE));

    file.setContents(
      someCombineArchiveWithNoManifestFileContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_WITH_NO_MANIFEST_FILE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);

    expectIssues(libopencor, document, [
      [
        libopencor.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
      ],
    ]);

    document.delete();
    file.delete();
  });

  test("COMBINE archive with no master file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.COMBINE_ARCHIVE));

    file.setContents(
      someCombineArchiveWithNoMasterFileContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_WITH_NO_MASTER_FILE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);

    expectIssues(libopencor, document, [
      [
        libopencor.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
      ],
    ]);

    document.delete();
    file.delete();
  });

  test("COMBINE archive with SBML file as master file", () => {
    const file = new libopencor.File(utils.resourcePath(utils.COMBINE_ARCHIVE));

    file.setContents(
      someCombineArchiveWithSbmlFileAsMasterFileContentsPtr,
      utils.SOME_COMBINE_ARCHIVE_WITH_SBML_FILE_AS_MASTER_FILE_CONTENTS.length,
    );

    const document = new libopencor.SedDocument(file);

    expectIssues(libopencor, document, [
      [
        libopencor.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
      ],
    ]);

    document.delete();
    file.delete();
  });
});
