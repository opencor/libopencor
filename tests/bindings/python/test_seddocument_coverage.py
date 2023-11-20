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


from libopencor import SedDocument, SedSimulationUniformTimeCourse


def test_initialise():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
"""

    sed = SedDocument()

    assert sed.serialise() == expected_serialisation


def test_simulations():
    sed = SedDocument()

    assert len(sed.simulations) == 0
    assert sed.add_simulation(None) == False

    simulation = SedSimulationUniformTimeCourse(sed)

    assert sed.add_simulation(simulation) == True

    assert len(sed.simulations) == 1
    assert sed.simulations[0] == simulation

    assert sed.add_simulation(simulation) == False
    assert sed.remove_simulation(simulation) == True

    assert len(sed.simulations) == 0

    assert sed.remove_simulation(None) == False
