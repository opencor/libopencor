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


import base64
import libopencor as loc
import math
import os
import pathlib
import platform
import pytest


ResourceLocation = "@RESOURCE_LOCATION@"


UnknownFile = "unknown_file.txt"
SbmlFile = "sbml.sbml"
ErrorCellmlFile = "error.cellml"
ErrorSedmlFile = "error.sedml"
Cellml1xFile = "cellml_1_x.cellml"
Sedml1xFile = "cellml_1_x.sedml"
Combine1xArchive = "cellml_1_x.omex"
Cellml2File = "cellml_2.cellml"
Sedml2File = "cellml_2.sedml"
Combine2Archive = "cellml_2.omex"
IrretrievableFile = "irretrievable_file.txt"


if platform.system() == "Windows":
    LocalFile = "P:\\some\\path\\file.txt"
else:
    LocalFile = "/some/path/file.txt"


# Note: the three following URLs use http rather than https to help with coverage testing.
HttpRemoteCellmlFile = "http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"
HttpRemoteSedmlFile = "http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml"
HttpRemoteCombineArchive = (
    "http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex"
)
RemoteBasePath = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res"
RemoteFile = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"
EncodedRemoteFile = "https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO%20BG%20example%203.1.cellml"
NonEncodedRemoteFile = "https://models.physiomeproject.org/workspace/aed/@@rawfile/d4accf8429dbf5bdd5dfa1719790f361f5baddbe/FAIRDO BG example 3.1.cellml"
UnknownRemoteFile = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/unknown_file.txt"
IrretrievableRemoteFile = "https://some.domain.com/irretrievable_file.txt"


NoContents = ""
SomeUnknownContents = "Some unknown contents..."
SomeCellmlContents = """@CELLML_CONTENTS@"""
SomeErrorCellmlContents = """@ERROR_CELLML_CONTENTS@"""
SomeWarningCellmlContents = """@WARNING_CELLML_CONTENTS@"""
SomeSedmlContents = """@SEDML_CONTENTS@"""
SomeCombineArchiveContents = base64.b64decode(b"@BASE64_COMBINE_ARCHIVE_CONTENTS@")


def assert_issues(logger, expected_issues):
    issues = logger.issues

    assert len(issues) == len(expected_issues)

    for i in range(len(issues)):
        assert issues[i].type == expected_issues[i][0]
        assert issues[i].description == expected_issues[i][1]

        if issues[i].type == loc.Issue.Type.Error:
            assert issues[i].type_as_string == "Error"
        else:
            assert issues[i].type_as_string == "Warning"


def assert_value(value, expected_value, abs_tol):
    if math.isinf(expected_value):
        assert math.isinf(value) is True
    elif math.isnan(expected_value):
        assert math.isnan(value) is True
    else:
        assert value == pytest.approx(expected_value, abs=abs_tol)


def assert_values(
    instance_task,
    index,
    state_values,
    state_abs_tols,
    rate_values,
    rate_abs_tols,
    constant_values,
    constant_abs_tols,
    computed_constant_values,
    computed_constant_abs_tols,
    algebraic_values,
    algebraic_abs_tols,
):
    for i in range(instance_task.state_count):
        assert_value(instance_task.state(i)[index], state_values[i], state_abs_tols[i])

    for i in range(instance_task.rate_count):
        assert_value(instance_task.rate(i)[index], rate_values[i], rate_abs_tols[i])

    for i in range(instance_task.constant_count):
        assert_value(
            instance_task.constant(i)[index],
            constant_values[i],
            constant_abs_tols[i],
        )

    for i in range(instance_task.computed_constant_count):
        assert_value(
            instance_task.computed_constant(i)[index],
            computed_constant_values[i],
            computed_constant_abs_tols[i],
        )

    for i in range(instance_task.algebraic_count):
        assert_value(
            instance_task.algebraic(i)[index],
            algebraic_values[i],
            algebraic_abs_tols[i],
        )


def resource_path(relative_path=""):
    return os.path.realpath(ResourceLocation + "/" + relative_path)


def string_to_list(string):
    return [ord(x) for x in string]


def binary_to_list(string):
    return [x for x in string]


def string_to_path(string):
    return pathlib.Path(string)


def path_to_string(path):
    return str(path)


def to_string(array):
    return "".join([chr(x) for x in array])


def text_file_contents(file_name):
    with open(file_name, "r") as file:
        return file.read()
