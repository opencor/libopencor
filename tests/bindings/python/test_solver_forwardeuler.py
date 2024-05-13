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


from libopencor import File, Issue, SedDocument, SolverForwardEuler
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
    solver = SolverForwardEuler()

    solver.step = 0.0

    simulation.ode_solver = solver

    instance = document.create_instance()

    instance.run()

    assert_issues(instance, expected_issues)


def forward_euler_solve(state_values, rate_values, variable_values, compiled):
    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = SolverForwardEuler()

    solver.step = 0.0123

    simulation.ode_solver = solver

    ode_model.run(document, state_values, rate_values, variable_values, compiled)


state_values = [-63.787727, 0.134748, 0.984255, 0.741178]
rate_values = [49.73577, -0.127963, -0.051257, 0.098331]
variable_values = [
    0.0,
    -15.952418,
    -823.361177,
    789.590304,
    1.0,
    0.0,
    -10.613,
    0.3,
    -115.0,
    120.0,
    3.960664,
    0.115617,
    0.002884,
    0.967035,
    12.0,
    36.0,
    0.54037,
    0.056315,
]


def test_compiled_solve():
    forward_euler_solve(state_values, rate_values, variable_values, True)


def test_interpreted_solve():
    forward_euler_solve(state_values, rate_values, variable_values, False)
