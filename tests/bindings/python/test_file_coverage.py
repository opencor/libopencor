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


def test_underconstrained_cellml_file():
    f = File(utils.resource_path("underconstrained.cellml"))

    assert f.resolve() == File.Status.Ok
    assert f.instantiate() == File.Status.NonInstantiableFile


def test_remote_file():
    f = File(utils.REMOTE_FILE)

    assert f.resolve() == File.Status.Ok
    assert f.instantiate() == File.Status.Ok


def test_note_remote_file():
    f = File(utils.NOT_REMOTE_FILE)

    assert f.resolve() == File.Status.NonRetrievableFile
    assert f.instantiate() == File.Status.NonRetrievableFile
