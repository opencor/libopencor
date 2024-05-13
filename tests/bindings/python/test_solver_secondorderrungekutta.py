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


from libopencor import File, Issue, SedDocument, SolverSecondOrderRungeKutta
import ode_model
import utils
from utils import assert_issues


def test_step_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The step cannot be equal to 0. It must be greater than 0.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = SolverSecondOrderRungeKutta()

    solver.step = 0.0

    simulation.ode_solver = solver

    instance = document.create_instance()

    instance.run()

    assert_issues(instance, expected_issues)


def second_order_runge_kutta_solve(
    state_values, rate_values, variable_values, compiled
):
    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = SolverSecondOrderRungeKutta()

    solver.step = 0.0123

    simulation.ode_solver = solver

    ode_model.run(document, state_values, rate_values, variable_values, compiled)


state_values = [-63.886525, 0.135009, 0.984334, 0.740971]
rate_values = [49.725722, -0.128194, -0.050903, 0.098651]
variable_values = [
    0.0,
    -15.982058,
    -823.516942,
    789.779614,
    1.0,
    0.0,
    -10.613,
    0.3,
    -115.0,
    120.0,
    3.969929,
    0.114985,
    0.00287,
    0.967348,
    12.0,
    36.0,
    0.541338,
    0.056246,
]


def test_compiled_solve():
    second_order_runge_kutta_solve(state_values, rate_values, variable_values, True)


def test_interpreted_solve():
    second_order_runge_kutta_solve(state_values, rate_values, variable_values, False)
