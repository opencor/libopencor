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
import utils
from utils import assert_issues


def test_no_file():
    document = loc.SedDocument()

    assert document.has_issues == False

    document = loc.SedDocument(None)

    assert document.has_issues == False


def test_unknown_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = loc.File(utils.resource_path(utils.UnknownFile))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_cellml_file():
    file = loc.File(utils.resource_path(utils.Cellml2File))
    document = loc.SedDocument(file)

    assert document.has_issues == False


def test_sedml_file():
    file = loc.File(utils.resource_path(utils.Sedml2File))
    document = loc.SedDocument(file)

    assert document.has_issues == True

    needed_file = loc.File(utils.resource_path(utils.Cellml2File))

    document = loc.SedDocument(file)

    assert document.has_issues == False


def test_sedml_file_with_absolute_cellml_file():
    file = loc.File(utils.resource_path("api/sed/absolute_cellml_file.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues == True

    needed_file = loc.File(utils.LocalFile)

    document = loc.SedDocument(file)

    assert document.has_issues == False


def test_sedml_file_with_remote_cellml_file():
    file = loc.File(utils.resource_path("api/sed/remote_cellml_file.sedml"))
    document = loc.SedDocument(file)

    assert document.has_issues == True

    needed_file = loc.File(utils.RemoteFile)

    document = loc.SedDocument(file)

    assert document.has_issues == False


def test_combine_archive():
    file = loc.File(utils.resource_path(utils.Combine2Archive))
    document = loc.SedDocument(file)

    assert document.has_issues == False


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


def test_combine_archive_with_no_master_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/no_master_file.omex"))
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
            "Task 'task1' requires a model of CellML type.",
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

    file = loc.File(utils.resource_path(utils.IrretrievableFile))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
