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


NON_RETRIEVABLE_LOCAL_FILE = "non_retrievable_file.txt"
UNKNOWN_LOCAL_FILE = "unknown_file.txt"
SBML_LOCAL_FILE = "sbml.sbml"
ERROR_SEDML_LOCAL_FILE = "error.sedml"
# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# CELLML_1_X_LOCAL_FILE = "cellml_1_x.cellml"
# SEDML_1_X_LOCAL_FILE = "cellml_1_x.sedml"
# COMBINE_1_X_LOCAL_ARCHIVE = "cellml_1_x.omex"
CELLML_2_LOCAL_FILE = "cellml_2.cellml"
SEDML_2_LOCAL_FILE = "cellml_2.sedml"
COMBINE_2_LOCAL_ARCHIVE = "cellml_2.omex"

NON_RETRIEVABLE_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/non_retrievable_file.txt"
UNKNOWN_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt"
SBML_REMOTE_FILE = (
    "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/sbml.sbml"
)
ERROR_SEDML_REMOTE_FILE = (
    "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/error.sedml"
)
# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# CELLML_1_X_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_1_x.cellml"
# SEDML_1_X_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_1_x.sedml"
# COMBINE_1_X_REMOTE_ARCHIVE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_1_x.omex"
CELLML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"
SEDML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml"
COMBINE_2_REMOTE_ARCHIVE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex"


def test_resolve_non_retrievable_local_file():
    f = File(utils.resource_path(NON_RETRIEVABLE_LOCAL_FILE))

    assert f.resolve() == File.Status.NonRetrievableFile


def test_resolve_unknown_local_file():
    f = File(utils.resource_path(UNKNOWN_LOCAL_FILE))

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.UnknownFile


def test_resolve_sbml_local_file():
    f = File(utils.resource_path(SBML_LOCAL_FILE))

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.UnknownFile


def test_resolve_error_sedml_local_file():
    f = File(utils.resource_path(ERROR_SEDML_LOCAL_FILE))

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.SedmlFile


# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# def test_resolve_cellml_1_x_local_file():
#     f = File(utils.resource_path(CELLML_1_X_LOCAL_FILE))

#     assert f.resolve() == File.Status.Ok
#     assert f.type == File.Type.CellmlFile


# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# def test_resolve_sedml_1_x_local_file():
#     f = File(utils.resource_path(SEDML_1_X_LOCAL_FILE))

#     assert f.resolve() == File.Status.Ok
#     assert f.type == File.Type.SedmlFile


# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# def test_resolve_combine_1_x_local_archive():
#     f = File(utils.resource_path(COMBINE_1_X_LOCAL_ARCHIVE))

#     assert f.resolve() == File.Status.Ok
#     assert f.type == File.Type.CombineArchive


def test_resolve_cellml_2_local_file():
    f = File(utils.resource_path(CELLML_2_LOCAL_FILE))

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.CellmlFile


def test_resolve_sedml_2_local_file():
    f = File(utils.resource_path(SEDML_2_LOCAL_FILE))

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.SedmlFile


def test_resolve_combine_2_local_archive():
    f = File(utils.resource_path(COMBINE_2_LOCAL_ARCHIVE))

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.CombineArchive


def test_resolve_non_retrievable_remote_file():
    f = File(NON_RETRIEVABLE_REMOTE_FILE)

    assert f.resolve() == File.Status.NonRetrievableFile


def test_resolve_unknown_remote_file():
    f = File(UNKNOWN_REMOTE_FILE)

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.UnknownFile


def test_resolve_sbml_remote_file():
    f = File(SBML_REMOTE_FILE)

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.UnknownFile


def test_resolve_error_sedml_remote_file():
    f = File(ERROR_SEDML_REMOTE_FILE)

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.SedmlFile


# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# def test_resolve_cellml_1_x_remote_file():
#     f = File(CELLML_1_X_REMOTE_FILE)

#     assert f.resolve() == File.Status.Ok
#     assert f.type == File.Type.CellmlFile


# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# def test_resolve_sedml_1_x_remote_file():
#     f = File(SEDML_1_X_REMOTE_FILE)

#     assert f.resolve() == File.Status.Ok
#     assert f.type == File.Type.SedmlFile


# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.0/1.1.
# def test_resolve_combine_1_x_remote_archive():
#     f = File(COMBINE_1_X_REMOTE_ARCHIVE)

#     assert f.resolve() == File.Status.Ok
#     assert f.type == File.Type.CombineArchive


def test_resolve_cellml_2_remote_file():
    f = File(CELLML_2_REMOTE_FILE)

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.CellmlFile


def test_resolve_sedml_2_remote_file():
    f = File(SEDML_2_REMOTE_FILE)

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.SedmlFile


def test_resolve_combine_2_remote_archive():
    f = File(COMBINE_2_REMOTE_ARCHIVE)

    assert f.resolve() == File.Status.Ok
    assert f.type == File.Type.CombineArchive
