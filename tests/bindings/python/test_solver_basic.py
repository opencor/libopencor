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


from libopencor import Solver, SolverOde, SolverProperty


def test_solvers_info():
    solvers_info = Solver.solvers_info()

    assert len(solvers_info) == 1

    solver_info = solvers_info[0]

    assert solver_info.type == Solver.Type.Ode
    assert solver_info.name == "Forward Euler"
    assert solver_info.kisao_id == "KISAO:0000030"

    properties = solver_info.properties

    assert len(properties) == 1

    property = properties[0]

    assert property.type == SolverProperty.Type.DoubleGt0
    assert property.name == "Step"
    assert property.kisao_id == "KISAO:0000483"
    assert property.default_value == "1.000000"
    assert property.has_voi_unit == True

    list_values = property.list_values

    assert len(list_values) == 0


def test_unknown_ode_solver():
    ode_solver = SolverOde("Unknown")

    assert ode_solver.is_valid == False


def test_forward_euler_by_name():
    ode_solver = SolverOde("Forward Euler")

    assert ode_solver.is_valid == True


def test_forward_euler_by_kisao_id():
    ode_solver = SolverOde("KISAO:0000030")

    assert ode_solver.is_valid == True


def test_properties():
    ode_solver = SolverOde("Forward Euler")
    properties = ode_solver.properties

    assert len(ode_solver.properties) == 1
    assert ode_solver.property("Step") == "1.000000"
    assert ode_solver.property("KISAO:0000483") == "1.000000"

    ode_solver.set_property("Step", "1.2345")

    assert len(ode_solver.properties) == 1
    assert ode_solver.property("KISAO:0000483") == "1.2345"

    ode_solver.set_property("KISAO:0000483", "7.89")

    assert len(ode_solver.properties) == 1
    assert ode_solver.property("Step") == "7.89"

    ode_solver.set_property("Unknown property", "1.23")

    assert len(ode_solver.properties) == 1
    assert ode_solver.property("Step") == "7.89"
    assert ode_solver.property("Unknown property") == ""

    ode_solver.set_properties(properties)

    assert len(ode_solver.properties) == 1
    assert ode_solver.property("Step") == "1.000000"
