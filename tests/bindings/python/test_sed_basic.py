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
import utils
from utils import assert_issues


def test_no_file():
    document = oc.SedDocument()

    assert document.has_issues == False


def test_unknown_file():
    expected_issues = [
        [
            oc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = oc.File(utils.resource_path(utils.UnknownFile))
    document = oc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_cellml_file():
    file = oc.File(utils.resource_path(utils.Cellml2File))
    document = oc.SedDocument(file)

    assert document.has_issues == False


def test_sedml_file():
    file = oc.File(utils.resource_path(utils.Sedml2File))
    document = oc.SedDocument(file)

    assert document.has_issues == True

    needed_file = oc.File(utils.resource_path(utils.Cellml2File))

    document = oc.SedDocument(file)

    assert document.has_issues == False


def test_sedml_file_with_absolute_cellml_file():
    file = oc.File(utils.resource_path("api/sed/absolute_cellml_file.sedml"))
    document = oc.SedDocument(file)

    assert document.has_issues == True

    needed_file = oc.File(utils.LocalFile)

    document = oc.SedDocument(file)

    assert document.has_issues == False


def test_sedml_file_with_remote_cellml_file():
    file = oc.File(utils.resource_path("api/sed/remote_cellml_file.sedml"))
    document = oc.SedDocument(file)

    assert document.has_issues == True

    needed_file = oc.File(utils.RemoteFile)

    document = oc.SedDocument(file)

    assert document.has_issues == False


def test_combine_archive():
    file = oc.File(utils.resource_path(utils.Combine2Archive))
    document = oc.SedDocument(file)

    assert document.has_issues == False


def test_combine_archive_with_no_manifest_file():
    expected_issues = [
        [
            oc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
        ],
    ]

    file = oc.File(utils.resource_path("api/sed/no_manifest_file.omex"))
    document = oc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_no_master_file():
    expected_issues = [
        [
            oc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with no master file.",
        ],
    ]

    file = oc.File(utils.resource_path("api/sed/no_master_file.omex"))
    document = oc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_combine_archive_with_sbml_file_as_master_file():
    expected_issues = [
        [
            oc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = oc.File(utils.resource_path("api/sed/sbml_file_as_master_file.omex"))
    document = oc.SedDocument(file)

    assert_issues(document, expected_issues)


def test_irretrievable_file():
    expected_issues = [
        [
            oc.Issue.Type.Error,
            "A simulation experiment description cannot be created using an irretrievable file.",
        ],
    ]

    file = oc.File(utils.resource_path(utils.IrretrievableFile))
    document = oc.SedDocument(file)

    assert_issues(document, expected_issues)
