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
import math
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

    # Make sure that the issues are still present after "running" the instance.

    assert instance.run() == 0.0
    assert_issues(instance, expected_issues)


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


def test_cellml_file_with_units_prefix_out_of_range():
    # Note: libCellML handles such a units prefix by catching the std::out_of_range exception thrown by std::stoi(), so
    #       this checks that exceptions can be caught everywhere in libOpenCOR, including in our third-party libraries.

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Model: the CellML file is invalid.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Model | CellML | Analyser: prefix '92233720368547758077876856757465433' of a unit referencing 'second' in units 'my_units' is out of the integer range.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/units_prefix_out_of_range.cellml"))
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

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.wait_for_run() == 0.0


def test_asynchronous_run_lifecycle():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(200):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.status == loc.SedInstance.Status.Idle
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
    SIMULATION_PROPERTY = 1000000
    WAIT_ITERATIONS = 60000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    simulation.number_of_steps = SIMULATION_PROPERTY
    simulation.output_end_time = float(SIMULATION_PROPERTY)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.stop_run()

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.progress < 1.0
    assert not instance.has_issues


def test_stop_run_results_have_nans():
    SIMULATION_PROPERTY = 1000000
    WAIT_ITERATIONS = 60000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    simulation.number_of_steps = SIMULATION_PROPERTY
    simulation.output_end_time = float(SIMULATION_PROPERTY)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.stop_run()

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.progress < 1.0
    assert not instance.has_issues

    instance_task = instance.tasks[0]
    voi = instance_task.voi
    state0 = instance_task.state(0)

    assert len(voi) == len(state0)
    assert len(voi) == SIMULATION_PROPERTY + 1

    assert not math.isnan(voi[0])
    assert not math.isnan(state0[0])

    nan_index = len(voi)

    for i in range(1, len(voi)):
        if math.isnan(state0[i]):
            nan_index = i

            break

    assert nan_index < len(voi)
    assert math.isnan(voi[nan_index])
    assert nan_index < len(voi) - 1


def test_stop_run_when_not_running():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    # Calling stop_run() when idle is a no-op.

    instance.stop_run()

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.progress == 0.0


def test_pause_run_and_resume_run():
    SIMULATION_PROPERTY = 1000000
    WAIT_ITERATIONS = 60000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = SIMULATION_PROPERTY
    simulation.output_end_time = float(SIMULATION_PROPERTY)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.pause_run()

    time.sleep(0.05)

    instance.resume_run()
    instance.stop_run()

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.progress < 1.0
    assert not instance.has_issues


def test_pause_run_and_resume_run_when_not_running():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.pause_run()
    instance.resume_run()

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.progress == 0.0


def test_pause_run_then_stop_run():
    SIMULATION_PROPERTY = 1000000
    WAIT_ITERATIONS = 60000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = SIMULATION_PROPERTY
    simulation.output_end_time = float(SIMULATION_PROPERTY)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.pause_run()

    time.sleep(0.05)

    instance.stop_run()

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.progress < 1.0
    assert not instance.has_issues


def test_pause_run_and_resume_run_with_natural_completion():
    moderate_step_count = 50000
    WAIT_ITERATIONS = 60000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = moderate_step_count
    simulation.output_end_time = float(moderate_step_count)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    instance.pause_run()

    time.sleep(0.05)

    instance.resume_run()

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.wait_for_run() > 0.0
    assert not instance.has_issues


def test_start_run_while_already_running():
    SIMULATION_PROPERTY = 1000000
    WAIT_ITERATIONS = 60000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = SIMULATION_PROPERTY
    simulation.output_end_time = float(SIMULATION_PROPERTY)

    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.progress > 0.0:
            break

        time.sleep(0.001)

    assert instance.start_run() is False

    instance.stop_run()

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.progress < 1.0
    assert not instance.has_issues


def test_start_run_after_previous_run_completed():
    WAIT_ITERATIONS = 60000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.status == loc.SedInstance.Status.Idle

    assert instance.start_run() is True

    for _ in range(WAIT_ITERATIONS):
        if instance.status == loc.SedInstance.Status.Idle:
            break

        time.sleep(0.001)

    assert instance.status == loc.SedInstance.Status.Idle
    assert instance.wait_for_run() > 0.0
    assert not instance.has_issues


def run_ode_model():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | CVODE: at t = 0.00140013827899996, mxstep steps taken before reaching tout.",
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


def test_ode_model_with_non_uniform_time_course_simulation():
    one_step_expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which (currently) requires a uniform time course simulation.",
        ],
    ]
    steady_state_expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation2' is to be used with model 'model1' which (currently) requires a uniform time course simulation.",
        ],
    ]

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument()
    model = loc.SedModel(document, file)
    one_step = loc.SedOneStep(document)
    steady_state = loc.SedSteadyState(document)
    task = loc.SedTask(document, model, one_step)

    one_step.ode_solver = loc.SolverCvode()
    steady_state.ode_solver = loc.SolverCvode()

    document.add_model(model)
    document.add_simulation(one_step)
    document.add_simulation(steady_state)
    document.add_task(task)

    instance = document.instantiate()

    assert_issues(instance, one_step_expected_issues)

    task.simulation = steady_state

    instance = document.instantiate()

    assert_issues(instance, steady_state_expected_issues)


def test_ode_model_with_invalid_number_of_steps():
    zero_steps_expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but 0 is provided.",
        ],
    ]
    negative_steps_expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but -100 is provided.",
        ],
    ]

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = 0

    instance = document.instantiate()

    assert_issues(instance, zero_steps_expected_issues)

    simulation.number_of_steps = -100

    instance = document.instantiate()

    assert_issues(instance, negative_steps_expected_issues)

    # Running the instance should not do anything, but it should still leave it idle.

    assert instance.start_run() is True
    assert instance.wait_for_run() == 0.0
    assert instance.status == loc.SedInstance.Status.Idle
    assert_issues(instance, negative_steps_expected_issues)

    # Make sure that the number of steps is not reported as invalid anymore once it has been fixed.

    simulation.number_of_steps = 1000

    instance = document.instantiate()

    assert not instance.has_issues
    assert instance.run() > 0.0
    assert not instance.has_issues

    # Make sure that an invalid number of steps is reported when running an instance even if it was valid when the
    # instance was created.

    instance_task = instance.tasks[0]

    simulation.number_of_steps = 0

    assert instance.run() == 0.0
    assert_issues(instance, zero_steps_expected_issues)
    assert len(instance_task.voi) == 1001

    simulation.number_of_steps = -100

    assert instance.start_run() is True
    assert instance.wait_for_run() == 0.0
    assert instance.status == loc.SedInstance.Status.Idle
    assert_issues(instance, negative_steps_expected_issues)
    assert len(instance_task.voi) == 1001

    # Make sure that the instance can be run again once the number of steps has been fixed.

    simulation.number_of_steps = 500

    assert instance.run() > 0.0
    assert not instance.has_issues
    assert len(instance_task.voi) == 501


def test_ode_model_with_invalid_times():
    initial_time_expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 5, 0, and 50 are provided.",
        ],
    ]
    output_end_time_expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 0, 5, and 5 are provided.",
        ],
    ]
    times_and_steps_expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires finite times such that initialTime <= outputStartTime < outputEndTime but 0, 5, and 5 are provided.",
        ],
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires a strictly positive number of steps but 0 is provided.",
        ],
    ]

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    # Initial time after the output start time.

    simulation.initial_time = 5.0
    simulation.output_start_time = 0.0
    simulation.output_end_time = 50.0

    instance = document.instantiate()

    assert_issues(instance, initial_time_expected_issues)

    # Output start time equal to the output end time.

    simulation.initial_time = 0.0
    simulation.output_start_time = 5.0
    simulation.output_end_time = 5.0

    instance = document.instantiate()

    assert_issues(instance, output_end_time_expected_issues)

    # Both invalid times and an invalid number of steps.

    simulation.number_of_steps = 0

    instance = document.instantiate()

    assert_issues(instance, times_and_steps_expected_issues)

    # Non-finite times.

    simulation.output_end_time = math.nan
    simulation.number_of_steps = 10

    instance = document.instantiate()

    assert instance.error_count == 1

    simulation.output_end_time = math.inf

    instance = document.instantiate()

    assert instance.error_count == 1

    # Valid times, but then made invalid after instantiation.

    simulation.output_start_time = 0.0
    simulation.output_end_time = 50.0
    simulation.number_of_steps = 50

    instance = document.instantiate()

    assert not instance.has_issues
    assert instance.run() > 0.0
    assert not instance.has_issues
    assert instance.progress == 1.0

    simulation.output_start_time = 5.0
    simulation.output_end_time = 5.0

    assert instance.run() == 0.0
    assert_issues(instance, output_end_time_expected_issues)
    assert instance.progress == 0.0

    # Valid times again.

    simulation.output_start_time = 0.0
    simulation.output_end_time = 50.0

    assert instance.run() > 0.0
    assert not instance.has_issues
    assert instance.progress == 1.0


def test_ode_model_results_reused_when_run_again():
    # Note: our results are returned as zero-copy NumPy arrays, so make sure that they remain valid when running an
    #       instance again with the same number of steps.

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues

    instance_task = instance.tasks[0]
    voi = instance_task.voi
    state = instance_task.state(0)

    instance.run()

    assert not instance.has_issues
    assert (
        voi.__array_interface__["data"][0]
        == instance_task.voi.__array_interface__["data"][0]
    )
    assert (
        state.__array_interface__["data"][0]
        == instance_task.state(0).__array_interface__["data"][0]
    )
    assert voi[-1] == instance_task.voi[-1]
    assert state[-1] == instance_task.state(0)[-1]


def test_ode_model_with_rounding_error_on_output_end_time():
    # Note: with an output start time of -1, an output end time of 0, and 49 steps, the time of the last step is computed
    #       as -1 + 49 * (1 / 49), which is not exactly 0 due to rounding errors. This used to result in an extra step
    #       being taken and its results being written past the end of our results arrays.

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.initial_time = -1.0
    simulation.output_start_time = -1.0
    simulation.output_end_time = 0.0
    simulation.number_of_steps = 49

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert not instance.has_issues
    assert instance.progress == 1.0

    voi = instance.tasks[0].voi

    assert len(voi) == 50
    assert voi[0] == -1.0
    assert voi[-1] == 0.0


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


def test_dae_model_with_non_uniform_time_course_simulation():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which (currently) requires a uniform time course simulation.",
        ],
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
    document = loc.SedDocument()
    model = loc.SedModel(document, file)
    one_step = loc.SedOneStep(document)
    task = loc.SedTask(document, model, one_step)

    document.add_model(model)
    document.add_simulation(one_step)
    document.add_task(task)

    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_dae_model_with_failing_ode_solver():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | CVODE: at t = 1.08537561647883e-09, mxstep steps taken before reaching tout.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/dae.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    cvode = simulation.ode_solver

    cvode.maximum_number_of_steps = 1

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

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
    assert instance.progress == 1.0

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


def test_changes_to_variables_used_to_initialise_other_variables():
    file = loc.File(
        utils.resource_path("api/sed/variables_initialised_using_variables.cellml")
    )
    document = loc.SedDocument(file)
    instance = document.instantiate()
    model = document.model(0)
    instance_task = instance.tasks[0]

    def initial_value(name):
        for i in range(instance_task.state_count):
            if instance_task.state_name(i) == name:
                return instance_task.state(i)[0]

        for i in range(instance_task.constant_count):
            if instance_task.constant_name(i) == name:
                return instance_task.constant(i)[0]

        for i in range(instance_task.computed_constant_count):
            if instance_task.computed_constant_name(i) == name:
                return instance_task.computed_constant(i)[0]

        return math.nan

    def check_initial_values(expected_initial_values):
        instance.run()

        assert not instance.has_issues

        for name, expected_initial_value in expected_initial_values.items():
            assert math.isclose(
                initial_value(name), expected_initial_value, rel_tol=1e-12
            ), name

    # No changes.

    check_initial_values(
        {
            "main/x": 3.0,
            "main/y": 3.0,
            "main/k2": 3.0,
            "main/k3": 3.0,
            "main/cc": 6.0,
            "main/z": 6.0,
            "main/w": 6.0,
            "main/v": 3.0,
            "main/u": 0.005,
            "main/q": 0.005,
            "main/r": 2.0,
        }
    )

    # Change a constant that is used (directly or indirectly) to initialise some variables.

    model.add_change(loc.SedChangeAttribute("main", "k", "5.0"))

    check_initial_values(
        {
            "main/x": 5.0,
            "main/y": 5.0,
            "main/k2": 5.0,
            "main/k3": 5.0,
            "main/cc": 10.0,
            "main/z": 10.0,
            "main/w": 10.0,
            "main/v": 5.0,
            "main/u": 0.005,
            "main/q": 0.005,
            "main/r": 2.0,
        }
    )

    # Change a state that is used (directly or indirectly) to initialise some variables.

    model.add_change(loc.SedChangeAttribute("main", "x", "7.0"))

    check_initial_values(
        {
            "main/x": 7.0,
            "main/y": 7.0,
            "main/k2": 5.0,
            "main/k3": 7.0,
            "main/cc": 14.0,
            "main/z": 14.0,
            "main/w": 14.0,
            "main/v": 5.0,
            "main/u": 0.005,
            "main/q": 0.005,
            "main/r": 2.0,
        }
    )

    # Change a state that is initialised using a computed constant.

    model.add_change(loc.SedChangeAttribute("main", "z", "1.0"))

    check_initial_values(
        {
            "main/x": 7.0,
            "main/y": 7.0,
            "main/k2": 5.0,
            "main/k3": 7.0,
            "main/cc": 14.0,
            "main/z": 1.0,
            "main/w": 1.0,
            "main/v": 5.0,
            "main/u": 0.005,
            "main/q": 0.005,
            "main/r": 2.0,
        }
    )

    # Change a constant that is initialised using a state.

    model.add_change(loc.SedChangeAttribute("main", "k3", "2.0"))

    check_initial_values(
        {
            "main/x": 7.0,
            "main/y": 7.0,
            "main/k2": 5.0,
            "main/k3": 2.0,
            "main/cc": 4.0,
            "main/z": 1.0,
            "main/w": 1.0,
            "main/v": 5.0,
            "main/u": 0.005,
            "main/q": 0.005,
            "main/r": 2.0,
        }
    )

    # Change some constants that are used to initialise some variables in another component and with different units.

    model.add_change(loc.SedChangeAttribute("initialisation", "u_init", "2.0"))
    model.add_change(loc.SedChangeAttribute("initialisation", "p", "4.0"))

    check_initial_values(
        {
            "main/x": 7.0,
            "main/y": 7.0,
            "main/k2": 5.0,
            "main/k3": 2.0,
            "main/cc": 4.0,
            "main/z": 1.0,
            "main/w": 1.0,
            "main/v": 5.0,
            "main/u": 0.002,
            "main/q": 0.004,
            "main/r": 2.0,
        }
    )

    # Change a constant that is used to initialise a variable in the same component but with different units.

    model.add_change(loc.SedChangeAttribute("main", "s", "4.0"))

    check_initial_values(
        {
            "main/x": 7.0,
            "main/y": 7.0,
            "main/k2": 5.0,
            "main/k3": 2.0,
            "main/cc": 4.0,
            "main/z": 1.0,
            "main/w": 1.0,
            "main/v": 5.0,
            "main/u": 0.002,
            "main/q": 0.004,
            "main/r": 4.0,
        }
    )

    # Remove all our changes.

    model.remove_all_changes()

    check_initial_values(
        {
            "main/x": 3.0,
            "main/y": 3.0,
            "main/k2": 3.0,
            "main/k3": 3.0,
            "main/cc": 6.0,
            "main/z": 6.0,
            "main/w": 6.0,
            "main/v": 3.0,
            "main/u": 0.005,
            "main/q": 0.005,
            "main/r": 2.0,
        }
    )
