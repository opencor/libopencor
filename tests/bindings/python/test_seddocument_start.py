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


from libopencor import File, Issue, SedDocument
import utils
from utils import assert_issues


def test_no_file():
    expected_issues = [
        [
            Issue.Type.Error,
            "A simulation experiment description must (currently) have exactly one model.",
        ],
        [
            Issue.Type.Error,
            "A simulation experiment description must (currently) have exactly one simulation.",
        ],
    ]

    sed = SedDocument()

    assert sed.start() == False
    assert_issues(sed, expected_issues)


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
    sed = SedDocument(file)

    assert sed.start() == False
    assert_issues(sed, expected_issues)


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
    sed = SedDocument(file)

    assert sed.start() == False
    assert_issues(sed, expected_issues)


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
    sed = SedDocument(file)

    assert sed.start() == False
    assert_issues(sed, expected_issues)


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
    sed = SedDocument(file)

    assert sed.start() == False
    assert_issues(sed, expected_issues)


def test_algebraic_model():
    file = File(utils.resource_path("api/sed/algebraic.cellml"))
    sed = SedDocument(file)

    assert sed.start() == True


def test_ode_model():
    expected_issues = [
        [
            Issue.Type.Error,
            "At t = 0.00140014, mxstep steps taken before reaching tout.",
        ],
    ]

    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)
    simulation = sed.simulations[0]
    cvode = simulation.ode_solver

    cvode.maximum_number_of_steps = 10

    assert sed.start() == False
    assert_issues(sed, expected_issues)

    cvode.maximum_number_of_steps = 500

    assert sed.start() == True


def test_ode_model_with_no_ode_solver():
    expected_issues = [
        [
            Issue.Type.Error,
            "The simulation is to be linked to an ODE model and must therefore specify an ODE solver.",
        ],
    ]

    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)

    sed.simulations[0].ode_solver = None

    assert sed.start() == False
    assert_issues(sed, expected_issues)


def test_nla_model():
    file = File(utils.resource_path("api/sed/nla.cellml"))
    sed = SedDocument(file)

    assert sed.start() == True


def test_nla_model_with_no_nla_solver():
    expected_issues = [
        [
            Issue.Type.Error,
            "The simulation is to be linked to an NLA model and must therefore specify an NLA solver.",
        ],
    ]

    file = File(utils.resource_path("api/sed/nla.cellml"))
    sed = SedDocument(file)

    sed.simulations[0].nla_solver = None

    assert sed.start() == False
    assert_issues(sed, expected_issues)


def test_dae_model():
    file = File(utils.resource_path("api/sed/dae.cellml"))
    sed = SedDocument(file)

    assert sed.start() == True


def test_dae_model_with_no_ode_or_nla_solver():
    expected_issues = [
        [
            Issue.Type.Error,
            "The simulation is to be linked to a DAE model and must therefore specify an ODE solver.",
        ],
        [
            Issue.Type.Error,
            "The simulation is to be linked to a DAE model and must therefore specify an NLA solver.",
        ],
    ]

    file = File(utils.resource_path("api/sed/dae.cellml"))
    sed = SedDocument(file)
    simulation = sed.simulations[0]

    simulation.ode_solver = None
    simulation.nla_solver = None

    assert sed.start() == False
    assert_issues(sed, expected_issues)
