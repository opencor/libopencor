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


from libopencor import File, Issue, SedDocument, SolverKinsol
import platform
import utils
from utils import assert_issues


def test_no_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "The simulation experiment description does not contain any tasks to run.",
        ],
    ]

    document = SedDocument()
    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_invalid_cellml_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "The CellML file is invalid.",
        ],
        [
            Issue.Type.Error,
            "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS).",
        ],
    ]

    file = File(utils.resource_path(utils.ERROR_CELLML_FILE))
    document = SedDocument(file)
    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_overconstrained_cellml_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "The CellML file is overconstrained.",
        ],
        [
            Issue.Type.Error,
            "Variable 'x' in component 'my_component' is computed more than once.",
        ],
    ]

    file = File(utils.resource_path("api/sed/overconstrained.cellml"))
    document = SedDocument(file)
    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_underconstrained_cellml_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "The CellML file is underconstrained.",
        ],
        [
            Issue.Type.Error,
            "The type of variable 'x' in component 'my_component' is unknown.",
        ],
    ]

    file = File(utils.resource_path("api/sed/underconstrained.cellml"))
    document = SedDocument(file)
    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_unsuitable_constrained_cellml_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "The CellML file is unsuitably constrained.",
        ],
        [
            Issue.Type.Error,
            "Variable 'y' in component 'my_component' is computed more than once.",
        ],
        [
            Issue.Type.Error,
            "The type of variable 'x' in component 'my_component' is unknown.",
        ],
    ]

    file = File(utils.resource_path("api/sed/unsuitably_constrained.cellml"))
    document = SedDocument(file)
    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def run_algebraic_model(compiled):
    file = File(utils.resource_path("api/sed/algebraic.cellml"))
    document = SedDocument(file)
    instance = document.create_instance(compiled)

    instance.run()

    assert instance.has_issues == False


def test_compiled_algebraic_model():
    run_algebraic_model(True)


def test_interpreted_algebraic_model():
    run_algebraic_model(False)


def run_ode_model(compiled):
    expected_issues = [
        [
            Issue.Type.Error,
            (
                "At t = 0.00140014, mxstep steps taken before reaching tout."
                if platform.system() == "Windows"
                else (
                    "At t = 1, mxstep steps taken before reaching tout."
                    if platform.system() == "Linux"
                    else (
                        "At t = 5.00577e+06, mxstep steps taken before reaching tout."
                        if platform.machine() == "x86_64"
                        else "At t = 0.00140014, mxstep steps taken before reaching tout."
                    )
                )
            ),
        ],
    ]

    file = File(utils.resource_path(utils.CELLML_2_FILE))
    document = SedDocument(file)
    simulation = document.simulations[0]
    cvode = simulation.ode_solver

    cvode.maximum_number_of_steps = 10

    instance = document.create_instance(compiled)

    assert instance.has_issues == False

    instance.run()

    assert_issues(instance, expected_issues)

    cvode.maximum_number_of_steps = 500

    instance = document.create_instance(compiled)

    instance.run()

    assert instance.has_issues == False


def test_compiled_ode_model():
    run_ode_model(True)


def test_interpreted_ode_model():
    run_ode_model(False)


def test_ode_model_with_no_ode_solver():
    expected_issues = [
        [
            Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
        ],
    ]

    file = File(utils.resource_path(utils.CELLML_2_FILE))
    document = SedDocument(file)

    document.simulations[0].ode_solver = None

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


# ---GRY--- AS FOR THE ALGEBRAIC AND ODE MODELS, WE WILL NEED TO ADD AN INTERPRETED VERSION OF THIS TEST.


def test_nla_model():
    expected_issues = [
        [
            Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
        ],
    ]

    file = File(utils.resource_path("api/sed/nla.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    kinsol = simulation.nla_solver

    kinsol.linear_solver = SolverKinsol.LinearSolver.Banded
    kinsol.upper_half_bandwidth = -1

    instance = document.create_instance()

    assert_issues(instance, expected_issues)

    kinsol.linear_solver = SolverKinsol.LinearSolver.Dense

    instance = document.create_instance()

    assert instance.has_issues == False


def test_nla_model_with_no_nla_solver():
    expected_issues = [
        [
            Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
        ],
    ]

    file = File(utils.resource_path("api/sed/nla.cellml"))
    document = SedDocument(file)

    document.simulations[0].nla_solver = None

    instance = document.create_instance()

    assert_issues(instance, expected_issues)


def test_dae_model():
    expected_issues = [
        [
            Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
        ],
    ]

    file = File(utils.resource_path("api/sed/dae.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]
    kinsol = simulation.nla_solver

    kinsol.linear_solver = SolverKinsol.LinearSolver.Banded
    kinsol.upper_half_bandwidth = -1

    instance = document.create_instance()

    assert_issues(instance, expected_issues)

    instance.run()

    assert_issues(instance, expected_issues)

    kinsol.linear_solver = SolverKinsol.LinearSolver.Dense

    instance = document.create_instance()

    instance.run()

    assert instance.has_issues == False


def test_dae_model_with_no_ode_or_nla_solver():
    expected_issues = [
        [
            Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
        ],
        [
            Issue.Type.Error,
            "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
        ],
    ]

    file = File(utils.resource_path("api/sed/dae.cellml"))
    document = SedDocument(file)
    simulation = document.simulations[0]

    simulation.ode_solver = None
    simulation.nla_solver = None

    instance = document.create_instance()

    assert_issues(instance, expected_issues)
