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


from libopencor import File, SedDocument
import utils


def test_issue():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)

    assert sed.issue(0) == None


def test_has_errors():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)

    assert not sed.has_errors


def test_error():
    # Has an error.

    file = File(utils.resource_path(utils.UNKNOWN_FILE))
    sed = SedDocument(file)

    assert sed.error(0) != None

    # Doesn't have an error.

    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)

    assert sed.error(0) == None
