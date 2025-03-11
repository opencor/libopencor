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


import libopencor as oc
import utils


def test_empty_file():
    file = oc.File(utils.LocalFile)

    file.contents = utils.string_to_list(utils.NoContents)

    assert file.type == oc.File.Type.UnknownFile


def test_file_with_null_character():
    file = oc.File(utils.LocalFile)

    file.contents = utils.string_to_list("\0")

    assert file.type == oc.File.Type.UnknownFile


def test_sedml_file_with_no_parent():
    file = oc.File(utils.Sedml2File)

    file.contents = utils.string_to_list(utils.SomeSedmlContents)


def test_irretrievable_remote_file():
    oc.File(utils.IrretrievableRemoteFile)


def test_same_local_file():
    file1 = oc.File(utils.LocalFile)
    file2 = oc.File(utils.LocalFile)

    assert file1.__subclasshook__ == file2.__subclasshook__


def test_same_remote_file():
    file1 = oc.File(utils.RemoteFile)
    file2 = oc.File(utils.RemoteFile)

    assert file1.__subclasshook__ == file2.__subclasshook__


def test_do_not_retrieve_contents():
    file = oc.File(utils.RemoteFile, False)

    assert file.type == oc.File.Type.UnknownFile
    assert file.contents == []


def test_unmanage_file_with_children():
    file = oc.File(utils.resource_path(utils.Combine2Archive))
    file_manager = oc.FileManager.instance()

    assert file_manager.file_count == 3

    file_manager.unmanage(file)

    assert file_manager.file_count == 0
