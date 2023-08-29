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


from libopencor import File, SedDocument
import utils


def test_initialise():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\"/>
"""
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument()

    sed.initialise(None)

    assert sed.serialise() == expected_serialisation
