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
from math import *
import ode_model
import utils
from utils import assert_issues


def test_maximum_step_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The maximum step cannot be equal to -1.234. It must be greater or equal to 0.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.maximum_step = -1.234

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_maximum_number_of_steps_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The maximum number of steps cannot be equal to 0. It must be greater than 0.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.maximum_number_of_steps = 0

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_upper_half_bandwidth_value_with_number_too_small():
    expected_issues = [
        [
            Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.upper_half_bandwidth = -1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_upper_half_bandwidth_value_with_number_too_big():
    expected_issues = [
        [
            Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.upper_half_bandwidth = 4

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_lower_half_bandwidth_value_with_number_too_small():
    expected_issues = [
        [
            Issue.Type.Error,
            "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.lower_half_bandwidth = -1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_banded_linear_solver_and_lower_half_bandwidth_value_with_number_too_big():
    expected_issues = [
        [
            Issue.Type.Error,
            "The lower half-bandwidth cannot be equal to 4. It must be between 0 and 3.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded
    solver.lower_half_bandwidth = 4

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_relative_tolerance_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The relative tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.relative_tolerance = -1.234

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_absolute_tolerance_value_with_invalid_number():
    expected_issues = [
        [
            Issue.Type.Error,
            "The absolute tolerance cannot be equal to -1.234. It must be greater or equal to 0.",
        ]
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.absolute_tolerance = -1.234

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def cvode_solve(
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
    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)

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


def test_compiled_solve():
    state_values = [-63.886, 0.135007, 0.984333, 0.740973]
    state_abs_tols = [0.001, 0.000001, 0.000001, 0.000001]
    rate_values = [49.726, -0.128192, -0.050904, 0.098649]
    rate_abs_tols = [0.001, 0.000001, 0.000001, 0.000001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.9819,
        -823.517,
        789.779,
        3.9699,
        0.11499,
        0.002869,
        0.967346,
        0.54133,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.0001,
        0.001,
        0.001,
        0.0001,
        0.00001,
        0.000001,
        0.000001,
        0.00001,
        0.000001,
    ]

    cvode_solve(
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
    state_values = [-63.886, 0.135008, 0.984333, 0.740972]
    state_abs_tols = [0.001, 0.000001, 0.000001, 0.000001]
    rate_values = [49.726, -0.128193, -0.05090, 0.09865]
    rate_abs_tols = [0.001, 0.000001, 0.00001, 0.00001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.9819,
        -823.517,
        789.779,
        3.9699,
        0.11499,
        0.002869,
        0.967347,
        0.54133,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.0001,
        0.001,
        0.001,
        0.0001,
        0.00001,
        0.000001,
        0.000001,
        0.00001,
        0.000001,
    ]

    cvode_solve(
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


def test_solve_without_interpolate_solution():
    state_values = [-63.886395, 0.135008, 0.984334, 0.740972]
    state_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
    rate_values = [49.725709, -0.128194, -0.050903, 0.098651]
    rate_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.982019,
        -823.51695,
        789.77946,
        3.969916,
        0.114985,
        0.002869,
        0.967348,
        0.541337,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.000001,
        0.00001,
        0.00001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.interpolate_solution = False

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


def test_solve_with_adams_moulton_integration_method():
    state_values = [-63.89, 0.13501, 0.98434, 0.74097]
    state_abs_tols = [0.01, 0.00001, 0.00001, 0.00001]
    rate_values = [49.726, -0.12820, -0.0509, 0.09866]
    rate_abs_tols = [0.001, 0.00001, 0.0001, 0.00001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.982,
        -823.52,
        789.78,
        3.97,
        0.11498,
        0.002869,
        0.96735,
        0.5413,
        0.056245,
    ]
    algebraic_abs_tols = [
        0.0,
        0.001,
        0.01,
        0.01,
        0.01,
        0.00001,
        0.000001,
        0.00001,
        0.0001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.integration_method = SolverCvode.IntegrationMethod.AdamsMoulton

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


def test_solve_with_functional_iteration_type():
    state_values = [-63.886, 0.13501, 0.984334, 0.740972]
    state_abs_tols = [0.001, 0.00001, 0.000001, 0.000001]
    rate_values = [49.726, -0.12820, -0.05090, 0.09865]
    rate_abs_tols = [0.001, 0.00001, 0.00001, 0.00001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.982,
        -823.517,
        789.78,
        3.9699,
        0.11499,
        0.002869,
        0.96735,
        0.54134,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.001,
        0.001,
        0.01,
        0.0001,
        0.00001,
        0.000001,
        0.00001,
        0.00001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.iteration_type = SolverCvode.IterationType.Functional

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


def test_solve_with_banded_linear_solver():
    state_values = [-54.958, 0.114716, 0.971365, 0.756967]
    state_abs_tols = [0.001, 0.000001, 0.000001, 0.000001]
    rate_values = [47.196, -0.10208, -0.09298, 0.06298]
    rate_abs_tols = [0.001, 0.00001, 0.00001, 0.00001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -13.3035,
        -791.43,
        757.55,
        3.1535,
        0.18882,
        0.004484,
        0.92385,
        0.45465,
        0.062887,
    ]
    algebraic_abs_tols = [
        0.0,
        0.0001,
        0.01,
        0.01,
        0.0001,
        0.00001,
        0.000001,
        0.00001,
        0.00001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Banded

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


def test_solve_with_diagonal_linear_solver():
    state_values = [-63.886, 0.13501, 0.984334, 0.740971]
    state_abs_tols = [0.001, 0.00001, 0.000001, 0.000001]
    rate_values = [49.725, -0.128193, -0.05090, 0.09865]
    rate_abs_tols = [0.001, 0.000001, 0.00001, 0.00001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.982,
        -823.52,
        789.78,
        3.9699,
        0.11498,
        0.002869,
        0.96735,
        0.54134,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.001,
        0.01,
        0.01,
        0.0001,
        0.00001,
        0.000001,
        0.00001,
        0.00001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Diagonal

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


def test_solve_with_gmres_linear_solver():
    state_values = [9.518468, 0.367366, 0.016120, 0.491145]
    state_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
    rate_values = [-0.864067, 0.064354, 0.001718, -0.052716]
    rate_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        6.03944,
        -5.198281,
        0.022996,
        0.112958,
        6.787605,
        0.112665,
        0.018857,
        0.032306,
        0.140794,
    ]
    algebraic_abs_tols = [
        0.0,
        0.00001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Gmres

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


def test_solve_with_bicgstab_linear_solver():
    state_values = [9.518406, 0.367370, 0.016121, 0.491141]
    state_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
    rate_values = [-0.864079, 0.064354, 0.001718, -0.052715]
    rate_abs_tols = [0.000001, 0.000001, 0.000001, 0.000001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        6.039422,
        -5.198258,
        0.022996,
        0.112959,
        6.787581,
        0.112665,
        0.018857,
        0.032306,
        0.140793,
    ]
    algebraic_abs_tols = [
        0.0,
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

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Bicgstab

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


def test_solve_with_tfqmr_linear_solver():
    state_values = [9.5181, 0.367396, 0.016121, 0.491122]
    state_abs_tols = [0.0001, 0.000001, 0.000001, 0.000001]
    rate_values = [-0.86414, 0.064349, 0.001718, -0.052712]
    rate_abs_tols = [0.00001, 0.000001, 0.000001, 0.000001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        6.03933,
        -5.1981,
        0.023001,
        0.112961,
        6.78747,
        0.112663,
        0.018857,
        0.032307,
        0.140793,
    ]
    algebraic_abs_tols = [
        0.0,
        0.00001,
        0.0001,
        0.000001,
        0.000001,
        0.00001,
        0.000001,
        0.000001,
        0.000001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Tfqmr

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


def test_solve_with_gmres_linear_solver_and_no_preconditioner():
    state_values = [-63.887, 0.135009, 0.984334, 0.740971]
    state_abs_tols = [0.001, 0.000001, 0.000001, 0.000001]
    rate_values = [49.7259, -0.128194, -0.05090, 0.098651]
    rate_abs_tols = [0.0001, 0.000001, 0.00001, 0.000001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.9821,
        -823.517,
        789.780,
        3.9700,
        0.114985,
        0.002869,
        0.967348,
        0.54134,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.0001,
        0.001,
        0.001,
        0.0001,
        0.000001,
        0.000001,
        0.000001,
        0.00001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Gmres
    solver.preconditioner = SolverCvode.Preconditioner.No

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


def test_solve_with_bicgstab_linear_solver_and_no_preconditioner():
    state_values = [-63.886, 0.13501, 0.984333, 0.740972]
    state_abs_tols = [0.001, 0.00001, 0.000001, 0.000001]
    rate_values = [49.725, -0.12819, -0.050904, 0.098649]
    rate_abs_tols = [0.001, 0.00001, 0.000001, 0.000001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.9819,
        -823.516,
        789.779,
        3.9699,
        0.11499,
        0.002869,
        0.96735,
        0.54133,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.0001,
        0.001,
        0.001,
        0.0001,
        0.00001,
        0.000001,
        0.00001,
        0.00001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Bicgstab
    solver.preconditioner = SolverCvode.Preconditioner.No

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


def test_solve_with_tfqmr_linear_solver_and_no_preconditioner():
    state_values = [-63.886, 0.13501, 0.984333, 0.740972]
    state_abs_tols = [0.001, 0.00001, 0.000001, 0.000001]
    rate_values = [49.726, -0.12820, -0.05090, 0.09865]
    rate_abs_tols = [0.001, 0.00001, 0.00001, 0.00001]
    constant_values = [1.0, 0.0, 0.3, 120.0, 36.0]
    constant_abs_tols = [0.0, 0.0, 0.0, 0.0, 0.0]
    computed_constant_values = [-10.613, -115.0, 12.0]
    computed_constant_abs_tols = [0.0, 0.0, 0.0]
    algebraic_values = [
        0.0,
        -15.982,
        -823.52,
        789.779,
        3.9699,
        0.11499,
        0.002869,
        0.96735,
        0.54133,
        0.056246,
    ]
    algebraic_abs_tols = [
        0.0,
        0.001,
        0.01,
        0.001,
        0.0001,
        0.00001,
        0.000001,
        0.00001,
        0.00001,
        0.000001,
    ]

    file = File(utils.resource_path("api/solver/ode.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    solver = simulation.ode_solver

    solver.linear_solver = SolverCvode.LinearSolver.Tfqmr
    solver.preconditioner = SolverCvode.Preconditioner.No

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
