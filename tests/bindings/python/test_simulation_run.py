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


from libopencor import File, Issue, Simulation
import utils
from utils import assert_issues


def test_run_unknown_file():
    file = File(utils.resource_path(utils.UNKNOWN_FILE))
    simulation = Simulation(file)

    simulation.run()


def test_run_cellml_file():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    simulation = Simulation(file)

    simulation.run()


def test_run_sedml_file():
    file = File(utils.resource_path(utils.SEDML_2_FILE))
    simulation = Simulation(file)

    simulation.run()


def test_run_combine_archive():
    file = File(utils.resource_path(utils.COMBINE_2_ARCHIVE))
    simulation = Simulation(file)

    simulation.run()


def test_run_irretrievable_file():
    file = File(utils.resource_path(utils.IRRETRIEVABLE_FILE))
    simulation = Simulation(file)

    simulation.run()
