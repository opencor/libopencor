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
import test from 'node:test';

import libOpenCOR from './libopencor.js';
import * as utils from './utils.js';

const loc = await libOpenCOR();

test.describe('File i18n tests', () => {
  test.beforeEach(() => {
    loc.FileManager.instance().reset();
  });

  test('Arabic files', () => {
    const cellmlFile = new loc.File(utils.resourcePath('i18n/ملف/file.cellml'));
    const sedmlFile = new loc.File(utils.resourcePath('i18n/ملف/file.sedml'));
    const omexFile = new loc.File(utils.resourcePath('i18n/ملف/file.omex'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));
    sedmlFile.setContents(utils.fileContents(sedmlFile.path));
    omexFile.setContents(utils.fileContents(omexFile.path));

    assert.strictEqual(cellmlFile.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(sedmlFile.type.value, loc.File.Type.SEDML_FILE.value);
    assert.strictEqual(omexFile.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });

  test('Chinese files', () => {
    const cellmlFile = new loc.File(utils.resourcePath('i18n/文件夹/file.cellml'));
    const sedmlFile = new loc.File(utils.resourcePath('i18n/文件夹/file.sedml'));
    const omexFile = new loc.File(utils.resourcePath('i18n/文件夹/file.omex'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));
    sedmlFile.setContents(utils.fileContents(sedmlFile.path));
    omexFile.setContents(utils.fileContents(omexFile.path));

    assert.strictEqual(cellmlFile.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(sedmlFile.type.value, loc.File.Type.SEDML_FILE.value);
    assert.strictEqual(omexFile.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });

  test('Hindi files', () => {
    const cellmlFile = new loc.File(utils.resourcePath('i18n/फोल्डर/file.cellml'));
    const sedmlFile = new loc.File(utils.resourcePath('i18n/फोल्डर/file.sedml'));
    const omexFile = new loc.File(utils.resourcePath('i18n/फोल्डर/file.omex'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));
    sedmlFile.setContents(utils.fileContents(sedmlFile.path));
    omexFile.setContents(utils.fileContents(omexFile.path));

    assert.strictEqual(cellmlFile.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(sedmlFile.type.value, loc.File.Type.SEDML_FILE.value);
    assert.strictEqual(omexFile.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });

  test('Japanese files', () => {
    const cellmlFile = new loc.File(utils.resourcePath('i18n/フォルダ/file.cellml'));
    const sedmlFile = new loc.File(utils.resourcePath('i18n/フォルダ/file.sedml'));
    const omexFile = new loc.File(utils.resourcePath('i18n/フォルダ/file.omex'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));
    sedmlFile.setContents(utils.fileContents(sedmlFile.path));
    omexFile.setContents(utils.fileContents(omexFile.path));

    assert.strictEqual(cellmlFile.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(sedmlFile.type.value, loc.File.Type.SEDML_FILE.value);
    assert.strictEqual(omexFile.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });

  test('Russian files', () => {
    const cellmlFile = new loc.File(utils.resourcePath('i18n/папка/file.cellml'));
    const sedmlFile = new loc.File(utils.resourcePath('i18n/папка/file.sedml'));
    const omexFile = new loc.File(utils.resourcePath('i18n/папка/file.omex'));

    cellmlFile.setContents(utils.fileContents(cellmlFile.path));
    sedmlFile.setContents(utils.fileContents(sedmlFile.path));
    omexFile.setContents(utils.fileContents(omexFile.path));

    assert.strictEqual(cellmlFile.type.value, loc.File.Type.CELLML_FILE.value);
    assert.strictEqual(sedmlFile.type.value, loc.File.Type.SEDML_FILE.value);
    assert.strictEqual(omexFile.type.value, loc.File.Type.COMBINE_ARCHIVE.value);
  });
});
