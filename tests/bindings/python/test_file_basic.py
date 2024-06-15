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


from libopencor import File, FileManager, Issue
import platform
import utils
from utils import assert_issues


expected_non_existing_file_issues = [
    [Issue.Type.Error, "The file does not exist."],
]
expected_non_downloadable_file_issues = [
    [Issue.Type.Error, "The file could not be downloaded."],
]
expected_unknown_file_issues = [
    [
        Issue.Type.Error,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
    ],
]


def test_local_file():
    file = File(utils.LocalFile)

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.LocalFile
    assert file.url == ""
    assert file.path == utils.LocalFile
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_relative_local_file():
    if platform.system() == "Windows":
        file = File("some\\.\\relative\\..\\..\\path\\.\\..\\dir\\file.txt")
    else:
        file = File("some/./relative/../../path/./../dir/file.txt")

    assert file.type == File.Type.IrretrievableFile

    if platform.system() == "Windows":
        assert file.file_name == "dir\\file.txt"
    else:
        assert file.file_name == "dir/file.txt"

    assert file.url == ""

    if platform.system() == "Windows":
        assert file.path == "dir\\file.txt"
    else:
        assert file.path == "dir/file.txt"

    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_url_based_local_file():
    if platform.system() == "Windows":
        file = File("file:///P:/some/path/file.txt")
    else:
        file = File("file:///some/path/file.txt")

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.LocalFile
    assert file.url == ""
    assert file.path == utils.LocalFile
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_remote_file():
    file = File(utils.RemoteFile)

    assert file.type == File.Type.CellmlFile
    assert file.file_name != ""
    assert file.url == utils.RemoteFile
    assert file.path == utils.RemoteFile
    assert file.contents != []


def test_local_virtual_file():
    file = File(utils.LocalFile)

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.LocalFile
    assert file.url == ""
    assert file.path == utils.LocalFile
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)

    some_unknown_contents_list = utils.string_to_list(utils.SomeUnknownContents)

    file.contents = some_unknown_contents_list

    assert file.type == File.Type.UnknownFile
    assert file.contents == some_unknown_contents_list
    assert_issues(file, expected_unknown_file_issues)


def test_remote_virtual_file():
    file = File(utils.IrretrievableRemoteFile)

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == ""
    assert file.url == utils.IrretrievableRemoteFile
    assert file.path == utils.IrretrievableRemoteFile
    assert file.contents == []
    assert_issues(file, expected_non_downloadable_file_issues)

    some_unknown_contents_list = utils.string_to_list(utils.SomeUnknownContents)

    file.contents = some_unknown_contents_list

    assert file.type == File.Type.UnknownFile
    assert file.contents == some_unknown_contents_list
    assert_issues(file, expected_unknown_file_issues)


def test_file_manager():
    file_manager = FileManager.instance()

    assert file_manager.has_files() == False
    assert len(file_manager.files()) == 0
    assert file_manager.file(utils.LocalFile) == None

    local_file = File(utils.LocalFile)
    same_file_manager = FileManager.instance()

    assert same_file_manager.has_files() == True
    assert len(file_manager.files()) == 1
    assert same_file_manager.file(utils.LocalFile) == local_file

    remote_file = File(utils.RemoteFile)

    assert file_manager.has_files() == True
    assert len(file_manager.files()) == 2
    assert file_manager.file(utils.RemoteFile) == remote_file
