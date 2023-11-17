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
import platform
import utils
from utils import assert_issues


def expected_serialisation(source):
    return f"""<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="{source}"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
</sed>
"""


def test_local_cellml_file_with_base_path():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)

    assert sed.serialise(utils.resource_path()) == expected_serialisation(
        "cellml_2.cellml"
    )


def test_local_cellml_file_without_base_path():
    file = File(utils.LOCAL_FILE)

    file.set_contents(utils.string_to_list(utils.SOME_CELLML_CONTENTS))

    sed = SedDocument(file)

    if platform.system() == "Windows":
        assert sed.serialise() == expected_serialisation(
            "file:///P:/some/path/file.txt"
        )
    else:
        assert sed.serialise() == expected_serialisation("file:///some/path/file.txt")


def test_relative_local_cellml_file_with_base_path():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)

    assert sed.serialise(utils.resource_path() + "../..") == expected_serialisation(
        "tests/res/cellml_2.cellml"
    )


def test_relative_local_cellml_file_without_base_path():
    file = File(utils.CELLML_2_FILE)

    file.set_contents(utils.string_to_list(utils.SOME_CELLML_CONTENTS))

    sed = SedDocument(file)

    assert sed.serialise() == expected_serialisation("cellml_2.cellml")


def test_remote_cellml_file_with_base_path():
    file = File(utils.REMOTE_FILE)
    sed = SedDocument(file)

    assert sed.serialise(utils.REMOTE_BASE_PATH) == expected_serialisation(
        "cellml_2.cellml"
    )


def test_remote_cellml_file_without_base_path():
    file = File(utils.REMOTE_FILE)
    sed = SedDocument(file)

    assert sed.serialise() == expected_serialisation(
        "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"
    )


def test_relative_remote_cellml_file_with_base_path():
    file = File(utils.REMOTE_FILE)
    sed = SedDocument(file)

    assert sed.serialise(utils.REMOTE_BASE_PATH + "/../..") == expected_serialisation(
        "tests/res/cellml_2.cellml"
    )
