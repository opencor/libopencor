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


import libopencor as oc
import ode_model
import utils
from utils import assert_issues


def test_step_value_with_invalid_number():
    expected_issues = [
        [
            oc.Issue.Type.Error,
            "The step cannot be equal to 0. It must be greater than 0.",
        ],
    ]

    file = oc.File(utils.resource_path("api/solver/ode.cellml"))
    document = oc.SedDocument(file)
    simulation = document.simulations[0]
    solver = oc.SolverFourthOrderRungeKutta()

    solver.step = 0.0

    simulation.ode_solver = solver

    instance = document.instantiate()

    instance.run()

    assert_issues(instance, expected_issues)


def fourth_order_runge_kutta_solve(
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
    compiled,
):
    file = oc.File(utils.resource_path("api/solver/ode.cellml"))
    document = oc.SedDocument(file)
    simulation = document.simulations[0]
    solver = oc.SolverFourthOrderRungeKutta()

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
        compiled,
    )


state_values = [-63.821233, 0.134844, 0.984267, 0.741105]
state_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
rate_values = [49.702735, -0.127922, -0.051225, 0.098266]
rate_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
computed_constant_values = [-10.613, -115.0, 12.0]
computed_constant_abs_tols = [0.0, 0.0, 0.0]
algebraic_values = [
    0.0,
    -15.96247,
    -823.402257,
    789.661995,
    3.963806,
    0.115402,
    0.002879,
    0.967141,
    0.540698,
    0.056292,
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


def test_compiled_solve():
    fourth_order_runge_kutta_solve(
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
        True,
    )


def test_interpreted_solve():
    fourth_order_runge_kutta_solve(
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
        False,
    )
