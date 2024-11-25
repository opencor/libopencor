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
import os
import platform
import utils
from utils import assert_issues


expected_no_issues = []
expected_non_existing_file_issues = [
    [oc.Issue.Type.Error, "The file does not exist."],
]
expected_non_downloadable_file_issues = [
    [oc.Issue.Type.Error, "The file could not be downloaded."],
]
expected_unknown_file_issues = [
    [
        oc.Issue.Type.Error,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
    ],
]


def test_local_file():
    file = oc.File(utils.LocalFile)

    assert file.type == oc.File.Type.IrretrievableFile
    assert file.file_name == utils.LocalFile
    assert file.url == ""
    assert file.path == utils.LocalFile
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_relative_local_file():
    if platform.system() == "Windows":
        file = oc.File("some\\.\\relative\\..\\..\\path\\.\\..\\dir\\file.txt")
    else:
        file = oc.File("some/./relative/../../path/./../dir/file.txt")

    assert file.type == oc.File.Type.IrretrievableFile

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


def test_existing_relative_local_file():
    orig_dir = os.getcwd()

    os.chdir(utils.resource_path())

    file = oc.File(utils.Cellml2File)

    assert file.contents != []
    assert_issues(file, expected_no_issues)

    os.chdir(orig_dir)


def test_url_based_local_file():
    if platform.system() == "Windows":
        file = oc.File("file:///P:/some/path/file.txt")
    else:
        file = oc.File("file:///some/path/file.txt")

    assert file.type == oc.File.Type.IrretrievableFile
    assert file.file_name == utils.LocalFile
    assert file.url == ""
    assert file.path == utils.LocalFile
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_remote_file():
    file = oc.File(utils.RemoteFile)

    assert file.type == oc.File.Type.CellmlFile
    assert file.file_name != ""
    assert file.url == utils.RemoteFile
    assert file.path == utils.RemoteFile
    assert file.contents != []


def test_local_virtual_file():
    file = oc.File(utils.LocalFile)

    assert file.type == oc.File.Type.IrretrievableFile
    assert file.file_name == utils.LocalFile
    assert file.url == ""
    assert file.path == utils.LocalFile
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)

    some_unknown_contents_list = utils.string_to_list(utils.SomeUnknownContents)

    file.contents = some_unknown_contents_list

    assert file.type == oc.File.Type.UnknownFile
    assert file.contents == some_unknown_contents_list
    assert_issues(file, expected_unknown_file_issues)


def test_remote_virtual_file():
    file = oc.File(utils.IrretrievableRemoteFile)

    assert file.type == oc.File.Type.IrretrievableFile
    assert file.file_name == ""
    assert file.url == utils.IrretrievableRemoteFile
    assert file.path == utils.IrretrievableRemoteFile
    assert file.contents == []
    assert_issues(file, expected_non_downloadable_file_issues)

    some_unknown_contents_list = utils.string_to_list(utils.SomeUnknownContents)

    file.contents = some_unknown_contents_list

    assert file.type == oc.File.Type.UnknownFile
    assert file.contents == some_unknown_contents_list
    assert_issues(file, expected_unknown_file_issues)


def test_file_manager():
    file_manager = oc.FileManager.instance()

    assert file_manager.has_files == False
    assert file_manager.file_count == 0
    assert len(file_manager.files) == 0
    assert file_manager.file(0) == None
    assert file_manager.file(utils.LocalFile) == None

    local_file = oc.File(utils.LocalFile)
    same_file_manager = oc.FileManager.instance()

    assert same_file_manager.has_files == True
    assert same_file_manager.file_count == 1
    assert len(file_manager.files) == 1
    assert file_manager.file(0) == local_file
    assert same_file_manager.file(utils.LocalFile) == local_file

    remote_file = oc.File(utils.RemoteFile)

    assert file_manager.has_files == True
    assert file_manager.file_count == 2
    assert len(file_manager.files) == 2
    assert file_manager.file(1) == remote_file
    assert file_manager.file(utils.RemoteFile) == remote_file

    same_file_manager.unmanage(local_file)

    assert same_file_manager.has_files == True
    assert same_file_manager.file_count == 1
    assert len(file_manager.files) == 1
    assert file_manager.file(1) == None
    assert same_file_manager.file(utils.LocalFile) == None

    same_file_manager.manage(local_file)

    assert same_file_manager.has_files == True
    assert same_file_manager.file_count == 2
    assert len(file_manager.files) == 2
    assert file_manager.file(1) == local_file
    assert same_file_manager.file(utils.LocalFile) == local_file

    file_manager.reset()

    assert file_manager.has_files == False
    assert file_manager.file_count == 0
    assert len(file_manager.files) == 0
    assert file_manager.file(0) == None
    assert file_manager.file(1) == None
    assert file_manager.file(utils.RemoteFile) == None
    assert file_manager.file(utils.UnknownFile) == None
