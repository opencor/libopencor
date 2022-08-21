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
CELLML_2_LOCAL_FILE = "cellml_2.cellml"
# ---GRY--- TO BE ENABLED WHEN WE SUPPORT SED-ML FILES AND COMBINE ARCHIVES.
# SEDML_2_LOCAL_FILE = "cellml_2.sedml"
# COMBINE_2_LOCAL_ARCHIVE = "cellml_2.omex"

NON_RETRIEVABLE_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/non_retrievable_file.txt"
CELLML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"
# ---GRY--- TO BE ENABLED WHEN WE SUPPORT SED-ML FILES AND COMBINE ARCHIVES.
# SEDML_2_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml"
# COMBINE_2_REMOTE_ARCHIVE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex"


def test_instantiate_non_retrievable_local_file():
    f = File(utils.resource_path(NON_RETRIEVABLE_LOCAL_FILE))

    assert f.instantiate() == File.Status.NonRetrievableFile


def test_instantiate_cellml_2_local_file():
    f = File(utils.resource_path(CELLML_2_LOCAL_FILE))

    assert f.instantiate() == File.Status.Ok


# ---GRY--- TO BE ENABLED WHEN WE SUPPORT SED-ML FILES AND COMBINE ARCHIVES.
# def test_instantiate_sedml_2_local_file():
#     f = File(utils.resource_path(SEDML_2_LOCAL_FILE))

#     assert f.instantiate() == File.Status.Ok


# ---GRY--- TO BE ENABLED WHEN WE SUPPORT SED-ML FILES AND COMBINE ARCHIVES.
# def test_instantiate_combine_2_local_file():
#     f = File(utils.resource_path(COMBINE_2_LOCAL_ARCHIVE))

#     assert f.instantiate() == File.Status.Ok


def test_instantiate_non_retrievable_remote_file():
    f = File(NON_RETRIEVABLE_REMOTE_FILE)

    assert f.instantiate() == File.Status.NonRetrievableFile


def test_instantiate_cellml_2_remote_file():
    f = File(CELLML_2_REMOTE_FILE)

    assert f.instantiate() == File.Status.Ok


# ---GRY--- TO BE ENABLED WHEN WE SUPPORT SED-ML FILES AND COMBINE ARCHIVES.
# def test_instantiate_sedml_2_remote_file():
#     f = File(SEDML_2_REMOTE_FILE)

#     assert f.instantiate() == File.Status.Ok


# ---GRY--- TO BE ENABLED WHEN WE SUPPORT SED-ML FILES AND COMBINE ARCHIVES.
# def test_instantiate_combine_2_remote_file():
#     f = File(COMBINE_2_REMOTE_ARCHIVE)

#     assert f.instantiate() == File.Status.Ok
