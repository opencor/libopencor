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

from libopencor import File
import utils


WINDOWS_FILE = "C:\\some\\path\\file.txt"
UNIX_FILE = "/some/path/file.txt"
URL_BASED_WINDOWS_FILE = "file:///C:/some/path/file.txt"
URL_BASED_UNIX_FILE = "file:///some/path/file.txt"
REMOTE_FILE = "https://models.physiomeproject.org/workspace/noble_1962/rawfile/c70f8962407db00673f1fdcac9f35a2593781c17/noble_1962.cellml"
UNKNOWN_FILE = "unknown_file.txt"
CELLML_1_X_FILE = "cellml_1_x.cellml"
SEDML_1_X_FILE = "cellml_1_x.sedml"
COMBINE_1_X_ARCHIVE = "cellml_1_x.omex"
CELLML_2_FILE = "cellml_2.cellml"
SEDML_2_FILE = "cellml_2.sedml"
COMBINE_2_ARCHIVE = "cellml_2.omex"


def test_windows_file():
    f = File(WINDOWS_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == WINDOWS_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + WINDOWS_FILE


def test_unix_file():
    f = File(UNIX_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == UNIX_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + UNIX_FILE


def test_url_based_windows_file():
    f = File(URL_BASED_WINDOWS_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == WINDOWS_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + WINDOWS_FILE


def test_url_based_unix_file():
    f = File(URL_BASED_UNIX_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == UNIX_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + UNIX_FILE


def test_remote_file():
    f = File(REMOTE_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == ""
    assert f.url == REMOTE_FILE
    assert repr(f) == "Remote file: " + REMOTE_FILE


def test_cellml_1_x_file():
    f = File(utils.resource_path(CELLML_1_X_FILE))

    f.resolve()

    assert f.type == File.Type.CellmlFile


def test_sedml_1_x_file():
    f = File(utils.resource_path(SEDML_1_X_FILE))

    f.resolve()

    assert f.type == File.Type.SedmlFile


def test_combine_1_x_archive():
    f = File(utils.resource_path(COMBINE_1_X_ARCHIVE))

    f.resolve()

    assert f.type == File.Type.CombineArchive


def test_cellml_2_file():
    f = File(utils.resource_path(CELLML_2_FILE))

    f.resolve()

    assert f.type == File.Type.CellmlFile


def test_sedml_2_file():
    f = File(utils.resource_path(SEDML_2_FILE))

    f.resolve()

    assert f.type == File.Type.SedmlFile


def test_combine_2_archive():
    f = File(utils.resource_path(COMBINE_2_ARCHIVE))

    f.resolve()

    assert f.type == File.Type.CombineArchive


def test_unknown_file():
    f = File(utils.resource_path(UNKNOWN_FILE))

    f.resolve()

    assert f.type == File.Type.UnknownFile
