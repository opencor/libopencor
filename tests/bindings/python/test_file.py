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


WINDOWS_FILE = "C:\\some\\path\\file.txt"
UNIX_FILE = "/some/path/file.txt"
URL_BASED_WINDOWS_FILE = "file:///C:/some/path/file.txt"
URL_BASED_UNIX_FILE = "file:///some/path/file.txt"
REMOTE_FILE = "https://models.physiomeproject.org/workspace/noble_1962/rawfile/c70f8962407db00673f1fdcac9f35a2593781c17/noble_1962.cellml"


def test_windows_file():
    f = File(WINDOWS_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == WINDOWS_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + WINDOWS_FILE


def test_unix_file():
    f = File(UNIX_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == UNIX_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + UNIX_FILE


def test_url_based_windows_file():
    f = File(URL_BASED_WINDOWS_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == WINDOWS_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + WINDOWS_FILE


def test_url_based_unix_file():
    f = File(URL_BASED_UNIX_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == UNIX_FILE
    assert f.url == ""
    assert repr(f) == "Local file: " + UNIX_FILE


def test_remote_file():
    f = File(REMOTE_FILE)

    assert f.type == File.Type.Undefined
    assert f.file_name == ""
    assert f.url == REMOTE_FILE
    assert repr(f) == "Remote file: " + REMOTE_FILE
