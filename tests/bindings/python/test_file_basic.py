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
import pytest
import shutil
import tempfile
import threading
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
    # An existing relative file name must stay relative, i.e. it must not be resolved against the current working
    # directory.

    orig_dir = os.getcwd()

    os.chdir(utils.resource_path())

    file = loc.File("cellml_2.cellml")

    assert file.file_name == "cellml_2.cellml"
    assert file.path == "cellml_2.cellml"
    assert file.contents != []
    assert_issues(file, expected_no_issues)

    os.chdir(orig_dir)


def test_non_existing_relative_local_file():
    if platform.system() == "Windows":
        file = loc.File("some\\.\\relative\\..\\..\\path\\.\\..\\dir\\file.txt")
    else:
        file = loc.File("some/relative/../../path/../dir/file.txt")

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


def test_non_existing_relative_local_file_with_leading_parent_directories():
    file = loc.File("../models/./lorenz.cellml")

    assert file.type == loc.File.Type.IrretrievableFile

    if platform.system() == "Windows":
        assert file.file_name == "..\\models\\lorenz.cellml"
    else:
        assert file.file_name == "../models/lorenz.cellml"

    assert file.url == ""

    if platform.system() == "Windows":
        assert file.path == "..\\models\\lorenz.cellml"
    else:
        assert file.path == "../models/lorenz.cellml"

    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_too_long_local_file_name():
    # A file name that is too long for the file system must not result in an exception being thrown.

    TOO_LONG_NAME_LENGTH = 5000

    file = loc.File("/" + "a" * TOO_LONG_NAME_LENGTH)

    assert file.type == loc.File.Type.IrretrievableFile
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)


def test_local_file_with_null_character():
    # A file name with a null character cannot exist and must never be truncated at that null character (e.g.,
    # "/some/path/file.txt\0.cellml" must not be considered to be "/some/path/file.txt").

    file_path = utils.resource_path("cellml_2.cellml")
    file_name = file_path + "\0.txt"
    file = loc.File(file_name)

    assert file.type == loc.File.Type.IrretrievableFile
    assert file.file_name == file_name
    assert file.url == ""
    assert file.path == file_name
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)
    assert loc.File(file_path) != file


def test_local_file_with_non_utf8_character():
    # A file name that is not valid UTF-8 (e.g., "/some/path/\xFF.cellml", which is what "file:///some/path/%FF.cellml"
    # gets percent-decoded to) must not result in an exception being thrown and must be kept as is.

    file_name = utils.resource_path("\udcff.cellml")
    file = loc.File(file_name)

    assert file.type == loc.File.Type.IrretrievableFile
    assert file.file_name.endswith("\udcff.cellml")
    assert file.file_name == file_name
    assert file.url == ""
    assert file.path == file_name
    assert file.contents == []
    assert_issues(file, expected_non_existing_file_issues)

    if platform.system() == "Windows":
        assert (
            loc.File(
                "file:///" + utils.forward_slash_path(utils.resource_path("%FF.cellml"))
            )
            == file
        )
    else:
        assert loc.File("file://" + utils.resource_path("%FF.cellml")) == file

    # A file name with a character that is outside of the Basic Multilingual Plane (e.g., an emoji) is also kept as is.

    emoji_file_name = utils.resource_path("\U0001f600.cellml")
    emoji_file = loc.File(emoji_file_name)

    assert emoji_file.type == loc.File.Type.IrretrievableFile
    assert emoji_file.file_name.endswith("\U0001f600.cellml")
    assert emoji_file.file_name == emoji_file_name
    assert_issues(emoji_file, expected_non_existing_file_issues)

    if platform.system() == "Windows":
        assert (
            loc.File(
                "file:///"
                + utils.forward_slash_path(utils.resource_path("%F0%9F%98%80.cellml"))
            )
            == emoji_file
        )
    else:
        assert (
            loc.File("file://" + utils.resource_path("%F0%9F%98%80.cellml"))
            == emoji_file
        )


def test_local_directory():
    # A directory is not a file, but it must not result in an exception being thrown.

    file = loc.File(utils.resource_path("api"))

    assert file.type == loc.File.Type.UnknownFile
    assert file.contents == []
    assert_issues(file, expected_no_issues)


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

    # A scheme is case-insensitive.

    if platform.system() == "Windows":
        assert loc.File("FILE:///" + utils.forward_slash_path(file_path)) == file
    else:
        assert loc.File("FILE://" + file_path) == file

    # A "file" URL with the "localhost" host or without an authority also refers to a local file while a "file" URL
    # with another host refers to a remote file.

    if platform.system() == "Windows":
        url_path = "/" + utils.forward_slash_path(file_path)
    else:
        url_path = file_path

    assert loc.File("file://localhost" + url_path) == file
    assert loc.File("file:" + url_path) == file

    remote_file = loc.File("file://example.com" + url_path, False)

    assert remote_file != file
    assert remote_file.file_name == ""
    assert remote_file.url == "file://example.com" + url_path


def test_local_files_with_percent_signs():
    # A local file name is never decoded since a percent sign is a valid character in a file name.

    dir_path = os.path.join(
        tempfile.gettempdir(), "libopencor_local_files_with_percent_signs"
    )

    with open(utils.resource_path("cellml_2.cellml"), "rb") as f:
        cellml_contents = f.read()

    not_a_model = b"not a model"

    os.makedirs(dir_path, exist_ok=True)

    with open(os.path.join(dir_path, "a%20model.cellml"), "wb") as f:
        f.write(cellml_contents)

    with open(os.path.join(dir_path, "b%20c.cellml"), "wb") as f:
        f.write(cellml_contents)

    with open(os.path.join(dir_path, "b c.cellml"), "wb") as f:
        f.write(not_a_model)

    canonical_dir_path = os.path.realpath(dir_path)
    a = loc.File(os.path.join(dir_path, "a%20model.cellml"))
    b = loc.File(os.path.join(dir_path, "b%20c.cellml"))
    bc = loc.File(os.path.join(dir_path, "b c.cellml"))

    assert a.type == loc.File.Type.CellmlFile
    assert a.file_name == os.path.join(canonical_dir_path, "a%20model.cellml")
    assert_issues(a, expected_no_issues)
    assert b.type == loc.File.Type.CellmlFile
    assert b.file_name == os.path.join(canonical_dir_path, "b%20c.cellml")
    assert b.contents == utils.binary_to_list(cellml_contents)
    assert_issues(b, expected_no_issues)
    assert bc != b
    assert bc.file_name == os.path.join(canonical_dir_path, "b c.cellml")
    assert bc.contents == utils.binary_to_list(not_a_model)

    # The path of a "file" URL is, however, decoded.

    if platform.system() == "Windows":
        dir_url = "file:///" + utils.forward_slash_path(canonical_dir_path)
    else:
        dir_url = "file://" + canonical_dir_path

    assert loc.File(dir_url + "/b%2520c.cellml") == b
    assert loc.File(dir_url + "/b%20c.cellml") == bc
    assert loc.FileManager.instance().file(dir_url + "/b%2520c.cellml") == b
    assert loc.FileManager.instance().file(dir_url + "/b%20c.cellml") == bc

    shutil.rmtree(dir_path)


def test_remote_files_with_percent_encoded_characters():
    # Percent-encoded characters are decoded, unless they have a meaning in a URL, in which case the URL would refer to
    # a different file.

    file = loc.File("https://example.com/my%20file.cellml", False)

    assert file.url == "https://example.com/my file.cellml"
    assert loc.File("https://example.com/my file.cellml", False) == file
    assert (
        loc.FileManager.instance().file("https://example.com/my%20file.cellml") == file
    )
    assert loc.FileManager.instance().file("https://example.com/my file.cellml") == file

    for encoded_url, other_url in [
        ("https://example.com/a%2Fb.cellml", "https://example.com/a/b.cellml"),
        ("https://example.com/x/..%2Fy.cellml", "https://example.com/y.cellml"),
        ("https://example.com/a%3Fb.cellml", "https://example.com/a?b.cellml"),
        ("https://example.com/a%23b.cellml", "https://example.com/a#b.cellml"),
        ("https://example.com/a%2520b.cellml", "https://example.com/a%20b.cellml"),
        ("https://example.com/a%5Cb.cellml", "https://example.com/a/b.cellml"),
    ]:
        encoded_file = loc.File(encoded_url, False)
        other_file = loc.File(other_url, False)

        assert encoded_file != other_file
        assert encoded_file.url == encoded_url
        assert loc.File(encoded_file.url, False) == encoded_file

    # A percent sign that doesn't start a percent-encoded character is percent-encoded.

    percent_file = loc.File("https://example.com/100%.cellml", False)

    assert percent_file.url == "https://example.com/100%25.cellml"
    assert loc.File("https://example.com/100%25.cellml", False) == percent_file
    assert loc.File(percent_file.url, False) == percent_file

    # A percent-encoded character that is not valid UTF-8 is also decoded.

    non_utf8_file = loc.File("https://example.com/%FF.cellml", False)

    assert non_utf8_file.url == "https://example.com/\udcff.cellml"
    assert loc.File(non_utf8_file.url, False) == non_utf8_file

    # A percent-encoded character that is outside of the Basic Multilingual Plane (e.g., an emoji) is also decoded.

    emoji_file = loc.File("https://example.com/%F0%9F%98%80.cellml", False)

    assert emoji_file.url == "https://example.com/\U0001f600.cellml"
    assert loc.File(emoji_file.url, False) == emoji_file

    # The representation of a file which URL is not valid UTF-8 has its invalid bytes escaped.
    # Note: this has no C++ or JavaScript equivalent since a file has no representation in C++ or JavaScript.

    assert repr(non_utf8_file) == 'File("https://example.com/\\xff.cellml")'

    # A string with a lone surrogate that doesn't stand for an invalid byte (i.e. other than U+DC80 to U+DCFF) is
    # rejected.
    # Note: this has no C++ equivalent since a std::string cannot contain a lone surrogate.

    with pytest.raises(TypeError):
        loc.File("https://example.com/\ud800.cellml", False)

    with pytest.raises(TypeError):
        loc.File("https://example.com/\udc41.cellml", False)


def test_remote_file():
    file = loc.File(utils.RemoteFile)

    assert file.type == loc.File.Type.CellmlFile
    assert file.file_name != ""
    assert file.url == utils.RemoteFile
    assert file.path == utils.RemoteFile
    assert file.contents != []


def test_remote_file_local_copy():
    # The local copy of a remote file must exist for as long as the remote file exists, and be deleted with it.
    # Note: this has no JavaScript equivalent since our JavaScript bindings never retrieve the contents of a file, so a
    #       remote file is never downloaded.

    file = loc.File(utils.RemoteFile)
    file_name = file.file_name

    assert os.path.exists(file_name)

    del file

    assert not os.path.exists(file_name)


def test_local_file_matching_local_copy_of_remote_file():
    # A local file must never be confused with the local copy of a remote file, and deleting it must not delete that
    # local copy (since it is not a file that we downloaded).
    # Note: this has no JavaScript equivalent since our JavaScript bindings never retrieve the contents of a file, so a
    #       remote file is never downloaded.

    file_manager = loc.FileManager.instance()
    remote_file = loc.File(utils.RemoteFile)
    file_name = remote_file.file_name
    local_file = loc.File(file_name)

    assert local_file != remote_file
    assert local_file.file_name == os.path.realpath(file_name)
    assert local_file.url == ""
    assert local_file.contents == remote_file.contents
    assert file_manager.file_count == 2

    del local_file

    assert os.path.exists(file_name)
    assert file_manager.file(file_name) is None


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


def test_remote_file_with_dot_segments():
    file = loc.File("https://example.com/a/./b/../../c/model.cellml", False)

    assert file.url == "https://example.com/c/model.cellml"
    assert file.path == "https://example.com/c/model.cellml"


def test_remote_file_with_upper_case_scheme():
    # A scheme is case-insensitive, so it gets normalised to lowercase.

    file = loc.File("HTTPS://example.com/model.cellml", False)

    assert file.url == "https://example.com/model.cellml"
    assert file.path == "https://example.com/model.cellml"
    assert loc.File("https://example.com/model.cellml", False) == file
    assert loc.File("Https://example.com/model.cellml", False) == file
    assert loc.FileManager.instance().file("HTTPS://example.com/model.cellml") == file


def test_remote_file_with_upper_case_host():
    # A host is case-insensitive, so it gets normalised to lowercase, unlike a path which is case-sensitive.

    file = loc.File("https://EXAMPLE.com/model.cellml", False)

    assert file.url == "https://example.com/model.cellml"
    assert file.path == "https://example.com/model.cellml"
    assert loc.File("https://example.com/model.cellml", False) == file
    assert loc.File("https://Example.COM/model.cellml", False) == file
    assert loc.FileManager.instance().file("https://EXAMPLE.com/model.cellml") == file
    assert loc.File("https://example.com/MODEL.cellml", False) != file


def test_remote_file_matching_local_file():
    # The host and path of a URL must never be resolved as a local file, even if such a local file exists.

    orig_dir = os.getcwd()
    temp_dir = os.path.join(
        tempfile.gettempdir(), "libopencor_remote_file_matching_local_file"
    )

    os.makedirs(os.path.join(temp_dir, "example.com"), exist_ok=True)
    open(os.path.join(temp_dir, "example.com", "model.cellml"), "w").close()

    os.chdir(temp_dir)

    url = loc.File("https://example.com/model.cellml", False).url

    os.chdir(orig_dir)
    shutil.rmtree(temp_dir)

    assert url == "https://example.com/model.cellml"


def test_concurrent_file_creations():
    # Creating a file must never change the current working directory since it is shared by all the threads of the
    # process. Also, the file manager must cope with files that get looked up while they are being destroyed by another
    # thread.

    FILE_CREATION_COUNT = 1000

    orig_dir = os.getcwd()
    done = threading.Event()
    working_directory_changed = False

    def check():
        nonlocal working_directory_changed

        file_manager = loc.FileManager.instance()

        while not done.is_set():
            if os.getcwd() != orig_dir:
                working_directory_changed = True

            file_manager.file("https://example.com/model.cellml")
            file_manager.files

    def create_files():
        for _ in range(FILE_CREATION_COUNT):
            loc.File("https://example.com/model.cellml", False)
            loc.File("non_existing_dir/../non_existing_file.txt", False)

    checker = threading.Thread(target=check)

    checker.start()

    thread1 = threading.Thread(target=create_files)
    thread2 = threading.Thread(target=create_files)

    thread1.start()
    thread2.start()

    thread1.join()
    thread2.join()

    done.set()

    checker.join()

    assert not working_directory_changed
    assert os.getcwd() == orig_dir


def test_concurrent_file_downloads():
    # Remote files that are downloaded at the same time must each have their own local copy, so that deleting the local
    # copy of one of them doesn't delete the local copy of another one.
    # Note: this has no JavaScript equivalent since our JavaScript bindings never retrieve the contents of a file, so a
    #       remote file is never downloaded.

    THREAD_COUNT = 8

    files = [None] * THREAD_COUNT

    def download_file(index):
        files[index] = loc.File(utils.RemoteFile + "?thread=" + str(index))

    threads = [
        threading.Thread(target=download_file, args=(i,)) for i in range(THREAD_COUNT)
    ]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    file_names = set()

    for file in files:
        assert file.type == loc.File.Type.CellmlFile
        assert file.contents == files[0].contents

        file_names.add(file.file_name)

    assert len(file_names) == THREAD_COUNT

    del file
    files.clear()

    for file_name in file_names:
        assert not os.path.exists(file_name)


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
    assert file.file_name == ""
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


def test_remote_virtual_file_matching_local_file():
    # A remote file that has no local copy has an empty file name, so it must never be confused with a local file that
    # has an empty file name.

    file_manager = loc.FileManager.instance()
    remote_file = loc.File(utils.RemoteFile, False)
    local_file = loc.File("", False)

    assert local_file != remote_file
    assert remote_file.file_name == ""
    assert local_file.file_name == ""
    assert local_file.url == ""
    assert file_manager.file("") == local_file
    assert file_manager.file(utils.RemoteFile) == remote_file


def test_file_manager():
    file_manager = loc.FileManager.instance()
    file_path = utils.resource_path("file.txt")

    assert not file_manager.has_files
    assert file_manager.file_count == 0
    assert len(file_manager.files) == 0
    assert file_manager.file(0) is None
    assert file_manager.file(file_path) is None

    local_file = loc.File(file_path)
    same_file_manager = loc.FileManager.instance()

    assert same_file_manager.has_files
    assert same_file_manager.file_count == 1
    assert len(same_file_manager.files) == 1
    assert file_manager.file(0) == local_file
    assert same_file_manager.file(file_path) == local_file

    # A file manager is sized and iterable.
    # Note: this has no C++ or JavaScript equivalent since a file manager is neither sized nor iterable in C++ or
    #       JavaScript.

    assert len(file_manager) == 1
    assert list(file_manager) == [local_file]

    remote_file = loc.File(utils.RemoteFile)

    assert file_manager.has_files
    assert file_manager.file_count == 2
    assert len(file_manager.files) == 2
    assert file_manager.file(1) == remote_file
    assert file_manager.file(utils.RemoteFile) == remote_file

    same_file_manager.unmanage(local_file)

    assert same_file_manager.has_files
    assert same_file_manager.file_count == 1
    assert len(same_file_manager.files) == 1
    assert file_manager.file(1) is None
    assert same_file_manager.file(file_path) is None

    same_file_manager.manage(local_file)

    assert same_file_manager.has_files
    assert same_file_manager.file_count == 2
    assert len(same_file_manager.files) == 2
    assert file_manager.file(1) == local_file
    assert same_file_manager.file(file_path) == local_file

    file_manager.reset()

    assert not file_manager.has_files
    assert file_manager.file_count == 0
    assert len(file_manager.files) == 0
    assert file_manager.file(0) is None
    assert file_manager.file(1) is None
    assert file_manager.file(utils.RemoteFile) is None
    assert file_manager.file(utils.resource_path("unknown_file.txt")) is None
