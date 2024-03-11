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


from libopencor import (
    File,
    SedDocument,
    SedModel,
    SedSimulationOneStep,
    SedSimulationUniformTimeCourse,
    SolverCvode,
    SolverKinsol,
)
import utils


def test_initialise():
    expected_serialisation = """<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4"/>
"""

    sed = SedDocument()

    assert sed.serialise() == expected_serialisation


def test_models():
    sed = SedDocument()

    assert sed.has_models == False
    assert sed.add_model(None) == False

    file = File(utils.LOCAL_FILE)
    model = SedModel(file, sed)

    assert sed.add_model(model) == True

    assert len(sed.models) == 1
    assert sed.models[0] == model

    assert sed.add_model(model) == False
    assert sed.remove_model(model) == True

    assert sed.has_models == False

    assert sed.remove_model(None) == False


def test_simulations():
    sed = SedDocument()

    assert sed.has_simulations == False
    assert sed.add_simulation(None) == False

    simulation = SedSimulationUniformTimeCourse(sed)

    assert sed.add_simulation(simulation) == True

    assert len(sed.simulations) == 1
    assert sed.simulations[0] == simulation

    assert sed.add_simulation(simulation) == False
    assert sed.remove_simulation(simulation) == True

    assert sed.has_simulations == False

    assert sed.remove_simulation(None) == False


def test_ode_solver():
    sed = SedDocument()
    simulation = SedSimulationUniformTimeCourse(sed)

    assert simulation.ode_solver == None

    solver = SolverCvode()

    simulation.ode_solver = solver

    assert simulation.ode_solver == solver

    simulation.ode_solver = None

    assert simulation.ode_solver == None


def test_nla_solver():
    sed = SedDocument()
    simulation = SedSimulationUniformTimeCourse(sed)

    assert simulation.nla_solver == None

    solver = SolverKinsol()

    simulation.nla_solver = solver

    assert simulation.nla_solver == solver

    simulation.nla_solver = None

    assert simulation.nla_solver == None


def test_sed_simulation_one_step():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)
    simulation = SedSimulationOneStep(sed)

    assert simulation.step == 1.0

    simulation.step = 1.23

    assert simulation.step == 1.23


def test_sed_simulation_uniform_time_course():
    file = File(utils.resource_path(utils.CELLML_2_FILE))
    sed = SedDocument(file)
    simulation = SedSimulationUniformTimeCourse(sed)

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
