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


def test_windows_local_file():
    file = File(utils.WINDOWS_LOCAL_FILE)

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.WINDOWS_LOCAL_FILE
    assert file.url == ""
    assert file.contents == None
    assert file.size == 0


def test_unix_local_file():
    file = File(utils.UNIX_LOCAL_FILE)

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.UNIX_LOCAL_FILE
    assert file.url == ""
    assert file.contents == None
    assert file.size == 0


def test_url_based_windows_local_file():
    file = File("file:///C:/some/path/file.txt")

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.WINDOWS_LOCAL_FILE
    assert file.url == ""
    assert file.contents == None
    assert file.size == 0


def test_url_based_unix_local_file():
    file = File("file:///some/path/file.txt")

    assert file.type == File.Type.IrretrievableFile
    assert file.file_name == utils.UNIX_LOCAL_FILE
    assert file.url == ""
    assert file.contents == None
    assert file.size == 0


def test_remote_file():
    file = File(utils.REMOTE_FILE)

    assert file.type == File.Type.CellmlFile
    assert file.file_name != ""
    assert file.url == utils.REMOTE_FILE
    assert file.contents != None
    assert file.size != 0


def test_local_virtual_file():
    file = File(
        utils.UNIX_LOCAL_FILE,
        utils.SOME_UNKNOWN_CONTENTS,
        len(utils.SOME_UNKNOWN_CONTENTS),
    )

    assert file.type == File.Type.UnknownFile
    assert file.file_name == utils.UNIX_LOCAL_FILE
    assert file.url == ""
    assert file.contents == utils.SOME_UNKNOWN_CONTENTS
    assert file.size == len(utils.SOME_UNKNOWN_CONTENTS)


def test_remote_virtual_file():
    file = File(
        utils.REMOTE_FILE, utils.SOME_UNKNOWN_CONTENTS, len(utils.SOME_UNKNOWN_CONTENTS)
    )

    assert file.type == File.Type.UnknownFile
    assert file.file_name == ""
    assert file.url == utils.REMOTE_FILE
    assert file.contents == utils.SOME_UNKNOWN_CONTENTS
    assert file.size == len(utils.SOME_UNKNOWN_CONTENTS)
