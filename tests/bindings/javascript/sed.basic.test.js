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

describe("Sed basic tests", () => {
  let unknownContentsPtr;
  let cellmlContentsPtr;
  let sedmlContentsPtr;
  let sedmlWithAbsoluteCellmlFileContentsPtr;
  let sedmlWithRemoteCellmlFileContentsPtr;
  let combineArchiveContentsPtr;
  let combineArchiveWithNoManifestFileContentsPtr;
  let combineArchiveWithNoMasterFileContentsPtr;
  let combineArchiveWithUnknownDirectCellmlFileContentsPtr;
  let combineArchiveWithUnknownIndirectCellmlFileContentsPtr;
  let combineArchiveWithUnknownSedmlFileContentsPtr;

  beforeAll(() => {
    unknownContentsPtr = utils.allocateMemory(loc, utils.UNKNOWN_CONTENTS);
    cellmlContentsPtr = utils.allocateMemory(loc, utils.CELLML_CONTENTS);
    sedmlContentsPtr = utils.allocateMemory(loc, utils.SEDML_CONTENTS);
    sedmlWithAbsoluteCellmlFileContentsPtr = utils.allocateMemory(
      loc,
      utils.SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS,
    );
    sedmlWithRemoteCellmlFileContentsPtr = utils.allocateMemory(
      loc,
      utils.SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS,
    );
    combineArchiveContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_CONTENTS,
    );
    combineArchiveWithNoManifestFileContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_WITH_NO_MANIFEST_FILE_CONTENTS,
    );
    combineArchiveWithNoMasterFileContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_WITH_NO_MASTER_FILE_CONTENTS,
    );
    combineArchiveWithUnknownDirectCellmlFileContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_WITH_UNKNOWN_DIRECT_CELLML_FILE_CONTENTS,
    );
    combineArchiveWithUnknownIndirectCellmlFileContentsPtr =
      utils.allocateMemory(
        loc,
        utils.COMBINE_ARCHIVE_WITH_UNKNOWN_INDIRECT_CELLML_FILE_CONTENTS,
      );
    combineArchiveWithUnknownSedmlFileContentsPtr = utils.allocateMemory(
      loc,
      utils.COMBINE_ARCHIVE_WITH_UNKNOWN_SEDML_FILE_CONTENTS,
    );
  });

  afterAll(() => {
    utils.freeMemory(loc, unknownContentsPtr);
    utils.freeMemory(loc, cellmlContentsPtr);
    utils.freeMemory(loc, sedmlContentsPtr);
    utils.freeMemory(loc, sedmlWithAbsoluteCellmlFileContentsPtr);
    utils.freeMemory(loc, sedmlWithRemoteCellmlFileContentsPtr);
    utils.freeMemory(loc, combineArchiveContentsPtr);
    utils.freeMemory(loc, combineArchiveWithNoManifestFileContentsPtr);
    utils.freeMemory(loc, combineArchiveWithNoMasterFileContentsPtr);
    utils.freeMemory(loc, combineArchiveWithUnknownDirectCellmlFileContentsPtr);
    utils.freeMemory(
      loc,
      combineArchiveWithUnknownIndirectCellmlFileContentsPtr,
    );
    utils.freeMemory(loc, combineArchiveWithUnknownSedmlFileContentsPtr);
  });

  test("No file", () => {
    const document = new loc.SedDocument();

    expect(document.hasIssues).toBe(false);

    document.delete();
  });

  test("Unknown file", () => {
    const file = new loc.File(utils.UNKNOWN_FILE);

    file.setContents(unknownContentsPtr, utils.UNKNOWN_CONTENTS.length);

    const document = new loc.SedDocument(file);

    expectIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using an unknown file.",
      ],
    ]);

    document.delete();
    file.delete();
  });

  test("CellML file", () => {
    const file = new loc.File(utils.CELLML_FILE);

    file.setContents(cellmlContentsPtr, utils.CELLML_CONTENTS.length);

    const document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(false);

    document.delete();
    file.delete();
  });

  test("SED-ML file", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(sedmlContentsPtr, utils.SEDML_CONTENTS.length);

    let document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(true);

    document.delete();

    const neededFile = new loc.File(utils.CELLML_FILE);

    document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(false);

    document.delete();
    neededFile.delete();
    file.delete();
  });

  test("SED-ML file with absolute CellML file", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(
      sedmlWithAbsoluteCellmlFileContentsPtr,
      utils.SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS.length,
    );

    let document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(true);

    document.delete();

    const neededFile = new loc.File(utils.LOCAL_FILE);

    document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(false);

    document.delete();
    neededFile.delete();
    file.delete();
  });

  test("SED-ML file with remote CellML file", () => {
    const file = new loc.File(utils.SEDML_FILE);

    file.setContents(
      sedmlWithRemoteCellmlFileContentsPtr,
      utils.SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS.length,
    );

    let document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(true);

    document.delete();

    const neededFile = new loc.File(utils.REMOTE_FILE);

    document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(false);

    document.delete();
    neededFile.delete();
    file.delete();
  });

  test("COMBINE archive", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveContentsPtr,
      utils.COMBINE_ARCHIVE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);

    expect(document.hasIssues).toBe(false);

    document.delete();
    file.delete();
  });

  test("COMBINE archive with no manifest file", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveWithNoManifestFileContentsPtr,
      utils.COMBINE_ARCHIVE_WITH_NO_MANIFEST_FILE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);

    expectIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
      ],
    ]);

    document.delete();
    file.delete();
  });

  test("COMBINE archive with no master file", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveWithNoMasterFileContentsPtr,
      utils.COMBINE_ARCHIVE_WITH_NO_MASTER_FILE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);

    expectIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
      ],
    ]);

    document.delete();
    file.delete();
  });

  test("COMBINE archive with unknown direct CellML file", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveWithUnknownDirectCellmlFileContentsPtr,
      utils.COMBINE_ARCHIVE_WITH_UNKNOWN_DIRECT_CELLML_FILE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);

    expectIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
      ],
    ]);

    document.delete();
    file.delete();
  });

  test("COMBINE archive with unknown indirect CellML file", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveWithUnknownIndirectCellmlFileContentsPtr,
      utils.COMBINE_ARCHIVE_WITH_UNKNOWN_INDIRECT_CELLML_FILE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);
    const instance = document.instantiate();

    instance.run();

    expectIssues(loc, instance, [
      [loc.Issue.Type.ERROR, "Task 'task1' requires a model of CellML type."],
    ]);

    instance.delete();
    document.delete();
    file.delete();
  });

  test("COMBINE archive with unknown SED-ML file", () => {
    const file = new loc.File(utils.COMBINE_ARCHIVE);

    file.setContents(
      combineArchiveWithUnknownSedmlFileContentsPtr,
      utils.COMBINE_ARCHIVE_WITH_UNKNOWN_SEDML_FILE_CONTENTS.length,
    );

    const document = new loc.SedDocument(file);

    expectIssues(loc, document, [
      [
        loc.Issue.Type.ERROR,
        "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
      ],
    ]);

    document.delete();
    file.delete();
  });
});
