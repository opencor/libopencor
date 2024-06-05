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


from libopencor import File, Issue, SedDocument
import utils
from utils import assert_issues


def test_no_file():
    document = SedDocument()

    assert document.has_issues == False


def test_unknown_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = File(utils.resource_path(utils.UNKNOWN_FILE))
    document = SedDocument(file)

    assert_issues(document, expected_issues)


def test_cellml_file():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    document = SedDocument(file)

    assert document.has_issues == False


def test_sedml_file():
    file = File(utils.resource_path(utils.SEDML_2_FILE))
    document = SedDocument(file)

    assert document.has_issues == False


def test_combine_archive():
    file = File(utils.resource_path(utils.COMBINE_2_ARCHIVE))
    document = SedDocument(file)

    assert document.has_issues == False


def test_irretrievable_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "A simulation experiment description cannot be created using an irretrievable file.",
        ],
    ]

    file = File(utils.resource_path(utils.IRRETRIEVABLE_FILE))
    document = SedDocument(file)

    assert_issues(document, expected_issues)
