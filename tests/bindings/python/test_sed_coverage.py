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
import math
import numpy
import platform
import pytest
import utils
from utils import assert_issues, assert_values


def test_initialise():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
"""

    document = loc.SedDocument()

    assert document.serialise() == expected_serialisation


def test_models():
    document = loc.SedDocument()

    assert not document.has_models
    assert document.model_count == 0
    assert len(document.models) == 0
    assert not document.add_model(None)

    file = loc.File(utils.resource_path("file.txt"))
    model = loc.SedModel(document, file)

    assert model.file == file

    assert document.add_model(model)

    assert document.has_models
    assert document.model_count == 1
    assert len(document.models) == 1
    assert document.models[0] == model
    assert document.model(0) == model
    assert document.model(1) is None

    assert not document.add_model(model)
    assert document.remove_model(model)

    assert not document.has_models
    assert document.model_count == 0
    assert len(document.models) == 0

    assert not document.remove_model(None)
    assert not document.remove_all_models()

    assert document.add_model(model)
    assert document.remove_all_models()

    assert not model.has_changes
    assert model.change_count == 0
    assert len(model.changes) == 0
    assert not model.add_change(None)
    assert not model.remove_all_changes()

    change_attribute = loc.SedChangeAttribute("component", "variable", "1.23")

    assert model.add_change(change_attribute)

    assert model.has_changes
    assert model.change_count == 1
    assert len(model.changes) == 1
    assert model.changes[0] == change_attribute
    assert model.change(0) == change_attribute
    assert model.change(1) is None

    assert not model.add_change(change_attribute)
    assert model.remove_change(change_attribute)

    assert model.add_change(change_attribute)
    assert model.remove_all_changes()

    assert not model.has_changes
    assert model.change_count == 0
    assert len(model.changes) == 0

    assert not model.remove_change(None)


def sed_changes_expected_serialisation(source, changes):
    if platform.system() == "Windows":
        source = "/" + utils.forward_slash_path(source)

    model_end = (
        f""">
      <listOfChanges>{changes}      </listOfChanges>
    </model>"""
        if changes
        else "/>"
    )

    return f"""<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model" language="urn:sedml:language:cellml" source="file://{source}"{model_end}
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="50" numberOfSteps="50000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
"""


def test_changes():
    file = loc.File(utils.resource_path("api/sed/sed_changes.omex"))
    document = loc.SedDocument(file)

    assert not document.has_issues
    assert document.serialise() == sed_changes_expected_serialisation(
        file.child_file(1).path,
        """
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='beta']" newValue="3.0"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='rho']" newValue="13.0"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='sigma']" newValue="9.0"/>
""",
    )

    expected_issues_1 = [
        [
            loc.Issue.Type.Error,
            "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target 'invalidTarget'.",
        ],
        [
            loc.Issue.Type.Error,
            "SED-ML file: the new value 'invalidNewValue' for the change of type 'changeAttribute' is not a valid double value.",
        ],
        [
            loc.Issue.Type.Error,
            "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name=''.",
        ],
        [
            loc.Issue.Type.Error,
            "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName'.",
        ],
        [
            loc.Issue.Type.Error,
            "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name=''.",
        ],
        [
            loc.Issue.Type.Error,
            "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName'.",
        ],
        [
            loc.Issue.Type.Error,
            "SED-ML file: the component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName']Invalid'.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/invalid_sed_changes.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues_1)
    assert document.serialise() == sed_changes_expected_serialisation(
        file.child_file(1).path,
        """
        <changeAttribute target="/cellml:model/cellml:component[@name='componentName']/cellml:variable[@name='variableName']" newValue="1.23"/>
""",
    )

    expected_issues_2 = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'addXML' has been ignored.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'changeXML' has been ignored.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'removeXML' has been ignored.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: only changes of type 'changeAttribute' are currently supported. The change of type 'computeChange' has been ignored.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/unsupported_sed_changes.omex"))
    document = loc.SedDocument(file)

    assert_issues(document, expected_issues_2)
    assert document.serialise() == sed_changes_expected_serialisation(
        file.child_file(1).path, ""
    )


def test_simulations():
    document = loc.SedDocument()

    assert not document.has_simulations
    assert document.simulation_count == 0
    assert len(document.simulations) == 0
    assert not document.add_simulation(None)

    uniformTimeCourse = loc.SedUniformTimeCourse(document)
    oneStep = loc.SedOneStep(document)
    steadyState = loc.SedSteadyState(document)
    analysis = loc.SedAnalysis(document)

    assert document.add_simulation(uniformTimeCourse)
    assert document.add_simulation(oneStep)
    assert document.add_simulation(steadyState)
    assert document.add_simulation(analysis)

    assert document.has_simulations
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
    assert document.simulation(4) is None

    assert not document.add_simulation(uniformTimeCourse)
    assert document.remove_simulation(uniformTimeCourse)

    assert not document.add_simulation(oneStep)
    assert document.remove_simulation(oneStep)

    assert not document.add_simulation(steadyState)
    assert document.remove_simulation(steadyState)

    assert not document.add_simulation(analysis)
    assert document.remove_simulation(analysis)

    assert not document.has_simulations
    assert document.simulation_count == 0
    assert len(document.simulations) == 0

    assert not document.remove_simulation(None)
    assert not document.remove_all_simulations()

    assert document.add_simulation(uniformTimeCourse)
    assert document.remove_all_simulations()


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

    assert not document.has_tasks
    assert document.task_count == 0
    assert len(document.tasks) == 0
    assert not document.add_task(None)

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    model = loc.SedModel(document, file)
    simulation = loc.SedUniformTimeCourse(document)
    task = loc.SedTask(document, model, simulation)

    assert task.model == model
    assert task.simulation == simulation

    assert document.add_task(task)

    assert document.has_tasks
    assert document.task_count == 1
    assert len(document.tasks) == 1
    assert document.tasks[0] == task
    assert document.task(0) == task
    assert document.task(1) is None

    assert document.serialise() == sed_task_expected_serialisation(True)

    task.model = None
    task.simulation = None

    assert task.model is None
    assert task.simulation is None

    assert document.serialise() == sed_task_expected_serialisation(False)

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task: task 'task1' requires a model.",
        ],
        [
            loc.Issue.Type.Error,
            "Task: task 'task1' requires a simulation.",
        ],
    ]

    instance = document.instantiate()

    assert_issues(instance, expected_issues)

    assert not document.add_task(task)
    assert document.remove_task(task)

    assert not document.has_tasks
    assert document.task_count == 0
    assert len(document.tasks) == 0

    assert not document.remove_task(None)
    assert not document.remove_all_tasks()

    assert document.add_task(task)
    assert document.remove_all_tasks()


def test_ode_solver():
    document = loc.SedDocument()
    simulation = loc.SedUniformTimeCourse(document)

    assert simulation.ode_solver is None

    solver = loc.SolverCvode()

    simulation.ode_solver = solver

    assert simulation.ode_solver == solver

    simulation.ode_solver = None

    assert simulation.ode_solver is None


def test_nla_solver():
    document = loc.SedDocument()
    simulation = loc.SedUniformTimeCourse(document)

    assert simulation.nla_solver is None

    solver = loc.SolverKinsol()

    simulation.nla_solver = solver

    assert simulation.nla_solver == solver

    simulation.nla_solver = None

    assert simulation.nla_solver is None


def test_sed_base():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = loc.SedOneStep(document)

    assert simulation.id == "simulation2"
    assert loc.SedBase.is_valid_id("my_simulation")

    simulation.id = "my_simulation"

    assert simulation.id == "my_simulation"

    # An id that is not a valid SED-ML identifier is rejected, leaving the id unchanged.
    # Note: a property setter cannot return whether the id was set, so a ValueError is raised instead.

    for id in [
        "",
        "my simulation",
        "1simulation",
        "simulation-1",
        "simulation\x01",
        "\udcff",
    ]:
        assert not loc.SedBase.is_valid_id(id)

        with pytest.raises(ValueError) as exception_info:
            simulation.id = id

        assert str(exception_info.value) == "The id is not a valid SED-ML identifier."
        assert simulation.id == "my_simulation"

    # A string with a lone surrogate that doesn't stand for an invalid byte (i.e. other than U+DC80 to U+DCFF) is
    # rejected.
    # Note: this has no C++ equivalent since a std::string cannot contain a lone surrogate.

    with pytest.raises(TypeError):
        loc.SedBase.is_valid_id("\ud800")

    with pytest.raises(TypeError):
        simulation.is_duplicate_id("\ud800")

    with pytest.raises(TypeError):
        simulation.id = "\ud800"

    assert simulation.id == "my_simulation"


def test_sed_ids():
    # An id must be unique within the simulation experiment description for which an object was created.
    # Note: a property setter cannot return whether the id was set, so a ValueError is raised instead.

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    model = document.models[0]
    simulation = document.simulations[0]
    task = document.tasks[0]

    assert model.id == "model1"
    assert simulation.id == "simulation1"
    assert task.id == "task1"

    # A new object is not given the id of an existing object, but it can be given an id that is not used anymore.

    model.id = "model2"

    other_model = loc.SedModel(document, file)

    assert other_model.id == "model1"

    # An id that is already used by another object, whatever its type, is a duplicate and is therefore rejected,
    # leaving the id unchanged, even though it is a valid SED-ML identifier. The id of an object is, however, not a
    # duplicate, so an object can be given its own id.

    for id in ["model2", "simulation1", "task1"]:
        assert loc.SedBase.is_valid_id(id)
        assert other_model.is_duplicate_id(id)

        with pytest.raises(ValueError) as exception_info:
            other_model.id = id

        assert str(exception_info.value) == "The id is already used."
        assert other_model.id == "model1"

    assert not other_model.is_duplicate_id("model1")

    other_model.id = "model1"

    assert not other_model.is_duplicate_id("my_model")

    # A removed object releases its id, which can then be given to another object, in which case the id of the removed
    # object is a duplicate and the removed object can only be added back once it has been given another id.

    assert document.remove_model(model)
    assert not model.is_duplicate_id("model2")

    other_model.id = "model2"

    assert model.is_duplicate_id("model2")

    with pytest.raises(ValueError) as exception_info:
        model.id = "model2"

    assert str(exception_info.value) == "The id is already used."
    assert not document.add_model(model)

    model.id = "my_model"

    assert document.add_model(model)
    assert other_model.is_duplicate_id("my_model")

    with pytest.raises(ValueError) as exception_info:
        other_model.id = "my_model"

    assert str(exception_info.value) == "The id is already used."

    assert len(document.models) == 1
    assert document.models[0] == model

    # A destroyed object releases its id, even if it was never added to its simulation experiment description, so that
    # its id can then be given to another object.

    transient_model = loc.SedModel(document, file)

    assert transient_model.id == "model1"
    assert other_model.is_duplicate_id("model1")

    del transient_model

    assert not other_model.is_duplicate_id("model1")

    other_model.id = "model1"

    assert other_model.id == "model1"

    # An object can only be added to the simulation experiment description for which it was created.

    other_document = loc.SedDocument()

    assert not other_document.add_model(other_model)
    assert not other_document.add_simulation(simulation)
    assert not other_document.add_task(task)
    assert not other_document.has_models
    assert not other_document.has_simulations
    assert not other_document.has_tasks

    # An object which simulation experiment description doesn't exist anymore has no duplicate id, so it can be given
    # any valid id, but it cannot be added to another simulation experiment description.

    orphan_simulation = loc.SedOneStep(other_document)

    del other_document

    assert orphan_simulation.id == "simulation1"
    assert not orphan_simulation.is_duplicate_id("my_simulation")

    orphan_simulation.id = "my_simulation"

    assert orphan_simulation.id == "my_simulation"
    assert not document.add_simulation(orphan_simulation)


def test_sed_change_attribute():
    # A change attribute can only be created with component and variable names that are valid CellML identifiers and
    # with a new value that is a valid double value.
    # Note: a constructor cannot return None, so a ValueError is raised instead.

    with pytest.raises(ValueError):
        loc.SedChangeAttribute("my component", "variable", "1.23")

    with pytest.raises(ValueError):
        loc.SedChangeAttribute("component", "1variable", "1.23")

    with pytest.raises(ValueError):
        loc.SedChangeAttribute("component", "variable", "abc")

    change_attribute = loc.SedChangeAttribute("component", "variable", "1.23")

    change_attribute.component_name = "new_component"
    change_attribute.variable_name = "new_variable"

    # A new value can be any decimal number, which value must be finite once rounded to the nearest double value.

    for new_value in ["1.", ".5", "+1.5e-3", "-1E+3", "1e-400", "4.56"]:
        change_attribute.new_value = new_value

        assert change_attribute.new_value == new_value

    # A component name, variable name, or new value that is not valid is rejected, leaving it unchanged.
    # Note: a property setter cannot return whether the value was set, so a ValueError is raised instead.

    for name in ["", "my component", "1component", "_1", "component\x01", "\udcff"]:
        with pytest.raises(ValueError):
            change_attribute.component_name = name

        with pytest.raises(ValueError):
            change_attribute.variable_name = name

    for new_value in [
        "",
        "abc",
        "1.23abc",
        "1.23\x01",
        "\udcff",
        ".",
        "1e400",
        "inf",
        "nan",
        "0x1",
        " 1",
    ]:
        with pytest.raises(ValueError):
            change_attribute.new_value = new_value

    assert change_attribute.component_name == "new_component"
    assert change_attribute.variable_name == "new_variable"
    assert change_attribute.new_value == "4.56"
    assert (
        change_attribute.target
        == "/cellml:model/cellml:component[@name='new_component']/cellml:variable[@name='new_variable']"
    )


def test_sed_one_step():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = loc.SedOneStep(document)

    assert simulation.step == 1.0

    simulation.step = 1.23

    assert simulation.step == 1.23


def test_sed_uniform_time_course():
    file = loc.File(utils.resource_path("cellml_2.cellml"))
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


def test_sed_instance_and_sed_instance_task_differential_model():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task instance | CVODE: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 3.",
        ],
    ]

    file = loc.File(utils.resource_path("api/solver/ode.cellml"))
    document = loc.SedDocument(file)
    solver = document.simulations[0].ode_solver

    solver.linear_solver = loc.SolverCvode.LinearSolver.Banded
    solver.upper_half_bandwidth = -1

    instance = document.instantiate()
    instance_task = instance.tasks[0]

    assert instance.has_tasks
    assert instance.task_count == 1
    assert instance.task(0) == instance_task
    assert instance.task(1) is None

    # A SED instance can be sized and iterated over.
    # Note: this has no C++ or JavaScript equivalent since a SED instance is neither sized nor iterable in C++ or
    #       JavaScript.

    assert len(instance) == 1
    assert list(instance)[0] == instance_task

    assert len(instance_task.voi) == 0
    assert instance_task.voi_name == "environment/time"
    assert instance_task.voi_unit == "millisecond"

    assert instance_task.state_count == 4
    assert len(instance_task.state(0)) == 0
    assert len(instance_task.state(4)) == 0
    assert instance_task.state_name(0) == "membrane/V"
    assert instance_task.state_name(4) == ""
    assert instance_task.state_unit(0) == "millivolt"
    assert instance_task.state_unit(4) == ""

    assert instance_task.rate_count == 4
    assert len(instance_task.rate(0)) == 0
    assert len(instance_task.rate(4)) == 0
    assert instance_task.rate_name(0) == "membrane/V'"
    assert instance_task.rate_name(4) == ""
    assert instance_task.rate_unit(0) == "millivolt/millisecond"
    assert instance_task.rate_unit(4) == ""

    assert instance_task.constant_count == 5
    assert len(instance_task.constant(0)) == 0
    assert len(instance_task.constant(5)) == 0
    assert instance_task.constant_name(0) == "membrane/Cm"
    assert instance_task.constant_name(5) == ""
    assert instance_task.constant_unit(0) == "microF_per_cm2"
    assert instance_task.constant_unit(5) == ""

    assert instance_task.computed_constant_count == 3
    assert len(instance_task.computed_constant(0)) == 0
    assert len(instance_task.computed_constant(3)) == 0
    assert instance_task.computed_constant_name(0) == "leakage_current/E_L"
    assert instance_task.computed_constant_name(3) == ""
    assert instance_task.computed_constant_unit(0) == "millivolt"
    assert instance_task.computed_constant_unit(3) == ""

    assert instance_task.algebraic_variable_count == 10
    assert len(instance_task.algebraic_variable(0)) == 0
    assert len(instance_task.algebraic_variable(10)) == 0
    assert instance_task.algebraic_variable_name(0) == "membrane/i_Stim"
    assert instance_task.algebraic_variable_name(10) == ""
    assert instance_task.algebraic_variable_unit(0) == "microA_per_cm2"
    assert instance_task.algebraic_variable_unit(10) == ""

    instance.run()

    assert_issues(instance, expected_issues)


def test_sed_instance_and_sed_instance_task_non_differential_model():
    file = loc.File(utils.resource_path("api/solver/nla1.cellml"))
    document = loc.SedDocument(file)

    instance = document.instantiate()
    instance_task = instance.tasks[0]

    assert len(instance_task.voi) == 0
    assert instance_task.voi_name == ""
    assert instance_task.voi_unit == ""

    assert instance_task.state_count == 0
    assert len(instance_task.state(0)) == 0
    assert instance_task.state_name(0) == ""
    assert instance_task.state_unit(0) == ""

    assert instance_task.rate_count == 0
    assert len(instance_task.rate(0)) == 0
    assert instance_task.rate_name(0) == ""
    assert instance_task.rate_unit(0) == ""


def test_sed_document():
    # Note: the three following URLs use http rather than https to help with coverage testing.

    file = loc.File(
        "http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"
    )
    loc.SedDocument(file)

    file = loc.File(
        "http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.sedml"
    )
    loc.SedDocument(file)

    file = loc.File(
        "http://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex"
    )
    loc.SedDocument(file)


def test_solver():
    # Get the duplicate() method of different solvers to be covered.

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)

    document.simulations[0].ode_solver = loc.SolverForwardEuler()

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues

    document.simulations[0].ode_solver = loc.SolverFourthOrderRungeKutta()

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues

    document.simulations[0].ode_solver = loc.SolverHeun()

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues

    document.simulations[0].ode_solver = loc.SolverSecondOrderRungeKutta()

    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues


def test_math():
    computed_constant_values = [
        243.0,
        3.0,
        7.0,
        20.085536923187668,
        1.0986122886681098,
        0.47712125471966244,
        4.0,
        3.0,
        3.0,
        5.0,
        3.0,
        0.14112000805986721,
        -0.98999249660044542,
        -0.1425465430742778,
        -1.0101086659079939,
        7.0861673957371867,
        -7.0152525514345339,
        10.017874927409903,
        10.067661995777765,
        0.99505475368673046,
        0.099327927419433207,
        0.099821569668822732,
        1.0049698233136892,
        0.3046926540153975,
        1.266103672779499,
        1.2490457723982544,
        1.2309594173407747,
        0.33983690945412193,
        0.32175055439664219,
        1.8184464592320668,
        1.7627471740390861,
        0.30951960420311175,
        1.8738202425274144,
        0.32745015023725843,
        0.34657359027997264,
        math.inf,
        math.nan,
    ]
    computed_constant_abs_tols = [
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
    ]

    file = loc.File(utils.resource_path("api/sed/math.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()
    instance_task = instance.tasks[0]

    assert instance_task.constant_count == 0
    assert instance_task.computed_constant_count == 37
    assert instance_task.algebraic_variable_count == 0

    instance.run()

    assert_values(
        instance_task,
        0,
        [],
        [],
        [],
        [],
        [],
        [],
        computed_constant_values,
        computed_constant_abs_tols,
        [],
        [],
    )


def test_math_with_voi():
    # Note: unlike in the math test, our mathematical functions are evaluated at run time rather than possibly at
    #       compile time. Also, LLVM may optimise 2^t to exp2(t), in which case exp2() must be available to our model.

    output_end_time = 10.0
    number_of_steps = 10

    file = loc.File(utils.resource_path("api/sed/math_with_voi.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.output_end_time = output_end_time
    simulation.number_of_steps = number_of_steps

    instance = document.instantiate()
    instance_task = instance.tasks[0]

    assert instance_task.state_count == 1
    assert instance_task.constant_count == 0
    assert instance_task.computed_constant_count == 0
    assert instance_task.algebraic_variable_count == 35

    instance.run()

    assert not instance.has_issues

    # At t = 0, our algebraic variables have the same values as the computed constants of the math test.

    algebraic_values = [
        243.0,
        3.0,
        7.0,
        20.085536923187668,
        1.0986122886681098,
        0.47712125471966244,
        4.0,
        3.0,
        3.0,
        5.0,
        3.0,
        0.14112000805986721,
        -0.98999249660044542,
        -0.1425465430742778,
        -1.0101086659079939,
        7.0861673957371867,
        -7.0152525514345339,
        10.017874927409903,
        10.067661995777765,
        0.99505475368673046,
        0.099327927419433207,
        0.099821569668822732,
        1.0049698233136892,
        0.3046926540153975,
        1.266103672779499,
        1.2490457723982544,
        1.2309594173407747,
        0.33983690945412193,
        0.32175055439664219,
        1.8184464592320668,
        1.7627471740390861,
        0.30951960420311175,
        1.8738202425274144,
        0.32745015023725843,
        0.34657359027997264,
    ]
    algebraic_abs_tols = [
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
        0.0000001,
    ]

    assert_values(
        instance_task,
        0,
        [0.0],
        [0.0000001],
        [1.0],
        [0.0000001],
        [],
        [],
        [],
        [],
        algebraic_values,
        algebraic_abs_tols,
    )

    # At t = 10, x = (2^10-1)/ln(2).

    assert math.isclose(
        instance_task.state(0)[number_of_steps], 1475.8770268294097, abs_tol=0.01
    )


def test_kinsol_with_inf_and_or_nan_values():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task instance | KINSOL: the NLA system could not be solved (it contains some Inf and/or NaN values).",
        ],
    ]

    file = loc.File(
        utils.resource_path("api/sed/kinsol_with_inf_and_or_nan_values.cellml")
    )
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_kinsol_with_no_solution():
    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task instance | KINSOL: the linear solver's setup function failed in an unrecoverable manner.",
        ],
    ]

    file = loc.File(utils.resource_path("api/sed/kinsol_with_no_solution.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    assert_issues(instance, expected_issues)


def test_kinsol_with_no_solution_over_time():
    # The first NLA system of our model only has a solution until t = ln(2) while its second NLA system always has a
    # solution, so our simulation should fail at t = ln(2), whether we use a fixed-step ODE solver or CVODE.
    # Note #1: we cannot check the full description of CVODE's error since it includes the step size, which may vary
    #          from one platform to another.
    # Note #2: KINSOL reuses the Jacobian of an NLA system from one solve to the next, which, with CVODE (which solves
    #          our NLA systems many times per step), results in KINSOL failing because of its line search rather than
    #          because of its maximum number of iterations.

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | KINSOL: the maximum number of iterations was reached before convergence.",
        ],
    ]
    cvode_kinsol_error = "Task | KINSOL: the line search algorithm was unable to find an iterate sufficiently distinct from the current iterate."
    cvode_error_start = "Task | CVODE: at t = 0.6931"
    last_valid_index = 6

    file = loc.File(
        utils.resource_path("api/sed/kinsol_with_no_solution_over_time.cellml")
    )
    document = loc.SedDocument(file)
    simulation = document.simulations[0]
    forward_euler = loc.SolverForwardEuler()

    simulation.output_end_time = 2.0
    simulation.number_of_steps = 20

    forward_euler.step = 0.01

    simulation.ode_solver = forward_euler

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert_issues(instance, expected_issues)
    assert not math.isnan(instance.tasks[0].voi[last_valid_index])
    assert math.isnan(instance.tasks[0].voi[last_valid_index + 1])

    simulation.ode_solver = loc.SolverCvode()

    instance = document.instantiate()

    assert not instance.has_issues

    instance.run()

    assert len(instance.issues) == 2
    assert instance.issues[0].description == cvode_kinsol_error
    assert instance.issues[1].description.startswith(cvode_error_start)
    assert not math.isnan(instance.tasks[0].voi[last_valid_index])
    assert math.isnan(instance.tasks[0].voi[last_valid_index + 1])


def test_kinsol_with_no_solution_when_initialising():
    # The first NLA system of our model (a^2+x = 1) has no solution for x > 1, so if the initial value of x gets set to
    # 2 after our instance has been created, then our model cannot be (re)initialised when running our instance,
    # whether we use a fixed-step ODE solver or CVODE. Our simulation should therefore stop there and we should have no
    # results to report.

    expected_issues = [
        [
            loc.Issue.Type.Error,
            "Task | KINSOL: the linear solver's setup function failed in an unrecoverable manner.",
        ],
    ]
    number_of_steps = 20

    file = loc.File(
        utils.resource_path("api/sed/kinsol_with_no_solution_over_time.cellml")
    )
    forward_euler = loc.SolverForwardEuler()

    forward_euler.step = 0.01

    for ode_solver in [forward_euler, loc.SolverCvode()]:
        document = loc.SedDocument(file)
        simulation = document.simulations[0]

        simulation.output_end_time = 2.0
        simulation.number_of_steps = number_of_steps
        simulation.ode_solver = ode_solver

        instance = document.instantiate()

        assert not instance.has_issues

        document.model(0).add_change(loc.SedChangeAttribute("my_component", "x", "2.0"))

        assert instance.run() == 0.0
        assert_issues(instance, expected_issues)
        assert instance.progress == 0.0

        instance_task = instance.tasks[0]

        assert len(instance_task.voi) == number_of_steps + 1
        assert all(math.isnan(value) for value in instance_task.voi)
        assert all(math.isnan(value) for value in instance_task.state(0))


def test_sedml_file_nla_algorithm_and_nla_algorithm():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the NLA solver is already set for the simulation 'simulation1'. The one specified in the nlaAlgorithm element will be ignored.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_nla_algorithm_and_nla_algorithm.sedml")
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_sedml_file_several_nla_algorithms():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: multiple nlaAlgorithm elements have been found for the simulation 'simulation1'. The first one will be used.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_several_nla_algorithms.sedml")
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_sedml_file_unknown_nla_algorithm():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the NLA solver 'KISAO:0000000' in simulation 'simulation1' is not recognised. The KINSOL solver will be used instead.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_unknown_nla_algorithm.sedml")
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_sedml_file_branch_coverage():
    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_branch_coverage.sedml")
    )
    document = loc.SedDocument(sedml_file)


def test_sedml_file_no_nla_algorithm():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the NLA solver '' in simulation 'simulation1' is not recognised. The KINSOL solver will be used instead.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_no_nla_algorithm.sedml")
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_legacy_sedml_file_nla_algorithm_and_nla_solver():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the NLA solver 'KINSOL' is specified in both the algorithm element and in the annotation of the simulation 'simulation1'. The one specified in the algorithm element will be used.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path(
            "api/sed/dae/model_legacy_nla_algorithm_and_nla_solver.sedml"
        )
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_legacy_sedml_file_several_nla_solvers():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: multiple NLA solvers have been found in the annotation of the simulation 'simulation1'. The first one will be used.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_legacy_several_nla_solvers.sedml")
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_legacy_sedml_file_unknown_namespace():
    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_legacy_unknown_namespace.sedml")
    )
    document = loc.SedDocument(sedml_file)


def test_legacy_sedml_file_unknown_element():
    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_legacy_unknown_element.sedml")
    )
    document = loc.SedDocument(sedml_file)


def test_legacy_sedml_file_unknown_nla_solver():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the NLA solver 'UNKNOWN' in simulation 'simulation1' is not recognised. The KINSOL solver will be used instead.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_legacy_unknown_nla_solver.sedml")
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_legacy_sedml_file_unknown_property_namespace():
    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_legacy_unknown_property_namespace.sedml")
    )
    document = loc.SedDocument(sedml_file)


def test_legacy_sedml_file_unknown_property_element():
    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_legacy_unknown_property_element.sedml")
    )
    document = loc.SedDocument(sedml_file)


def test_legacy_sedml_file_unknown_property():
    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the NLA solver property 'Unknown' is not recognised. It will be ignored.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("api/sed/dae/model.cellml"))
    sedml_file = loc.File(
        utils.resource_path("api/sed/dae/model_legacy_unknown_property.sedml")
    )
    document = loc.SedDocument(sedml_file)

    assert_issues(document, expected_issues)


def test_sedml_file_with_bare_filename():
    file = loc.File("cellml_2.sedml")

    file.contents = utils.text_to_list(
        utils.text_file_contents(utils.resource_path("cellml_2.sedml"))
    )


def test_sedml_file_with_invalid_ids_and_change_names():
    # libSEDML doesn't reject an id that is not a valid SED-ML identifier nor a change with component and/or variable
    # names that are not valid CellML identifiers, so such an id is replaced with the one generated by libOpenCOR and
    # such a change is ignored.

    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the id 'my model' of the model is not a valid SED-ML identifier. The id 'model1' has been used instead.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the component name 'my component' for the change of type 'changeAttribute' is not a valid CellML identifier. The change has been ignored.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the variable name '1x' for the change of type 'changeAttribute' is not a valid CellML identifier. The change has been ignored.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the id 'my simulation' of the simulation is not a valid SED-ML identifier. The id 'simulation1' has been used instead.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("cellml_2.cellml"))
    file = loc.File(utils.resource_path("my_model.sedml"), False)

    file.contents = utils.text_to_list(
        """<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="my model" language="urn:sedml:language:cellml" source="cellml_2.cellml">
      <listOfChanges>
        <changeAttribute target="/cellml:model/cellml:component[@name='my component']/cellml:variable[@name='x']" newValue="1.23"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='1x']" newValue="4.56"/>
        <changeAttribute target="/cellml:model/cellml:component[@name='main']/cellml:variable[@name='x']" newValue="7.89"/>
      </listOfChanges>
    </model>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="my simulation" step="1"/>
  </listOfSimulations>
</sedML>
"""
    )

    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
    assert len(document.models) == 1
    assert document.models[0].file == cellml_file
    assert document.models[0].id == "model1"
    assert document.models[0].change_count == 1
    assert len(document.simulations) == 1
    assert document.simulations[0].id == "simulation1"


def test_sedml_file_with_invalid_ids_and_clashing_valid_ids():
    # An id that is not a valid SED-ML identifier is replaced with the one generated by libOpenCOR, which must not be
    # the id of another model or simulation (e.g., "model1" and "simulation1" below).

    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the id 'my model' of the model is not a valid SED-ML identifier. The id 'model2' has been used instead.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the id 'my simulation' of the simulation is not a valid SED-ML identifier. The id 'simulation2' has been used instead.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("cellml_2.cellml"))
    file = loc.File(utils.resource_path("my_model.sedml"), False)

    file.contents = utils.text_to_list(
        """<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="my model" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="my simulation" step="1"/>
    <oneStep id="simulation1" step="1"/>
  </listOfSimulations>
</sedML>
"""
    )

    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
    assert len(document.models) == 2
    assert document.models[0].file == cellml_file
    assert document.models[0].id == "model2"
    assert document.models[1].file == cellml_file
    assert document.models[1].id == "model1"
    assert len(document.simulations) == 2
    assert document.simulations[0].id == "simulation2"
    assert document.simulations[1].id == "simulation1"


def test_sedml_file_with_duplicate_ids():
    # An id that is already used is replaced with the one generated by libOpenCOR. This includes a model and a
    # simulation with the same id since all the ids of a SED-ML file share the same namespace.

    expected_issues = [
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the id 'my_id' of the model is already used. The id 'model1' has been used instead.",
        ],
        [
            loc.Issue.Type.Warning,
            "SED-ML file: the id 'my_id' of the simulation is already used. The id 'simulation1' has been used instead.",
        ],
    ]

    cellml_file = loc.File(utils.resource_path("cellml_2.cellml"))
    file = loc.File(utils.resource_path("my_model.sedml"), False)

    file.contents = utils.text_to_list(
        """<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="my_id" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
    <model id="my_id" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="my_id" step="1"/>
    <oneStep id="my_simulation" step="1"/>
  </listOfSimulations>
</sedML>
"""
    )

    document = loc.SedDocument(file)

    assert_issues(document, expected_issues)
    assert len(document.models) == 2
    assert document.models[0].id == "my_id"
    assert document.models[1].id == "model1"
    assert len(document.simulations) == 2
    assert document.simulations[0].id == "simulation1"
    assert document.simulations[1].id == "my_simulation"

    # The ids of the SED-ML file are only reserved while it is being imported.

    assert loc.SedOneStep(document).id == "simulation2"


def test_simulation_results_are_numpy_arrays():
    # Note: this has no C++ equivalent since the type of our simulation results is known at compile time in C++.

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    instance = document.instantiate()

    instance.run()

    assert not instance.has_issues

    instance_task = instance.tasks[0]

    assert isinstance(instance_task.voi, numpy.ndarray)
    assert isinstance(instance_task.state(0), numpy.ndarray)
    assert isinstance(instance_task.rate(0), numpy.ndarray)
    assert isinstance(instance_task.constant(0), numpy.ndarray)
    assert isinstance(instance_task.computed_constant(0), numpy.ndarray)
    assert isinstance(instance_task.algebraic_variable(0), numpy.ndarray)
