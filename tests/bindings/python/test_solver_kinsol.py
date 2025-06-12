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
from math import *
import pytest
import utils
from utils import assert_issues


def test_maximum_number_of_iterations_value_with_invalid_number():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The maximum number of iterations cannot be equal to 0. It must be greater than 0.",
        ]
    ]

    file = loc.File(utils.resource_path("api/solver/nla1.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.nla_solver

    solver.maximum_number_of_iterations = 0

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_upper_half_bandwidth_value_with_number_too_small():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 2.",
        ]
    ]

    file = loc.File(utils.resource_path("api/solver/nla2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.nla_solver

    solver.linear_solver = loc.SolverKinsol.LinearSolver.Banded
    solver.upper_half_bandwidth = -1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_upper_half_bandwidth_value_with_number_too_big():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to 1. It must be between 0 and 0.",
        ]
    ]

    file = loc.File(utils.resource_path("api/solver/nla1.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.nla_solver

    solver.linear_solver = loc.SolverKinsol.LinearSolver.Banded
    solver.upper_half_bandwidth = 1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_lower_half_bandwidth_value_with_number_too_small():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 2.",
        ]
    ]

    file = loc.File(utils.resource_path("api/solver/nla2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.nla_solver

    solver.linear_solver = loc.SolverKinsol.LinearSolver.Banded
    solver.lower_half_bandwidth = -1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_lower_half_bandwidth_value_with_number_too_big():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The lower half-bandwidth cannot be equal to 1. It must be between 0 and 0.",
        ]
    ]

    file = loc.File(utils.resource_path("api/solver/nla1.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.nla_solver

    solver.linear_solver = loc.SolverKinsol.LinearSolver.Banded
    solver.lower_half_bandwidth = 1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


# ---GRY--- RENAME THIS TEST test_compiled_solve AND CREATE ONE CALLED test_interpreted_solve ONCE WE CAN INTERPRET
#           NLA-BASED MODELS.


ABS_TOL = 1e-05


def expect_nla1_solution(instance_task):
    assert instance_task.state_count == 0
    assert instance_task.rate_count == 0
    assert instance_task.constant_count == 0
    assert instance_task.computed_constant_count == 0
    assert instance_task.algebraic_count == 2

    assert instance_task.algebraic(0)[0] == pytest.approx(3.0, abs=ABS_TOL)
    assert instance_task.algebraic(1)[0] == pytest.approx(7.0, abs=ABS_TOL)


def expect_nla2_solution(instance_task):
    assert instance_task.state_count == 0
    assert instance_task.rate_count == 0
    assert instance_task.constant_count == 0
    assert instance_task.computed_constant_count == 0
    assert instance_task.algebraic_count == 3

    assert instance_task.algebraic(0)[0] == pytest.approx(3.0, abs=ABS_TOL)
    assert instance_task.algebraic(1)[0] == pytest.approx(7.0, abs=ABS_TOL)
    assert instance_task.algebraic(2)[0] == pytest.approx(-5.0, abs=ABS_TOL)


def test_solve():
    file = loc.File(utils.resource_path("api/solver/nla1.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    expect_nla1_solution(instance.tasks[0])


def test_solve_with_banded_linear_solver():
    file = loc.File(utils.resource_path("api/solver/nla2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    expect_nla2_solution(instance.tasks[0])


def test_solve_with_gmres_linear_solver():
    file = loc.File(utils.resource_path("api/solver/nla1.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    expect_nla1_solution(instance.tasks[0])


def test_solve_with_bicgstab_linear_solver():
    file = loc.File(utils.resource_path("api/solver/nla2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    expect_nla2_solution(instance.tasks[0])


def test_solve_with_tfqmr_linear_solver():
    file = loc.File(utils.resource_path("api/solver/nla1.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    expect_nla1_solution(instance.tasks[0])
