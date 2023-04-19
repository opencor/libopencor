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


def test_basic_windows_file():
    f = File(WINDOWS_LOCAL_FILE)

    assert f.type == File.Type.Unresolved
    assert f.file_name == WINDOWS_LOCAL_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + WINDOWS_LOCAL_FILE


def test_basic_unix_file():
    f = File(UNIX_LOCAL_FILE)

    assert f.type == File.Type.Unresolved
    assert f.file_name == UNIX_LOCAL_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + UNIX_LOCAL_FILE


def test_basic_url_based_windows_file():
    f = File("file:///C:/some/path/file.txt")

    assert f.type == File.Type.Unresolved
    assert f.file_name == WINDOWS_LOCAL_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + WINDOWS_LOCAL_FILE


def test_basic_url_based_unix_file():
    f = File("file:///some/path/file.txt")

    assert f.type == File.Type.Unresolved
    assert f.file_name == UNIX_LOCAL_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + UNIX_LOCAL_FILE


def test_basic_remote_file():
    f = File(utils.REMOTE_FILE)

    assert f.type == File.Type.Unresolved
    assert f.file_name == ""
    assert f.url == utils.REMOTE_FILE
    assert repr(f) == "Remote file: " + utils.REMOTE_FILE
