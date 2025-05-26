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
import platform
import utils
from utils import assert_issues


def test_no_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The simulation experiment description does not contain any tasks to run.",
        ],
    ]

    document = loc.SedDocument()
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_invalid_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The CellML file is invalid.",
        ],
        [
            loc.Issue.Type.Error,
            "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS).",
        ],
    ]

    file = loc.File(utils.resource_path(utils.ErrorCellmlFile))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_overconstrained_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The CellML file is overconstrained.",
        ],
        [
            loc.Issue.Type.Error,
            "Variable 'x' in component 'my_component' is computed more than once.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/overconstrained.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_underconstrained_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The CellML file is underconstrained.",
        ],
        [
            loc.Issue.Type.Error,
            "The type of variable 'x' in component 'my_component' is unknown.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/underconstrained.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_unsuitable_constrained_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The CellML file is unsuitably constrained.",
        ],
        [
            loc.Issue.Type.Error,
            "Variable 'y' in component 'my_component' is computed more than once.",
        ],
        [
            loc.Issue.Type.Error,
            "The type of variable 'x' in component 'my_component' is unknown.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unsuitably_constrained.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def run_algebraic_model(compiled):
    file = loc.File(utils.resource_path("api/sed/algebraic.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate(compiled)

    instance.run()

    assert instance.has_issues == False


def test_compiled_algebraic_model():
    run_algebraic_model(True)


def test_interpreted_algebraic_model():
    run_algebraic_model(False)


def run_ode_model(compiled):
    expected_issues = [
        [
            loc.Issue.Type.Error,
            (
                (
                    "At t = 0.00140013827899707, mxstep steps taken before reaching tout."
                    if compiled
                    else "At t = 0.00140013827900052, mxstep steps taken before reaching tout."
                )
                if platform.system() == "Darwin"
                else "At t = 0.00140013827899996, mxstep steps taken before reaching tout."
            ),
        ],
    ]

    file = loc.File(utils.resource_path(utils.Cellml2File))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    cvode = simulation.ode_solver

    cvode.maximum_number_of_steps = 10

    instance = document.instantiate(compiled)

    assert instance.has_issues == False

    instance.run()

    assert_issues(instance, expected_issues)

    cvode.maximum_number_of_steps = 500

    instance = document.instantiate(compiled)

    instance.run()

    assert instance.has_issues == False


def test_compiled_ode_model():
    run_ode_model(True)


def test_interpreted_ode_model():
    run_ode_model(False)


def test_ode_model_with_no_ode_solver():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
        ],
    ]

    file = loc.File(utils.resource_path(utils.Cellml2File))
    document = loc.SedDocument(file)

    document.simulations[0].ode_solver = None

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_nla_model():
    # ---GRY--- AS FOR THE ALGEBRAIC AND ODE MODELS, WE WILL NEED TO ADD AN INTERPRETED VERSION OF THIS TEST.

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/nla.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    kinsol = simulation.nla_solver

    kinsol.linear_solver = loc.SolverKinsol.LinearSolver.Banded
    kinsol.upper_half_bandwidth = -1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)

    kinsol.linear_solver = loc.SolverKinsol.LinearSolver.Dense

    instance = document.instantiate()

    assert instance.has_issues == False


def test_nla_model_with_no_nla_solver():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/nla.cellml"))
    document = loc.SedDocument(file)

    document.simulations[0].nla_solver = None

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_dae_model():
    # ---GRY--- AS FOR THE ALGEBRAIC AND ODE MODELS, WE WILL NEED TO ADD AN INTERPRETED VERSION OF THIS TEST.

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/dae.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    kinsol = simulation.nla_solver

    kinsol.linear_solver = loc.SolverKinsol.LinearSolver.Banded
    kinsol.upper_half_bandwidth = -1

    instance = document.instantiate()

    assert_issues(instance, expected_issues)

    instance.run()

    assert_issues(instance, expected_issues)

    kinsol.linear_solver = loc.SolverKinsol.LinearSolver.Dense

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False


def test_dae_model_with_no_ode_or_nla_solver():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
        ],
        [
            loc.Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/dae.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.ode_solver = None
    simulation.nla_solver = None

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_combine_archive():
    file = loc.File(utils.resource_path(utils.Combine2Archive))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False


def test_combine_archive_with_cellml_file_as_master_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "A simulation experiment description cannot be created using a COMBINE archive with an unknown master file (only CellML and SED-ML master files are supported).",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/cellml_file_as_master_file.omex"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.has_issues == False
