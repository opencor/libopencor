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


import libopencor as loc
import platform
import utils
from utils import assert_issues


def test_initialise():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
"""

    document = loc.SedDocument()

    assert document.serialise() == expected_serialisation


def test_models():
    document = loc.SedDocument()

    assert document.has_models == False
    assert document.model_count == 0
    assert len(document.models) == 0
    assert document.add_model(None) == False

    file = loc.File(utils.LocalFile)
    model = loc.SedModel(document, file)

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
    assert document.remove_all_models() == False

    assert document.add_model(model) == True
    assert document.remove_all_models() == True

    assert model.has_changes == False
    assert model.change_count == 0
    assert len(model.changes) == 0
    assert model.add_change(None) == False
    assert model.remove_all_changes() == False

    change_attribute = loc.SedChangeAttribute("component", "variable", "newValue")

    assert model.add_change(change_attribute) == True

    assert model.has_changes == True
    assert model.change_count == 1
    assert len(model.changes) == 1
    assert model.changes[0] == change_attribute
    assert model.change(0) == change_attribute
    assert model.change(1) == None

    assert model.add_change(change_attribute) == False
    assert model.remove_change(change_attribute) == True

    assert model.add_change(change_attribute) == True
    assert model.remove_all_changes() == True

    assert model.has_changes == False
    assert model.change_count == 0
    assert len(model.changes) == 0

    assert model.remove_change(None) == False


def sed_change_expected_serialisation(component, variable, new_value):
    source = (
        "file:///P:/some/path/file.txt"
        if platform.system() == "Windows"
        else "file:///some/path/file.txt"
    )
    return f"""<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="{source}">
      <listOfChanges>
        <changeAttribute target="/cellml:model/cellml:component[@name='{component}']/cellml:variable[@name='{variable}']" newValue="{new_value}"/>
      </listOfChanges>
    </model>
  </listOfModels>
</sedML>
"""


def test_changes():
    change_attribute = loc.SedChangeAttribute("component", "variable", "123.456789")

    assert (
        change_attribute.target
        == "/cellml:model/cellml:component[@name='component']/cellml:variable[@name='variable']"
    )
    assert change_attribute.component == "component"
    assert change_attribute.variable == "variable"
    assert change_attribute.new_value == "123.456789"

    document = loc.SedDocument()
    file = loc.File(utils.LocalFile)
    model = loc.SedModel(document, file)

    assert model.add_change(change_attribute) == True
    assert document.add_model(model) == True

    assert document.serialise() == sed_change_expected_serialisation(
        "component", "variable", "123.456789"
    )

    change_attribute.component = "new_component"
    change_attribute.variable = "new_variable"
    change_attribute.new_value = "987.654321"

    assert (
        change_attribute.target
        == "/cellml:model/cellml:component[@name='new_component']/cellml:variable[@name='new_variable']"
    )
    assert change_attribute.component == "new_component"
    assert change_attribute.variable == "new_variable"
    assert change_attribute.new_value == "987.654321"

    assert document.serialise() == sed_change_expected_serialisation(
        "new_component", "new_variable", "987.654321"
    )


def test_simulations():
    document = loc.SedDocument()

    assert document.has_simulations == False
    assert document.simulation_count == 0
    assert len(document.simulations) == 0
    assert document.add_simulation(None) == False

    uniformTimeCourse = loc.SedUniformTimeCourse(document)
    oneStep = loc.SedOneStep(document)
    steadyState = loc.SedSteadyState(document)
    analysis = loc.SedAnalysis(document)

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
    assert document.remove_all_simulations() == False

    assert document.add_simulation(uniformTimeCourse) == True
    assert document.remove_all_simulations() == True


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
    document = loc.SedDocument()

    assert document.has_tasks == False
    assert document.task_count == 0
    assert len(document.tasks) == 0
    assert document.add_task(None) == False

    file = loc.File(utils.LocalFile)
    model = loc.SedModel(document, file)
    simulation = loc.SedUniformTimeCourse(document)
    task = loc.SedTask(document, model, simulation)

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
            loc.Issue.Type.Error,
            "Task 'task1' requires a model.",
        ],
        [
            loc.Issue.Type.Error,
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
    assert document.remove_all_tasks() == False

    assert document.add_task(task) == True
    assert document.remove_all_tasks() == True


def test_ode_solver():
    document = loc.SedDocument()
    simulation = loc.SedUniformTimeCourse(document)

    assert simulation.ode_solver == None

    solver = loc.SolverCvode()

    simulation.ode_solver = solver

    assert simulation.ode_solver == solver

    simulation.ode_solver = None

    assert simulation.ode_solver == None


def test_nla_solver():
    document = loc.SedDocument()
    simulation = loc.SedUniformTimeCourse(document)

    assert simulation.nla_solver == None

    solver = loc.SolverKinsol()

    simulation.nla_solver = solver

    assert simulation.nla_solver == solver

    simulation.nla_solver = None

    assert simulation.nla_solver == None


def test_sed_one_step():
    file = loc.File(utils.resource_path(utils.Cellml2File))
    document = loc.SedDocument(file)
    simulation = loc.SedOneStep(document)

    assert simulation.step == 1.0

    simulation.step = 1.23

    assert simulation.step == 1.23


def test_sed_uniform_time_course():
    file = loc.File(utils.resource_path(utils.Cellml2File))
    document = loc.SedDocument(file)
    simulation = loc.SedUniformTimeCourse(document)

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
            loc.Issue.Type.Error,
            "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
        ],
    ]

    file = loc.File(utils.resource_path("api/solver/ode.cellml"))
    document = loc.SedDocument(file)
    solver = document.simulations[0].ode_solver

    solver.linear_solver = loc.SolverCvode.LinearSolver.Banded
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
    file = loc.File(utils.resource_path(utils.HttpRemoteCellmlFile))
    loc.SedDocument(file)

    file = loc.File(utils.resource_path(utils.HttpRemoteSedmlFile))
    loc.SedDocument(file)

    file = loc.File(utils.resource_path(utils.HttpRemoteCombineArchive))
    loc.SedDocument(file)


def test_solver():
    # Get the duplicate() method of different solvers to be covered.

    file = loc.File(utils.resource_path(utils.Cellml2File))
    document = loc.SedDocument(file)

    document.simulations[0].ode_solver = loc.SolverForwardEuler()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False

    document.simulations[0].ode_solver = loc.SolverFourthOrderRungeKutta()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False

    document.simulations[0].ode_solver = loc.SolverHeun()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False

    document.simulations[0].ode_solver = loc.SolverSecondOrderRungeKutta()

    instance = document.instantiate()

    instance.run()

    assert instance.has_issues == False
