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


def test_unknown_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "A simulation experiment description cannot be created using an unknown file.",
        ],
    ]

    file = File(utils.resource_path(utils.UNKNOWN_FILE))
    simulation = SedDocument(file)

    assert_issues(expected_issues, simulation)


def test_cellml_file():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels/>
</sed>
"""
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    simulation = SedDocument(file)

    assert not simulation.has_issues
    assert simulation.serialise == expected_serialisation


def test_sedml_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "A simulation experiment description cannot currently be created using a SED-ML file.",
        ],
    ]

    file = File(utils.resource_path(utils.SEDML_2_FILE))
    simulation = SedDocument(file)

    assert_issues(expected_issues, simulation)


def test_combine_archive():
    expected_issues = [
        [
            Issue.Type.Error,
            "A simulation experiment description cannot currently be created using a COMBINE archive.",
        ],
    ]

    file = File(utils.resource_path(utils.COMBINE_2_ARCHIVE))
    simulation = SedDocument(file)

    assert_issues(expected_issues, simulation)


def test_irretrievable_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "A simulation experiment description cannot be created using an irretrievable file.",
        ],
    ]

    file = File(utils.resource_path(utils.IRRETRIEVABLE_FILE))
    simulation = SedDocument(file)

    assert_issues(expected_issues, simulation)
