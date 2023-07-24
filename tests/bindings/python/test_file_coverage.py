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


from libopencor import File, Issue
import utils
from utils import assert_issues


def test_http_remote_file():
    File(utils.HTTP_REMOTE_FILE)


def test_irretrievable_remote_file():
    File(utils.IRRETRIEVABLE_REMOTE_FILE)


def test_error_cellml_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS).",
        ],
    ]

    file = File(
        utils.UNIX_LOCAL_FILE, utils.string_to_list(utils.SOME_ERROR_CELLML_CONTENTS)
    )

    assert_issues(expected_issues, file)


def test_warning_cellml_file():
    expected_issues = [
        [
            Issue.Type.Warning,
            "The units in 'metre = 1.0' in component 'main' are not equivalent. 'metre' is in 'metre' while '1.0' is 'dimensionless'.",
        ],
    ]

    file = File(
        utils.UNIX_LOCAL_FILE, utils.string_to_list(utils.SOME_WARNING_CELLML_CONTENTS)
    )

    assert_issues(expected_issues, file)
