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


from libopencor import File, Issue, SedDocument, SolverCvode
import ode_model
import utils
from utils import assert_issues


def test_maximum_step_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The maximum step cannot be equal to -1.234. It must be greater or equal to 0.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.maximum_step = -1.234

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_maximum_number_of_steps_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The maximum number of steps cannot be equal to 0. It must be greater than 0.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.maximum_number_of_steps = 0

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_upper_half_bandwidth_value_with_number_too_small():
    expected_issues = [
        [
            Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.upper_half_bandwidth = -1

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_upper_half_bandwidth_value_with_number_too_big():
    expected_issues = [
        [
            Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.upper_half_bandwidth = 4

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_lower_half_bandwidth_value_with_number_too_small():
    expected_issues = [
        [
            Issue.Type.Error,
            "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.lower_half_bandwidth = -1

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_lower_half_bandwidth_value_with_number_too_big():
    expected_issues = [
        [
            Issue.Type.Error,
            "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.lower_half_bandwidth = 4

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_relative_tolerance_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.relative_tolerance = -1.234

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_absolute_tolerance_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
        ],
    ]

    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.absolute_tolerance = -1.234

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def cvode_solve(state_values, rate_values, variable_values, compiled):
    file = File(utils.resource_path("api/solver/ode/model.cellml"))
    document = SedDocument(file)

    ode_model.run(document, state_values, rate_values, variable_values, compiled)


def test_compiled_solve():
    state_values = [-63.885782, 0.135007, 0.984333, 0.740973]
    rate_values = [49.725595, -0.128192, -0.050905, 0.098648]
    variable_values = [
        0.0,
        -15.981835,
        -823.516207,
        789.778615,
        1.0,
        0.0,
        -10.613,
        0.3,
        -115.0,
        120.0,
        3.969859,
        0.114989,
        0.00287,
        0.967346,
        12.0,
        36.0,
        0.541331,
        0.056246,
    ]

    cvode_solve(state_values, rate_values, variable_values, True)


def test_interpreted_solve():
    state_values = [-63.886003, 0.135008, 0.984333, 0.7409728]
    rate_values = [49.725651, -0.128193, -0.050904, 0.098649]
    variable_values = [
        0.0,
        -15.981901,
        -823.516588,
        789.779007,
        1.0,
        0.0,
        -10.613,
        0.3,
        -115.0,
        120.0,
        3.96988,
        0.114988,
        0.00287,
        0.967346,
        12.0,
        36.0,
        0.541333,
        0.056246,
    ]

    cvode_solve(state_values, rate_values, variable_values, False)
