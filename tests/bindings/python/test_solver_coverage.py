# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import libopencor as loc
import pytest
import utils
from utils import assert_issues


def test_ode_changes():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "The variable of integration 'time' in component 'environment'cannot be changed. Only state variables and constants can be changed.",
        ],
        [
            loc.Issue.Type.Warning,
            "The variable 'membrane' in component 'X'could not be found and therefore could not be changed.",
        ],
        [
            loc.Issue.Type.Warning,
            "The computed constant 'E_Na' in component 'sodium_channel' cannot be changed. Only state variables and constants can be changed.",
        ],
        [
            loc.Issue.Type.Warning,
            "The algebraic variable 'i_Stim' in component 'membrane' cannot be changed. Only state variables and constants can be changed.",
        ],
    ]

    file = loc.File(utils.resource_path("api/solver/ode_sed_changes.omex"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    assert_issues(instance, expected_issues)


def test_algebraic_changes():
    # We want to solve a system of three unknowns:
    # Variables:
    #  • a: 0
    #  • x: 1 ->  3
    #  • y: 1 -> -5
    #  • z: 1 ->  7
    # Equations:
    #  • k = a
    #  •  x +  y +  z +  k =  5
    #  • 6x - 4y + 5z - 3k = 73
    #  • 5x + 2y + 2z - 5k = 19
    #
    # When a = 0, we have:
    #  • x =  3
    #  • y = -5
    #  • z =  7
    # but our SED-ML file has a change attribute that sets a to 13, so we should now have:
    #  • x =  100/3  =  33.333333333333336
    #  • y = -356/27 = -13.185185185185185
    #  • z = -760/27 = -28.148148148148145

    file = loc.File(utils.resource_path("api/solver/algebraic_sed_changes.omex"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    ABS_TOL = 1e-05

    instance_task = instance.tasks[0]

    assert instance_task.state_count == 0
    assert instance_task.rate_count == 0
    assert instance_task.constant_count == 1
    assert instance_task.computed_constant_count == 1
    assert instance_task.algebraic_count == 3

    assert instance_task.algebraic(0)[0] == pytest.approx(33.33333, abs=ABS_TOL)
    assert instance_task.algebraic(1)[0] == pytest.approx(-28.14815, abs=ABS_TOL)
    assert instance_task.algebraic(2)[0] == pytest.approx(-13.18519, abs=ABS_TOL)
