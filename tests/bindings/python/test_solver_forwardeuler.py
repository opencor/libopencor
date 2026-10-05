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
            "Task instance | Forward Euler: the step cannot be equal to 0. It must be greater than 0.",
        ],
    ]

    file = loc.File(utils.resource_path("api/solver/ode.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = loc.SolverForwardEuler()

    solver.step = 0.0

    simulation.ode_solver = solver

    instance = document.instantiate()

    instance.run()

    assert_issues(instance, expected_issues)


def forward_euler_solve(
    step,
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
    solver = loc.SolverForwardEuler()

    solver.step = step

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


state_values = [-63.787727, 0.134748, 0.984255, 0.741178]
state_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
rate_values = [49.723292, -0.127811, -0.051436, 0.09812]
rate_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
computed_constant_values = [-10.613, -115.0, 12.0]
computed_constant_abs_tols = [0.0, 0.0, 0.0]
algebraic_values = [
    0.0,
    -15.952418,
    -823.361177,
    789.590304,
    3.960664,
    0.115617,
    0.002884,
    0.967035,
    0.54037,
    0.056315,
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
    forward_euler_solve(
        0.0123,
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


def test_solve_with_several_steps_per_output_point():
    # Note: the output interval is 0.001, hence our step means that we have several steps per output point.

    forward_euler_solve(
        0.00023,
        [-63.86285, 0.134946, 0.984315, 0.741021],
        state_abs_tols,
        [49.719825, -0.128045, -0.051096, 0.098444],
        rate_abs_tols,
        constant_values,
        constant_abs_tols,
        computed_constant_values,
        computed_constant_abs_tols,
        [
            0.0,
            -15.974955,
            -823.479865,
            789.734995,
            3.967708,
            0.115136,
            0.002873,
            0.967273,
            0.541106,
            0.056262,
        ],
        algebraic_abs_tols,
    )
