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


import libopencor as loc
import os
import platform
import utils
from utils import assert_issues

expected_no_issues = []
expected_non_existing_file_issues = [
    [loc.Issue.Type.Error, "The file does not exist."],
]
expected_unknown_file_issues = [
    [
        loc.Issue.Type.Error,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
    ],
]


def test_local_file():
    file_path = utils.resource_path("file.txt")
    file = loc.File(file_path)

    assert file.type == loc.File.Type.IrretrievableFile
    assert file.file_name == file_path
    assert file.url == ""
    assert file.path == file_path
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_existing_relative_local_file():
    orig_dir = os.getcwd()

    os.chdir(utils.resource_path())

    file = loc.File(utils.resource_path("cellml_2.cellml"))

    assert file.contents != []
    assert_issues(file, expected_no_issues)

    os.chdir(orig_dir)


def test_non_existing_relative_local_file():
    if platform.system() == "Windows":
        file = loc.File("some\\.\\relative\\..\\..\\path\\.\\..\\dir\\file.txt")
    else:
        file = loc.File("some/./relative/../../path/./../dir/file.txt")

    assert file.type == loc.File.Type.IrretrievableFile

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
    file_path = utils.resource_path("file.txt")

    if platform.system() == "Windows":
        file = loc.File("file:///" + utils.forward_slash_path(file_path))
    else:
        file = loc.File("file://" + file_path)

    assert file.type == loc.File.Type.IrretrievableFile
    assert file.file_name == file_path
    assert file.url == ""
    assert file.path == file_path
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_remote_file():
    file = loc.File(utils.RemoteFile)

    assert file.type == loc.File.Type.CellmlFile
    assert file.file_name != ""
    assert file.url == utils.RemoteFile
    assert file.path == utils.RemoteFile
    assert file.contents != []


def test_encoded_remote_file():
    file = loc.File(
        "https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO%20BG%20example%203.1.cellml"
    )

    assert file.type == loc.File.Type.CellmlFile
    assert file.file_name != ""
    assert (
        file.url
        == "https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml"
    )
    assert (
        file.path
        == "https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml"
    )
    assert file.contents != []


def test_local_virtual_file():
    file_path = utils.resource_path("unknown_file.txt")
    file = loc.File(file_path, False)

    assert file.type == loc.File.Type.UnknownFile
    assert file.file_name == file_path
    assert file.url == ""
    assert file.path == file_path
    assert file.contents == []
    assert_issues(file, expected_no_issues)

    some_unknown_contents_list = utils.text_to_list(utils.text_file_contents(file_path))

    file.contents = some_unknown_contents_list

    assert file.type == loc.File.Type.UnknownFile
    assert file.contents == some_unknown_contents_list
    assert_issues(file, expected_unknown_file_issues)


def test_remote_virtual_file():
    file = loc.File(
        "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt",
        False,
    )

    assert file.type == loc.File.Type.UnknownFile

    if platform.system() == "Windows":
        assert file.file_name == "\\some\\path\\file"
    else:
        assert file.file_name == "/some/path/file"

    assert (
        file.url
        == "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt"
    )
    assert (
        file.path
        == "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt"
    )
    assert file.contents == []
    assert_issues(file, expected_no_issues)

    some_unknown_contents_list = utils.text_to_list(
        utils.text_file_contents(utils.resource_path("unknown_file.txt"))
    )

    file.contents = some_unknown_contents_list

    assert file.type == loc.File.Type.UnknownFile
    assert file.contents == some_unknown_contents_list
    assert_issues(file, expected_unknown_file_issues)


def test_file_manager():
    file_manager = loc.FileManager.instance()
    file_path = utils.resource_path("file.txt")

    assert not file_manager.has_files
    assert file_manager.file_count == 0
    assert len(file_manager) == 0
    assert file_manager.file(0) is None
    assert file_manager.file(file_path) is None

    local_file = loc.File(file_path)
    same_file_manager = loc.FileManager.instance()

    assert same_file_manager.has_files
    assert same_file_manager.file_count == 1
    assert len(file_manager) == 1
    assert list(file_manager) == [local_file]
    assert file_manager.file(0) == local_file
    assert same_file_manager.file(file_path) == local_file

    remote_file = loc.File(utils.RemoteFile)

    assert file_manager.has_files
    assert file_manager.file_count == 2
    assert len(file_manager) == 2
    assert file_manager.file(1) == remote_file
    assert file_manager.file(utils.RemoteFile) == remote_file

    same_file_manager.unmanage(local_file)

    assert same_file_manager.has_files
    assert same_file_manager.file_count == 1
    assert len(file_manager) == 1
    assert file_manager.file(1) is None
    assert same_file_manager.file(file_path) is None

    same_file_manager.manage(local_file)

    assert same_file_manager.has_files
    assert same_file_manager.file_count == 2
    assert len(file_manager) == 2
    assert file_manager.file(1) == local_file
    assert same_file_manager.file(file_path) == local_file

    file_manager.reset()

    assert not file_manager.has_files
    assert file_manager.file_count == 0
    assert len(file_manager) == 0
    assert file_manager.file(0) is None
    assert file_manager.file(1) is None
    assert file_manager.file(utils.RemoteFile) is None
    assert file_manager.file(utils.resource_path("unknown_file.txt")) is None
