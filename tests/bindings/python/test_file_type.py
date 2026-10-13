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
import utils
from utils import assert_issues

expected_unknown_file_issues = [
    [
        loc.Issue.Type.Error,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
    ],
]
expected_irretrievable_file_issues = [
    [loc.Issue.Type.Error, "The file does not exist."],
]


def test_irretrievable_file():
    file = loc.File(utils.resource_path("irretrievable_file.txt"))

    assert file.type == loc.File.Type.IrretrievableFile
    assert_issues(file, expected_irretrievable_file_issues)


def test_unknown_file():
    file = loc.File(utils.resource_path("unknown_file.txt"))

    assert file.type == loc.File.Type.UnknownFile
    assert_issues(file, expected_unknown_file_issues)


def test_sbml_file():
    file = loc.File(utils.resource_path("sbml.sbml"))

    assert file.type == loc.File.Type.UnknownFile
    assert_issues(file, expected_unknown_file_issues)


def test_error_sedml_file():
    file = loc.File(utils.resource_path("error.sedml"))

    assert file.type == loc.File.Type.SedmlFile


def test_cellml_1_x_file():
    file = loc.File(utils.resource_path("cellml_1_x.cellml"))

    assert file.type == loc.File.Type.CellmlFile


def test_sedml_1_x_file():
    file = loc.File(utils.resource_path("cellml_1_x.sedml"))

    assert file.type == loc.File.Type.SedmlFile


def test_combine_1_x_archive():
    file = loc.File(utils.resource_path("cellml_1_x.omex"))

    assert file.type == loc.File.Type.CombineArchive


def test_cellml_2_file():
    file = loc.File(utils.resource_path("cellml_2.cellml"))

    assert file.type == loc.File.Type.CellmlFile


def test_sedml_2_file():
    file = loc.File(utils.resource_path("cellml_2.sedml"))

    assert file.type == loc.File.Type.SedmlFile


def test_combine_2_archive():
    file = loc.File(utils.resource_path("cellml_2.omex"))

    assert file.type == loc.File.Type.CombineArchive


def test_unknown_virtual_file():
    file_path = utils.resource_path("unknown_file.txt")
    file = loc.File(file_path, False)

    assert file.type == loc.File.Type.UnknownFile
    assert file.contents == []

    file.contents = utils.text_to_list(utils.text_file_contents(file_path))

    assert file.type == loc.File.Type.UnknownFile
    assert_issues(file, expected_unknown_file_issues)


def test_cellml_virtual_file():
    file_path = utils.resource_path("cellml_2.cellml")
    file = loc.File(file_path, False)

    assert file.type == loc.File.Type.UnknownFile
    assert file.contents == []

    file.contents = utils.text_to_list(utils.text_file_contents(file_path))

    assert file.type == loc.File.Type.CellmlFile


def test_sedml_virtual_file():
    file_path = utils.resource_path("cellml_2.sedml")
    file = loc.File(file_path, False)

    assert file.type == loc.File.Type.UnknownFile
    assert file.contents == []

    file.contents = utils.text_to_list(utils.text_file_contents(file_path))

    assert file.type == loc.File.Type.SedmlFile


def test_combine_virtual_archive():
    file_path = utils.resource_path("cellml_2.omex")
    file = loc.File(file_path, False)

    assert file.type == loc.File.Type.UnknownFile
    assert file.contents == []

    file.contents = utils.binary_to_list(utils.binary_file_contents(file_path))

    assert file.type == loc.File.Type.CombineArchive
