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
import os
import platform
import pytest
import shutil
import tempfile
import utils


def do_test_dataset(number, specific_child_file_names):
    file = File(utils.resource_path("api/file/dataset_" + str(number) + ".omex"))

    assert file.has_child_files
    assert file.child_file_count == len(specific_child_file_names) + 1
    assert len(file.child_file_names) == len(specific_child_file_names) + 1
    assert len(file.child_files) == len(specific_child_file_names) + 1

    index = 0
    simulation_file = file.child_file("simulation.json")

    assert file.child_file(index) is not None
    assert simulation_file is not None

    for specific_child_file_name in specific_child_file_names:
        index += 1

        assert file.child_file(index) is not None
        assert file.child_file(specific_child_file_name) is not None

    assert file.child_file(index + 1) is None
    assert file.child_file(utils.resource_path("unknown_file.txt")) is None

    assert utils.to_string(simulation_file.contents) == utils.text_file_contents(
        utils.resource_path("api/file/dataset_" + str(number) + ".json")
    )


def test_no_child_files():
    file_path = utils.resource_path("unknown_file.txt")
    file = File(file_path)

    assert not file.has_child_files
    assert file.child_file_count == 0
    assert len(file.child_file_names) == 0
    assert len(file.child_files) == 0
    assert file.child_file(0) is None
    assert file.child_file(file_path) is None


def test_dataset_135():
    do_test_dataset(135, ["HumanSAN_Fabbri_Fantini_Wilders_Severi_2017.cellml"])


def test_dataset_157():
    do_test_dataset(
        157,
        [
            "fabbri_et_al_based_composite_SAN_model.cellml",
            "fabbri_et_al_based_composite_SAN_model.sedml",
        ],
    )


def test_remote_virtual_combine_archives():
    # Two remote COMBINE archives that are not downloaded must not share their child files.

    file_135 = File(utils.RemoteBasePath + "/api/file/dataset_135.omex", False)
    file_157 = File(utils.RemoteBasePath + "/api/file/dataset_157.omex", False)

    file_135.contents = utils.binary_to_list(
        utils.binary_file_contents(utils.resource_path("api/file/dataset_135.omex"))
    )
    file_157.contents = utils.binary_to_list(
        utils.binary_file_contents(utils.resource_path("api/file/dataset_157.omex"))
    )

    assert file_135.type == File.Type.CombineArchive
    assert file_157.type == File.Type.CombineArchive

    simulation_file_135 = file_135.child_file("simulation.json")
    simulation_file_157 = file_157.child_file("simulation.json")

    assert simulation_file_135 is not None
    assert simulation_file_157 is not None
    assert simulation_file_135 != simulation_file_157
    assert utils.to_string(simulation_file_135.contents) == utils.text_file_contents(
        utils.resource_path("api/file/dataset_135.json")
    )
    assert utils.to_string(simulation_file_157.contents) == utils.text_file_contents(
        utils.resource_path("api/file/dataset_157.json")
    )


def test_remote_combine_archive():
    # The child files of a remote COMBINE archive must be retrievable using their file name, even if the local copy of
    # the COMBINE archive is in a directory that is accessed through a symbolic link (e.g., on macOS, the temporary
    # directory is "/var/...", which is a symbolic link to "/private/var/...").
    # Note: this has no JavaScript equivalent since our JavaScript bindings never retrieve the contents of a file, so a
    #       remote file is never downloaded.

    file = File(utils.RemoteBasePath + "/api/file/dataset_135.omex")
    simulation_file = file.child_file("simulation.json")

    assert file.type == File.Type.CombineArchive
    assert simulation_file is not None
    assert utils.to_string(simulation_file.contents) == utils.text_file_contents(
        utils.resource_path("api/file/dataset_135.json")
    )


def test_combine_archive_replaced_with_other_contents():
    # Replacing the contents of a COMBINE archive with some other contents must release its child files.

    file = File(utils.resource_path("cellml_2.omex"))
    file_manager = FileManager.instance()

    assert file_manager.file_count == 3

    file.contents = utils.text_to_list(
        utils.text_file_contents(utils.resource_path("cellml_2.cellml"))
    )

    assert file.type == File.Type.CellmlFile
    assert not file.has_child_files
    assert file_manager.file_count == 1


def test_combine_archive_with_files_outside_of_it():
    # A COMBINE archive may reference files that are located outside of it (e.g., "../sibling.cellml" or even the
    # COMBINE archive itself), in which case they must be ignored rather than replace the contents of another managed
    # file (or result in an infinite recursion).

    expected_issues = [
        [
            Issue.Type.Warning,
            "COMBINE archive: the file '../sibling.cellml' is not located within the COMBINE archive. It has been ignored.",
        ],
        [
            Issue.Type.Warning,
            "COMBINE archive: the file '%2E%2E/encoded_sibling.cellml' is not located within the COMBINE archive. It has been ignored.",
        ],
        [
            Issue.Type.Warning,
            "COMBINE archive: the file '../entries_outside_of_archive.omex' is not located within the COMBINE archive. It has been ignored.",
        ],
    ]

    sibling = File(utils.resource_path("api/file/sibling.cellml"), False)
    some_contents = utils.text_to_list("Some contents.")

    sibling.contents = some_contents

    file = File(utils.resource_path("api/file/entries_outside_of_archive.omex"))

    assert file.type == File.Type.CombineArchive
    assert file.child_file_count == 1
    assert file.child_file("model.cellml") is not None
    utils.assert_issues(file, expected_issues)
    assert sibling.contents == some_contents
    assert FileManager.instance().file_count == 3


@pytest.mark.skipif(
    platform.system() == "Windows",
    reason="A symbolic link usually requires some special privileges on Windows.",
)
def test_combine_archive_with_files_outside_of_it_through_symbolic_link():
    # The files of a COMBINE archive are located in a directory which name is that of the COMBINE archive followed by
    # ".contents". If such a directory happens to exist and to be a symbolic link then the ".." components that follow
    # it are kept (see canonicalFileName()), but the files they refer to are still located outside of the COMBINE
    # archive, so they must still be ignored.
    # Note: a symbolic link usually requires some special privileges on Windows.

    expected_issues = [
        [
            Issue.Type.Warning,
            "COMBINE archive: the file '../sibling.cellml' is not located within the COMBINE archive. It has been ignored.",
        ],
        [
            Issue.Type.Warning,
            "COMBINE archive: the file '%2E%2E/encoded_sibling.cellml' is not located within the COMBINE archive. It has been ignored.",
        ],
        [
            Issue.Type.Warning,
            "COMBINE archive: the file '../entries_outside_of_archive.omex' is not located within the COMBINE archive. It has been ignored.",
        ],
    ]

    dir_path = os.path.join(
        tempfile.gettempdir(),
        "libopencor_combine_archive_with_files_outside_of_it_through_symbolic_link",
    )

    shutil.rmtree(dir_path, ignore_errors=True)
    os.makedirs(os.path.join(dir_path, "real"))
    os.makedirs(os.path.join(dir_path, "cwd"))
    shutil.copyfile(
        utils.resource_path("api/file/entries_outside_of_archive.omex"),
        os.path.join(dir_path, "cwd", "entries_outside_of_archive.omex"),
    )
    os.symlink(
        "../real",
        os.path.join(dir_path, "cwd", "entries_outside_of_archive.omex.contents"),
    )

    orig_dir = os.getcwd()

    os.chdir(os.path.join(dir_path, "cwd"))

    file = File("entries_outside_of_archive.omex")

    os.chdir(orig_dir)

    assert file.type == File.Type.CombineArchive
    assert file.child_file_count == 1
    assert file.child_file("model.cellml") is not None
    utils.assert_issues(file, expected_issues)
    assert FileManager.instance().file_count == 2

    shutil.rmtree(dir_path)


def test_combine_archive_with_percent_encoded_locations():
    # A location is a URI reference, so it may be percent-encoded (e.g., "./my%20file.txt"), in which case the name of
    # its ZIP entry should be its decoded location (e.g., "my file.txt"), although some tools use the location itself
    # (e.g., "other%20file.txt"). Either way, a child file must have the contents of its ZIP entry and be retrievable
    # using either its location or its file name.
    # Note: the ZIP entry for "./my%2520file.txt" is "my%20file.txt", so it must not be used for "./my%20file.txt".

    locations_and_file_names = [
        ("./file.txt", "file.txt"),
        ("./my%20file.txt", "my file.txt"),
        ("./my%2520file.txt", "my%20file.txt"),
        ("./other%20file.txt", "other file.txt"),
    ]
    file = File(utils.resource_path("api/file/percent_encoded_locations.omex"))

    assert file.type == File.Type.CombineArchive
    assert not file.has_issues
    assert file.child_file_count == len(locations_and_file_names)

    for i, (location, file_name) in enumerate(locations_and_file_names):
        child_file = file.child_file(location)

        assert child_file is not None
        assert file.child_file_names[i] == location
        assert file.child_file(i) == child_file
        assert file.child_file(file_name) == child_file
        assert child_file.contents == utils.text_to_list(file_name)


def test_combine_archive_with_non_utf8_locations():
    # A location may get percent-decoded to a file name that is not valid UTF-8 (e.g., "./%FF.txt" gets decoded to
    # "\xFF.txt") or that has a character outside of the Basic Multilingual Plane (e.g., "./%F0%9F%98%80.txt" gets
    # decoded to an emoji), which must not result in an exception being thrown. Either way, a child file must have the
    # contents of its ZIP entry and be retrievable using either its location or its file name.
    # Note: the contents of a ZIP entry is the location of its child file.

    locations_and_file_names = [
        ("./%FF.txt", "\udcff.txt"),
        ("./%F0%9F%98%80.txt", "\U0001f600.txt"),
    ]
    file = File(utils.resource_path("api/file/non_utf8_locations.omex"))

    assert file.type == File.Type.CombineArchive
    assert not file.has_issues
    assert file.child_file_count == len(locations_and_file_names)

    for i, (location, file_name) in enumerate(locations_and_file_names):
        child_file = file.child_file(location)

        assert child_file is not None
        assert file.child_file_names[i] == location
        assert file.child_file(i) == child_file
        assert file.child_file(file_name) == child_file
        assert child_file.file_name.endswith(file_name)
        assert child_file.contents == utils.text_to_list(location)


def test_combine_archive_with_duplicate_locations():
    # Different locations may refer to the same file (e.g., "./my%20file.txt" and "./my file.txt", or "./file.txt" and
    # "file.txt"), in which case only the first one is kept and the other ones are ignored. The ignored locations can,
    # however, still be used to retrieve the child file since they refer to the same file.

    expected_issues = [
        [
            Issue.Type.Warning,
            "COMBINE archive: the file 'file.txt' is the same as the file './file.txt'. It has been ignored.",
        ],
        [
            Issue.Type.Warning,
            "COMBINE archive: the file './my file.txt' is the same as the file './my%20file.txt'. It has been ignored.",
        ],
        [
            Issue.Type.Warning,
            "COMBINE archive: the file './other%20file.txt' is the same as the file './other file.txt'. It has been ignored.",
        ],
    ]
    locations_file_names_and_duplicate_locations = [
        ("./file.txt", "file.txt", "file.txt"),
        ("./my%20file.txt", "my file.txt", "./my file.txt"),
        ("./other file.txt", "other file.txt", "./other%20file.txt"),
    ]
    file = File(utils.resource_path("api/file/duplicate_locations.omex"))

    assert file.type == File.Type.CombineArchive
    utils.assert_issues(file, expected_issues)
    assert file.child_file_count == len(locations_file_names_and_duplicate_locations)

    for i, (location, file_name, duplicate_location) in enumerate(
        locations_file_names_and_duplicate_locations
    ):
        child_file = file.child_file(location)

        assert child_file is not None
        assert file.child_file_names[i] == location
        assert file.child_file(i) == child_file
        assert file.child_file(file_name) == child_file
        assert file.child_file(duplicate_location) == child_file
        assert child_file.contents == utils.text_to_list(file_name)

    assert FileManager.instance().file_count == 4


def test_combine_archive_with_self_referencing_locations():
    # Locations that refer to the COMBINE archive itself (e.g., ".", "./", or "/") are not child files of the COMBINE
    # archive.

    file = File(utils.resource_path("api/file/self_referencing_locations.omex"))

    assert file.type == File.Type.CombineArchive
    assert not file.has_issues
    assert file.child_file_count == 1
    assert file.child_file_names[0] == "./file.txt"
    assert utils.to_string(file.child_file(0).contents) == "file.txt"
    assert file.child_file("./") is None
    assert file.child_file("/") is None
    assert FileManager.instance().file_count == 2
