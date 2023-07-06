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


from libopencor import File, Issue
import utils
from utils import assert_issues


expected_non_existing_file_issues = [
    [Issue.Type.Error, "The file does not exist."],
]
expected_unknown_file_issues = [
    [
        Issue.Type.Error,
        "The file is not a CellML file, a SED-ML file, or a COMBINE archive.",
    ],
]


def test_windows_local_file():
    file = File(utils.WINDOWS_LOCAL_FILE)

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.WINDOWS_LOCAL_FILE
    assert file.url == ""
    assert file.contents == []
    assert_issues(expected_non_existing_file_issues, file)


def test_unix_local_file():
    file = File(utils.UNIX_LOCAL_FILE)

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.UNIX_LOCAL_FILE
    assert file.url == ""
    assert file.contents == []
    assert_issues(expected_non_existing_file_issues, file)


def test_url_based_windows_local_file():
    file = File("file:///C:/some/path/file.txt")

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.WINDOWS_LOCAL_FILE
    assert file.url == ""
    assert file.contents == []
    assert_issues(expected_non_existing_file_issues, file)


def test_url_based_unix_local_file():
    file = File("file:///some/path/file.txt")

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.UNIX_LOCAL_FILE
    assert file.url == ""
    assert file.contents == []
    assert_issues(expected_non_existing_file_issues, file)


def test_remote_file():
    file = File(utils.REMOTE_FILE)

    assert file.type == File.Type.CellmlFile
    assert file.file_name != ""
    assert file.url == utils.REMOTE_FILE
    assert file.contents != []


def test_local_virtual_file():
    some_unknown_contents_list = utils.string_to_list(utils.SOME_UNKNOWN_CONTENTS)
    file = File(utils.UNIX_LOCAL_FILE, some_unknown_contents_list)

    assert file.type == File.Type.UnknownFile
    assert file.file_name == utils.UNIX_LOCAL_FILE
    assert file.url == ""
    assert file.contents == some_unknown_contents_list
    assert_issues(expected_unknown_file_issues, file)


def test_remote_virtual_file():
    some_unknown_contents_list = utils.string_to_list(utils.SOME_UNKNOWN_CONTENTS)
    file = File(utils.REMOTE_FILE, some_unknown_contents_list)

    assert file.type == File.Type.UnknownFile
    assert file.file_name == ""
    assert file.url == utils.REMOTE_FILE
    assert file.contents == some_unknown_contents_list
    assert_issues(expected_unknown_file_issues, file)
