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

NON_RETRIEVABLE_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/non_retrievable_file.txt"


def test_instantiate_non_retrievable_local_file():
    f = File(utils.resource_path(NON_RETRIEVABLE_LOCAL_FILE))

    assert f.instantiate() == File.Status.NonRetrievableFile


def test_instantiate_non_retrievable_remote_file():
    f = File(NON_RETRIEVABLE_REMOTE_FILE)

    assert f.instantiate() == File.Status.NonRetrievableFile
