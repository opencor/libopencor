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


import libopencor as loc
import ode_model
import utils
from utils import assert_issues


def test_step_value_with_invalid_number():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task instance | Heun: the step cannot be equal to 0. It must be greater than 0.",
        ],
    ]

    file = loc.File(utils.resource_path("api/solver/ode.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = loc.SolverHeun()

    solver.step = 0.0

    simulation.ode_solver = solver

    instance = document.instantiate()

    instance.run()

    assert_issues(instance, expected_issues)


def heun_solve(
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
    file = loc.File(utils.resource_path("api/solver/ode.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = loc.SolverHeun()

    solver.step = 0.0123

    simulation.ode_solver = solver

    ode_model.run(
        document,
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
    )


state_values = [-63.691259, 0.134516, 0.984133, 0.741370]
state_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
rate_values = [49.66942, -0.127532, -0.051693, 0.097711]
rate_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
computed_constant_values = [-10.613, -115.0, 12.0]
computed_constant_abs_tols = [0.0, 0.0, 0.0]
algebraic_values = [
    0.0,
    -15.923478,
    -823.166811,
    789.421406,
    3.951622,
    0.116239,
    0.002898,
    0.966726,
    0.539425,
    0.056383,
]
algebraic_abs_tols = [
    0.000001,
    0.000001,
    0.000001,
    0.000001,
    0.000001,
    0.000001,
    0.000001,
    0.000001,
    0.000001,
    0.000001,
]


def test_solve():
    heun_solve(
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
    )
