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
import time
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
    assert instance.progress == 0.0


def test_invalid_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Model: the CellML file is invalid.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Model | CellML | Analyser: equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS).",
        ],
    ]

    file = loc.File(utils.resource_path("error.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_overconstrained_cellml_file():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Model: the CellML file is overconstrained.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Model | CellML | Analyser: variable 'x' in component 'my_component' is overconstrained.",
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
            "Task | Model: the CellML file is underconstrained.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown.",
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
            "Task | Model: the CellML file is unsuitably constrained.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Model | CellML | Analyser: variable 'y' in component 'my_component' is overconstrained.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unsuitably_constrained.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def run_algebraic_model():
    file = loc.File(utils.resource_path("api/sed/algebraic.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


def test_algebraic_model():
    run_algebraic_model()


def test_asynchronous_run_without_active_run():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.is_running is False
    assert instance.wait_for_run() == 0.0


def test_asynchronous_run_lifecycle():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(200):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.is_running is False
    assert instance.wait_for_run() > 0.0
    assert not instance.has_issues


def test_asynchronous_run_can_be_restarted():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.start_run() is True
    assert instance.wait_for_run() > 0.0
    assert not instance.has_issues

    assert instance.start_run() is True
    assert instance.wait_for_run() > 0.0
    assert not instance.has_issues


def test_progress_before_any_run():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.progress == 0.0
    assert instance.tasks[0].progress == 0.0


def test_progress_of_algebraic_model():
    file = loc.File(utils.resource_path("api/sed/algebraic.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.progress == 0.0

    instance.run()

    assert instance.progress == 1.0
    assert instance.tasks[0].progress == 1.0
    assert not instance.has_issues


def test_progress_of_ode_model():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.progress == 0.0

    instance.run()

    assert instance.progress == 1.0
    assert instance.tasks[0].progress == 1.0
    assert not instance.has_issues


def test_stop_run():
    large_step_count = 1000000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    simulation.number_of_steps = large_step_count
    simulation.output_end_time = float(large_step_count)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(60000):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.stop_run()

    for _ in range(60000):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.progress < 1.0
    assert not instance.has_issues


def test_stop_run_when_not_running():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    # Calling stop_run() when idle is a no-op.

    instance.stop_run()

    assert instance.is_running is False
    assert instance.progress == 0.0


def test_pause_run_and_resume_run():
    large_step_count = 1000000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = large_step_count
    simulation.output_end_time = float(large_step_count)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(60000):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.pause_run()

    time.sleep(0.05)

    instance.resume_run()
    instance.stop_run()

    for _ in range(60000):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.is_running is False
    assert instance.progress < 1.0
    assert not instance.has_issues


def test_pause_run_and_resume_run_when_not_running():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.pause_run()
    instance.resume_run()

    assert instance.is_running is False
    assert instance.progress == 0.0


def test_pause_run_then_stop_run():
    large_step_count = 1000000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = large_step_count
    simulation.output_end_time = float(large_step_count)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(60000):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.pause_run()

    time.sleep(0.05)

    instance.stop_run()

    for _ in range(60000):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.is_running is False
    assert instance.progress < 1.0
    assert not instance.has_issues


def test_pause_run_and_resume_run_with_natural_completion():
    moderate_step_count = 50000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = moderate_step_count
    simulation.output_end_time = float(moderate_step_count)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(60000):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.pause_run()

    time.sleep(0.05)

    instance.resume_run()

    for _ in range(60000):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.is_running is False
    assert instance.wait_for_run() > 0.0
    assert not instance.has_issues


def test_start_run_while_already_running():
    large_step_count = 1000000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = large_step_count
    simulation.output_end_time = float(large_step_count)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(60000):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    assert instance.start_run() is False

    instance.stop_run()

    for _ in range(60000):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.is_running is False
    assert instance.progress < 1.0
    assert not instance.has_issues


def test_start_run_after_previous_run_completed():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(60000):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.is_running is False

    assert instance.start_run() is True

    for _ in range(60000):
        if not instance.is_running:
            break

        time.sleep(0.001)

    assert instance.is_running is False
    assert instance.wait_for_run() > 0.0
    assert not instance.has_issues


def run_ode_model():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            (
                "Task | CVODE: at t = 0.00140013827899707, mxstep steps taken before reaching tout."
                if platform.system() == "Darwin"
                else "Task | CVODE: at t = 0.00140013827899996, mxstep steps taken before reaching tout."
            ),
        ],
    ]

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    cvode = simulation.ode_solver

    cvode.maximum_number_of_steps = 10

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert_issues(instance, expected_issues)

    cvode.maximum_number_of_steps = 500

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


def test_ode_model():
    run_ode_model()


def test_ode_model_with_no_ode_solver():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
        ],
    ]

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)

    document.simulations[0].ode_solver = None

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_nla_model():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
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

    assert not instance.has_issues


def test_nla_model_with_no_nla_solver():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/nla.cellml"))
    document = loc.SedDocument(file)

    document.simulations[0].nla_solver = None

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_dae_model():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0.",
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

    assert not instance.has_issues


def test_dae_model_with_no_ode_or_nla_solver():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided.",
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
    file = loc.File(utils.resource_path("cellml_2.omex"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


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

    assert not instance.has_issues


def test_dae_model_from_cellml_file():
    file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    document = loc.SedDocument(file)

    assert not document.has_issues

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert not instance.has_issues


def test_dae_model_from_sedml_file():
    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(utils.resource_path("api/sed/dae/model.sedml"))
    document = loc.SedDocument(sedml_file)

    assert not document.has_issues

    nla_solver = document.simulations[0].nla_solver

    assert nla_solver.linear_solver == loc.SolverKinsol.LinearSolver.Gmres
    assert nla_solver.maximum_number_of_iterations == 123
    assert nla_solver.upper_half_bandwidth == 1
    assert nla_solver.lower_half_bandwidth == 1

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert not instance.has_issues


def test_dae_model_from_combine_archive():
    file = loc.File(utils.resource_path("api/sed/dae/model.omex"))
    document = loc.SedDocument(file)

    assert not document.has_issues

    nla_solver = document.simulations[0].nla_solver

    assert nla_solver.linear_solver == loc.SolverKinsol.LinearSolver.Gmres
    assert nla_solver.maximum_number_of_iterations == 123
    assert nla_solver.upper_half_bandwidth == 1
    assert nla_solver.lower_half_bandwidth == 1

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert not instance.has_issues


def test_dae_model_from_legacy_sedml_file():
    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(utils.resource_path("api/sed/dae/model_legacy.sedml"))
    document = loc.SedDocument(sedml_file)

    assert not document.has_issues

    nla_solver = document.simulations[0].nla_solver

    assert nla_solver.linear_solver == loc.SolverKinsol.LinearSolver.Gmres
    assert nla_solver.maximum_number_of_iterations == 123
    assert nla_solver.upper_half_bandwidth == 1
    assert nla_solver.lower_half_bandwidth == 1

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert not instance.has_issues


def test_dae_model_from_legacy_combine_archive():
    file = loc.File(utils.resource_path("api/sed/dae/model_legacy.omex"))
    document = loc.SedDocument(file)

    assert not document.has_issues

    nla_solver = document.simulations[0].nla_solver

    assert nla_solver.linear_solver == loc.SolverKinsol.LinearSolver.Gmres
    assert nla_solver.maximum_number_of_iterations == 123
    assert nla_solver.upper_half_bandwidth == 1
    assert nla_solver.lower_half_bandwidth == 1

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert not instance.has_issues


def test_simulation_with_initial_time():
    file = loc.File(utils.resource_path("api/sed/simulation_with_initial_time.omex"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert not instance.has_issues

    instance_task = instance.tasks[0]
    voi = instance_task.voi

    assert len(voi) == 50001
    assert voi[0] == 0.0
    assert voi[50000] == 50.0

    x = instance_task.state(0)
    y = instance_task.state(1)
    z = instance_task.state(2)

    assert len(x) == 50001
    assert len(y) == 50001
    assert len(z) == 50001

    assert x[0] != 1.0
    assert y[0] != 1.0
    assert z[0] != 1.0


def test_simulation_with_initial_time_failing():
    file = loc.File(
        utils.resource_path("api/sed/simulation_with_initial_time_failing.omex")
    )
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert instance.has_issues
