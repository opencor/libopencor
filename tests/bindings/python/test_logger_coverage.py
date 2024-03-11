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


def test_has_errors():
    file = File(utils.resource_path(utils.CELLML_2_FILE))

    assert not file.has_errors


def test_errors():
    file = File(utils.resource_path(utils.CELLML_2_FILE))

    assert len(file.errors) == 0


def test_has_warnings():
    file = File(utils.resource_path(utils.CELLML_2_FILE))

    assert not file.has_warnings


def test_warnings():
    file = File(utils.resource_path(utils.CELLML_2_FILE))

    assert len(file.warnings) == 0


def test_has_messages():
    file = File(utils.resource_path(utils.CELLML_2_FILE))

    assert not file.has_messages


def test_messages():
    file = File(utils.resource_path(utils.CELLML_2_FILE))

    assert len(file.messages) == 0
