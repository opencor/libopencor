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


def test_instantiate_non_retrievable_local_file():
    f = File(utils.resource_path(NON_RETRIEVABLE_FILE))

    assert f.instantiate() == File.Status.NonRetrievableFile


def test_instantiate_unknown_local_file():
    f = File(utils.resource_path(UNKNOWN_FILE))

    assert f.instantiate() == File.Status.NonInstantiableFile


def test_instantiate_sbml_local_file():
    f = File(utils.resource_path(SBML_FILE))

    assert f.instantiate() == File.Status.NonInstantiableFile


def test_instantiate_error_sedml_local_file():
    f = File(utils.resource_path(ERROR_SEDML_FILE))

    assert f.instantiate() == File.Status.NonInstantiableFile


# ---GRY--- TO BE ENABLED WHEN libCellML SUPPORTS CellML 1.X.
# def test_instantiate_cellml_1_x_local_file():
#     f = File(utils.resource_path(CELLML_1_X_FILE))

#     assert f.instantiate() == File.Status.Ok


def test_instantiate_sedml_1_x_local_file():
    f = File(utils.resource_path(SEDML_1_X_FILE))

    assert f.instantiate() == File.Status.NonInstantiableFile


def test_instantiate_combine_1_x_local_archive():
    f = File(utils.resource_path(COMBINE_1_X_ARCHIVE))

    assert f.instantiate() == File.Status.NonInstantiableFile


def test_instantiate_cellml_2_local_file():
    f = File(utils.resource_path(CELLML_2_FILE))

    assert f.instantiate() == File.Status.Ok


def test_instantiate_sedml_2_local_file():
    f = File(utils.resource_path(SEDML_2_FILE))

    assert f.instantiate() == File.Status.NonInstantiableFile


def test_instantiate_combine_2_local_file():
    f = File(utils.resource_path(COMBINE_2_ARCHIVE))

    assert f.instantiate() == File.Status.NonInstantiableFile
