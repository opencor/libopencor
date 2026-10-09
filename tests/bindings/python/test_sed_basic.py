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
import utils
from utils import assert_issues


def sedml_contents(model_source):
    return f"""<?xml version='1.0' encoding='UTF-8'?>
<sedML level="1" version="3" xmlns="http://sed-ml.org/sed-ml/level1/version3">
    <listOfModels>
        <model id="model" language="urn:sedml:language:cellml" source="{model_source}"/>
    </listOfModels>
</sedML>
"""


def test_no_file():
    document = loc.SedDocument()

    assert not document.has_issues

    document = loc.SedDocument(None)

    assert not document.has_issues


def test_unknown_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = loc.File(utils.resource_path("unknown_file.txt"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_cellml_file():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)

    assert not document.has_issues


def test_sedml_file():
    file = loc.File(utils.resource_path("cellml_2.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues

    needed_file = loc.File(utils.resource_path("cellml_2.cellml"))

    document = loc.SedDocument(file)

    assert not document.has_issues


def test_sedml_file_with_absolute_cellml_file():
    file = loc.File(utils.resource_path("api/sed/absolute_cellml_file.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues

    needed_file = loc.File(utils.resource_path("file.txt"))

    document = loc.SedDocument(file)

    assert not document.has_issues


def test_sedml_file_with_relative_cellml_file():
    file = loc.File(utils.resource_path("api/sed/relative_cellml_file.sedml"), False)

    file.contents = utils.text_to_list(sedml_contents("../../cellml_2.cellml"))

    document = loc.SedDocument(file)

    assert len(document.models) == 1
    assert document.models[0].file.path == utils.resource_path("cellml_2.cellml")

    needed_file = loc.File(utils.resource_path("cellml_2.cellml"))

    document = loc.SedDocument(file)

    assert not document.has_issues
    assert len(document.models) == 1
    assert document.models[0].file == needed_file


def test_sedml_file_with_relative_cellml_file_in_working_directory():
    # A relative model source is relative to the SED-ML file, not to the current working directory, even if the
    # current working directory contains a file with that name.

    orig_dir = os.getcwd()

    os.chdir(utils.resource_path())

    file = loc.File(utils.resource_path("api/sed/relative_cellml_file.sedml"), False)

    file.contents = utils.text_to_list(sedml_contents("cellml_2.cellml"))

    document = loc.SedDocument(file)

    os.chdir(orig_dir)

    assert len(document.models) == 1
    assert document.models[0].file.path == utils.resource_path(
        "api/sed/cellml_2.cellml"
    )


def test_sedml_file_with_remote_cellml_file():
    file = loc.File(utils.resource_path("api/sed/remote_cellml_file.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues

    needed_file = loc.File(utils.RemoteFile)

    document = loc.SedDocument(file)

    assert not document.has_issues


def test_combine_archive():
    file = loc.File(utils.resource_path("cellml_2.omex"))
    document = loc.SedDocument(file)

    assert not document.has_issues


def test_combine_archive_with_no_manifest_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/no_manifest_file.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_one_cellml_file():
    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_cellml_file.omex")
    )
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert document.simulations[0].output_end_time == 1000.0

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


def test_combine_archive_with_no_master_file_and_one_unknown_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_unknown_cellml_file.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_several_cellml_files():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file, no SED-ML file, and several CellML files.",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_several_cellml_files.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_one_sedml_file():
    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_sedml_file.omex")
    )
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert document.simulations[0].output_end_time == 50.0

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


def test_combine_archive_with_no_master_file_and_one_unknown_sedml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_one_unknown_sedml_file.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file_and_several_sedml_files():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file and several SED-ML files.",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/no_master_file_with_several_sedml_files.omex")
    )
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_unknown_direct_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unknown_direct_cellml_file.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_unknown_indirect_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task: task 'task1' requires a model of CellML type.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unknown_indirect_cellml_file.omex"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    assert_issues(instance, expected_issues)


def test_combine_archive_with_unknown_sedml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unknown_sedml_file.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_irretrievable_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an irretrievable file.",
        ],
    ]

    file = loc.File(utils.resource_path("irretrievable_file.txt"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
