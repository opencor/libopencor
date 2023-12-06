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
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)

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
