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


from libopencor import File, Issue
import utils
from utils import assert_issues


expected_unknown_file_issues = [
    [
        Issue.Type.Error,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
    ],
]


def test_type_irretrievable_file():
    expected_non_existing_file_issues = [
        [Issue.Type.Error, "The file does not exist."],
    ]

    file = File(utils.resource_path(utils.IRRETRIEVABLE_FILE))

    assert file.type == File.Type.IrretrievableFile
    assert_issues(file, expected_non_existing_file_issues)


def test_type_unknown_file():
    file = File(utils.resource_path(utils.UNKNOWN_FILE))

    assert file.type == File.Type.UnknownFile
    assert_issues(file, expected_unknown_file_issues)


def test_type_sbml_file():
    file = File(utils.resource_path(utils.SBML_FILE))

    assert file.type == File.Type.UnknownFile
    assert_issues(file, expected_unknown_file_issues)


def test_type_error_sedml_file():
    file = File(utils.resource_path(utils.ERROR_SEDML_FILE))

    assert file.type == File.Type.SedmlFile


def test_type_cellml_1_x_file():
    file = File(utils.resource_path(utils.CELLML_1_X_FILE))

    assert file.type == File.Type.CellmlFile


def test_type_sedml_1_x_file():
    file = File(utils.resource_path(utils.SEDML_1_X_FILE))

    assert file.type == File.Type.SedmlFile


def test_type_combine_1_x_archive():
    file = File(utils.resource_path(utils.COMBINE_1_X_ARCHIVE))

    assert file.type == File.Type.CombineArchive


def test_type_cellml_2_file():
    file = File(utils.resource_path(utils.CELLML_2_FILE))

    assert file.type == File.Type.CellmlFile


def test_type_sedml_2_file():
    file = File(utils.resource_path(utils.SEDML_2_FILE))

    assert file.type == File.Type.SedmlFile


def test_type_combine_2_archive():
    file = File(utils.resource_path(utils.COMBINE_2_ARCHIVE))

    assert file.type == File.Type.CombineArchive


def test_type_unknown_virtual_file():
    file = File(utils.LOCAL_FILE)

    file.contents = utils.string_to_list(utils.SOME_UNKNOWN_CONTENTS)

    assert file.type == File.Type.UnknownFile
    assert_issues(file, expected_unknown_file_issues)


def test_type_cellml_virtual_file():
    file = File(utils.LOCAL_FILE)

    file.contents = utils.string_to_list(utils.SOME_CELLML_CONTENTS)

    assert file.type == File.Type.CellmlFile


def test_type_sedml_virtual_file():
    file = File(utils.LOCAL_FILE)

    file.contents = utils.string_to_list(utils.SOME_SEDML_CONTENTS)

    assert file.type == File.Type.SedmlFile
