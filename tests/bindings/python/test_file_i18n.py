# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import libopencor as loc
import platform
import utils


def test_arabic_files():
    assert (
        loc.File(utils.resource_path("i18n/ملف/file.cellml")).type
        == loc.File.Type.CellmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/ملف/file.sedml")).type
        == loc.File.Type.SedmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/ملف/file.sedml")).type
        == loc.File.Type.SedmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/ملف/file.omex")).type
        == loc.File.Type.CombineArchive
    )


def test_chinese_files():
    assert (
        loc.File(utils.resource_path("i18n/文件夹/file.cellml")).type
        == loc.File.Type.CellmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/文件夹/file.sedml")).type
        == loc.File.Type.SedmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/文件夹/file.omex")).type
        == loc.File.Type.CombineArchive
    )


def test_hindi_files():
    assert (
        loc.File(utils.resource_path("i18n/फोल्डर/file.cellml")).type
        == loc.File.Type.CellmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/फोल्डर/file.sedml")).type
        == loc.File.Type.SedmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/फोल्डर/file.omex")).type
        == loc.File.Type.CombineArchive
    )


def test_japanese_files():
    # Note: see the comment in the corresponding C++ test.

    if platform.system() == "Darwin":
        assert (
            loc.File(utils.resource_path("i18n/フォルダ/file.cellml")).type
            == loc.File.Type.CellmlFile
        )
        assert (
            loc.File(utils.resource_path("i18n/フォルダ/file.sedml")).type
            == loc.File.Type.SedmlFile
        )
        assert (
            loc.File(utils.resource_path("i18n/フォルダ/file.omex")).type
            == loc.File.Type.CombineArchive
        )
    else:
        pass


def test_russian_files():
    assert (
        loc.File(utils.resource_path("i18n/папка/file.cellml")).type
        == loc.File.Type.CellmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/папка/file.sedml")).type
        == loc.File.Type.SedmlFile
    )
    assert (
        loc.File(utils.resource_path("i18n/папка/file.omex")).type
        == loc.File.Type.CombineArchive
    )
