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


def test_empty_file():
    file = File(utils.LocalFile)

    file.contents = utils.string_to_list(utils.NoContents)

    assert file.type == File.Type.UnknownFile


def test_http_remote_file():
    File(utils.HttpRemoteFile)


def test_sedml_file_with_no_parent():
    file = File(utils.Sedml2File)

    file.contents = utils.string_to_list(utils.SomeSedmlContents)


def test_irretrievable_remote_file():
    File(utils.IrretrievableRemoteFile)


def test_same_local_file():
    file1 = File(utils.LocalFile)
    file2 = File(utils.LocalFile)

    assert file1.__subclasshook__ == file2.__subclasshook__


def test_same_remote_file():
    file1 = File(utils.RemoteFile)
    file2 = File(utils.RemoteFile)

    assert file1.__subclasshook__ == file2.__subclasshook__
