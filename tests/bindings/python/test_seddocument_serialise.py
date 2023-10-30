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


def test_local_cellml_file_with_base_path():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument()

    sed.initialise(file)

    assert sed.serialise(utils.resource_path()) == expected_serialisation


def test_local_cellml_file_without_base_path():
    if platform.system() == "Windows":
        expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="file:///P:/some/path/file.txt"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""
    else:
        expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="file:///some/path/file.txt"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""

    file = File(utils.LOCAL_FILE)
    sed = SedDocument()

    file.set_contents(utils.string_to_list(utils.SOME_CELLML_CONTENTS))
    sed.initialise(file)

    assert sed.serialise() == expected_serialisation


def test_relative_local_cellml_file_with_base_path():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="tests/res/cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument()

    sed.initialise(file)

    assert sed.serialise(utils.resource_path() + "../..") == expected_serialisation


def test_relative_local_cellml_file_without_base_path():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""
    file = File(utils.CELLML_2_FILE)
    sed = SedDocument()

    file.set_contents(utils.string_to_list(utils.SOME_CELLML_CONTENTS))
    sed.initialise(file)

    assert sed.serialise() == expected_serialisation


def test_remote_cellml_file_with_base_path():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""
    file = File(utils.REMOTE_FILE)
    sed = SedDocument()

    sed.initialise(file)

    assert sed.serialise(utils.REMOTE_BASE_PATH) == expected_serialisation


def test_remote_cellml_file_without_base_path():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""
    file = File(utils.REMOTE_FILE)
    sed = SedDocument()

    sed.initialise(file)

    assert sed.serialise() == expected_serialisation


def test_relative_remote_cellml_file_with_base_path():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="tests/res/cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000"/>
  </listOfSimulations>
</sed>
"""
    file = File(utils.REMOTE_FILE)
    sed = SedDocument()

    sed.initialise(file)

    assert sed.serialise(utils.REMOTE_BASE_PATH + "/../..") == expected_serialisation
