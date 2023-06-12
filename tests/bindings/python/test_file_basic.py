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


WINDOWS_LOCAL_FILE = "C:\\some\\path\\file.txt"
UNIX_LOCAL_FILE = "/some/path/file.txt"


def test_windows_local_file():
    file = File(WINDOWS_LOCAL_FILE)

    assert file.type == File.Type.Unresolved
    assert file.file_name == WINDOWS_LOCAL_FILE
    assert file.url == ""
    assert repr(file) == "Local file: " + WINDOWS_LOCAL_FILE


def test_unix_local_file():
    file = File(UNIX_LOCAL_FILE)

    assert file.type == File.Type.Unresolved
    assert file.file_name == UNIX_LOCAL_FILE
    assert file.url == ""
    assert repr(file) == "Local file: " + UNIX_LOCAL_FILE


def test_url_based_windows_local_file():
    file = File("file:///C:/some/path/file.txt")

    assert file.type == File.Type.Unresolved
    assert file.file_name == WINDOWS_LOCAL_FILE
    assert file.url == ""
    assert repr(file) == "Local file: " + WINDOWS_LOCAL_FILE


def test_url_based_unix_local_file():
    file = File("file:///some/path/file.txt")

    assert file.type == File.Type.Unresolved
    assert file.file_name == UNIX_LOCAL_FILE
    assert file.url == ""
    assert repr(file) == "Local file: " + UNIX_LOCAL_FILE


def test_remote_file():
    file = File(utils.REMOTE_FILE)

    assert file.type == File.Type.Unresolved
    assert file.file_name == ""
    assert file.url == utils.REMOTE_FILE
    assert repr(file) == "Remote file: " + utils.REMOTE_FILE
