# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License"
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


import libopencor as oc
import utils
from utils import assert_issues


def test_initialise():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
"""

    document = oc.SedDocument()

    assert document.serialise() == expected_serialisation


def test_models():
    document = oc.SedDocument()

    assert document.has_models == False
    assert document.model_count == 0
    assert len(document.models) == 0
    assert document.add_model(None) == False

    file = oc.File(utils.LocalFile)
    model = oc.SedModel(document, file)

    assert model.file == file

    assert document.add_model(model) == True

    assert document.has_models == True
    assert document.model_count == 1
    assert len(document.models) == 1
    assert document.models[0] == model
    assert document.model(0) == model
    assert document.model(1) == None

    assert document.add_model(model) == False
    assert document.remove_model(model) == True

    assert document.has_models == False
    assert document.model_count == 0
    assert len(document.models) == 0

    assert document.remove_model(None) == False


def test_simulations():
    document = oc.SedDocument()

    assert document.has_simulations == False
    assert document.simulation_count == 0
    assert len(document.simulations) == 0
    assert document.add_simulation(None) == False

    uniformTimeCourse = oc.SedUniformTimeCourse(document)
    oneStep = oc.SedOneStep(document)
    steadyState = oc.SedSteadyState(document)
    analysis = oc.SedAnalysis(document)

    assert document.add_simulation(uniformTimeCourse) == True
    assert document.add_simulation(oneStep) == True
    assert document.add_simulation(steadyState) == True
    assert document.add_simulation(analysis) == True

    assert document.has_simulations == True
    assert document.simulation_count == 4
    assert len(document.simulations) == 4
    assert document.simulations[0] == uniformTimeCourse
    assert document.simulations[1] == oneStep
    assert document.simulations[2] == steadyState
    assert document.simulations[3] == analysis
    assert document.simulation(0) == uniformTimeCourse
    assert document.simulation(1) == oneStep
    assert document.simulation(2) == steadyState
    assert document.simulation(3) == analysis
    assert document.simulation(4) == None

    assert document.add_simulation(uniformTimeCourse) == False
    assert document.remove_simulation(uniformTimeCourse) == True

    assert document.add_simulation(oneStep) == False
    assert document.remove_simulation(oneStep) == True

    assert document.add_simulation(steadyState) == False
    assert document.remove_simulation(steadyState) == True

    assert document.add_simulation(analysis) == False
    assert document.remove_simulation(analysis) == True

    assert document.has_simulations == False
    assert document.simulation_count == 0
    assert len(document.simulations) == 0

    assert document.remove_simulation(None) == False


def sed_task_expected_serialisation(with_properties):
    properties = (
        ' modelReference="model1" simulationReference="simulation1"'
        if with_properties
        else ""
    )

    return f"""<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfTasks>
    <task id="task1"{properties}/>
  </listOfTasks>
</sedML>
"""


def test_tasks():
    document = oc.SedDocument()

    assert document.has_tasks == False
    assert document.task_count == 0
    assert len(document.tasks) == 0
    assert document.add_task(None) == False

    file = oc.File(utils.LocalFile)
    model = oc.SedModel(document, file)
    simulation = oc.SedUniformTimeCourse(document)
    task = oc.SedTask(document, model, simulation)

    assert task.model != None
    assert task.simulation != None

    assert document.add_task(task) == True

    assert document.has_tasks == True
    assert document.task_count == 1
    assert len(document.tasks) == 1
    assert document.tasks[0] == task
    assert document.task(0) == task
    assert document.task(1) == None

    assert document.serialise() == sed_task_expected_serialisation(True)

    task.model = None
    task.simulation = None

    assert task.model == None
    assert task.simulation == None

    assert document.serialise() == sed_task_expected_serialisation(False)

    expected_issues = [
        [
            oc.Issue.Type.Error,
            "Task 'task1' requires a model.",
        ],
        [
            oc.Issue.Type.Error,
            "Task 'task1' requires a simulation.",
        ],
    ]

    instance = document.instantiate()

    assert_issues(instance, expected_issues)

    assert document.add_task(task) == False
    assert document.remove_task(task) == True

    assert document.has_tasks == False
    assert document.task_count == 0
    assert len(document.tasks) == 0

    assert document.remove_task(None) == False


def test_ode_solver():
    document = oc.SedDocument()
    simulation = oc.SedUniformTimeCourse(document)

    assert simulation.ode_solver == None

    solver = oc.SolverCvode()

    simulation.ode_solver = solver

    assert simulation.ode_solver == solver

    simulation.ode_solver = None

    assert simulation.ode_solver == None


def test_nla_solver():
    document = oc.SedDocument()
    simulation = oc.SedUniformTimeCourse(document)

    assert simulation.nla_solver == None

    solver = oc.SolverKinsol()

    simulation.nla_solver = solver

    assert simulation.nla_solver == solver

    simulation.nla_solver = None

    assert simulation.nla_solver == None


def test_sed_one_step():
    file = oc.File(utils.resource_path(utils.Cellml2File))
    document = oc.SedDocument(file)
    simulation = oc.SedOneStep(document)

    assert simulation.step == 1.0

    simulation.step = 1.23

    assert simulation.step == 1.23


def test_sed_uniform_time_course():
    file = oc.File(utils.resource_path(utils.Cellml2File))
    document = oc.SedDocument(file)
    simulation = oc.SedUniformTimeCourse(document)

    assert simulation.initial_time == 0.0
    assert simulation.output_start_time == 0.0
    assert simulation.output_end_time == 1000.0
    assert simulation.number_of_steps == 1000

    simulation.initial_time = 1.23
    simulation.output_start_time = 4.56
    simulation.output_end_time = 7.89
    simulation.number_of_steps = 10

    assert simulation.initial_time == 1.23
    assert simulation.output_start_time == 4.56
    assert simulation.output_end_time == 7.89
    assert simulation.number_of_steps == 10


def test_sed_instance_and_sed_instance_task():
    expected_issues = [
        [
            oc.Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
        ],
    ]

    file = oc.File(utils.resource_path("api/solver/ode.cellml"))
    document = oc.SedDocument(file)
    solver = document.simulations[0].ode_solver

    solver.linear_solver = oc.SolverCvode.LinearSolver.Banded
    solver.upper_half_bandwidth = -1

    instance = document.instantiate()
    instance_task = instance.tasks[0]

    assert instance.has_tasks == True
    assert instance.task_count == 1
    assert instance.task(0) == instance_task
    assert instance.task(1) == None

    assert instance_task.voi == []
    assert instance_task.voi_name == "environment/time"
    assert instance_task.voi_unit == "millisecond"

    assert instance_task.state_count == 4
    assert instance_task.state(0) == []
    assert instance_task.state(4) == []
    assert instance_task.state_name(0) == "membrane/V"
    assert instance_task.state_name(4) == ""
    assert instance_task.state_unit(0) == "millivolt"
    assert instance_task.state_unit(4) == ""

    assert instance_task.rate_count == 4
    assert instance_task.rate(0) == []
    assert instance_task.rate(4) == []
    assert instance_task.rate_name(0) == "membrane/V'"
    assert instance_task.rate_name(4) == ""
    assert instance_task.rate_unit(0) == "millivolt/millisecond"
    assert instance_task.rate_unit(4) == ""

    assert instance_task.constant_count == 5
    assert instance_task.constant(0) == []
    assert instance_task.constant(5) == []
    assert instance_task.constant_name(0) == "membrane/Cm"
    assert instance_task.constant_name(5) == ""
    assert instance_task.constant_unit(0) == "microF_per_cm2"
    assert instance_task.constant_unit(5) == ""

    assert instance_task.computed_constant_count == 3
    assert instance_task.computed_constant(0) == []
    assert instance_task.computed_constant(3) == []
    assert instance_task.computed_constant_name(0) == "leakage_current/E_L"
    assert instance_task.computed_constant_name(3) == ""
    assert instance_task.computed_constant_unit(0) == "millivolt"
    assert instance_task.computed_constant_unit(3) == ""

    assert instance_task.algebraic_count == 10
    assert instance_task.algebraic(0) == []
    assert instance_task.algebraic(10) == []
    assert instance_task.algebraic_name(0) == "membrane/i_Stim"
    assert instance_task.algebraic_name(10) == ""
    assert instance_task.algebraic_unit(0) == "microA_per_cm2"
    assert instance_task.algebraic_unit(10) == ""

    instance.run()

    assert_issues(instance, expected_issues)


def test_sed_document():
    file = oc.File(utils.resource_path(utils.HttpRemoteCellmlFile))
    oc.SedDocument(file)

    file = oc.File(utils.resource_path(utils.HttpRemoteSedmlFile))
    oc.SedDocument(file)

    file = oc.File(utils.resource_path(utils.HttpRemoteCombineArchive))
    oc.SedDocument(file)


def test_solver():
    # Get the duplicate() method of different solvers to be covered.

    file = oc.File(utils.resource_path(utils.Cellml2File))
    document = oc.SedDocument(file)

    document.simulations[0].ode_solver = oc.SolverForwardEuler()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False

    document.simulations[0].ode_solver = oc.SolverFourthOrderRungeKutta()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False

    document.simulations[0].ode_solver = oc.SolverHeun()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False

    document.simulations[0].ode_solver = oc.SolverSecondOrderRungeKutta()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False
