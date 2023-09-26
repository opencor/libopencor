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


from libopencor import Solver, SolverProperty


def test_solvers_info():
    solvers_info = Solver.solvers_info()

    assert len(solvers_info) == 4

    # Forward Euler.

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

    # Fourth-order Runge-Kutta.

    solver_info = solvers_info[1]

    assert solver_info.type == Solver.Type.Ode
    assert solver_info.name == "Fourth-order Runge-Kutta"
    assert solver_info.kisao_id == "KISAO:0000032"

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

    # Heun.

    solver_info = solvers_info[2]

    assert solver_info.type == Solver.Type.Ode
    assert solver_info.name == "Heun"
    assert solver_info.kisao_id == "KISAO:0000301"

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

    # Second-order Runge-Kutta.

    solver_info = solvers_info[3]

    assert solver_info.type == Solver.Type.Ode
    assert solver_info.name == "Second-order Runge-Kutta"
    assert solver_info.kisao_id == "KISAO:0000381"

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


def test_unknown_solver():
    solver = Solver("Unknown")

    assert solver.is_valid == False


def test_forward_euler_by_name():
    solver = Solver("Forward Euler")

    assert solver.is_valid == True


def test_forward_euler_by_kisao_id():
    solver = Solver("KISAO:0000030")

    assert solver.is_valid == True


def test_fourth_order_runge_kutta_by_name():
    solver = Solver("Fourth-order Runge-Kutta")

    assert solver.is_valid == True


def test_fourth_order_runge_kutta_by_kisao_id():
    solver = Solver("KISAO:0000032")

    assert solver.is_valid == True


def test_heun_by_name():
    solver = Solver("Heun")

    assert solver.is_valid == True


def test_heun_by_kisao_id():
    solver = Solver("KISAO:0000301")

    assert solver.is_valid == True


def test_second_order_runge_kutta_by_name():
    solver = Solver("Second-order Runge-Kutta")

    assert solver.is_valid == True


def test_second_order_runge_kutta_by_kisao_id():
    solver = Solver("KISAO:0000381")

    assert solver.is_valid == True


def test_properties():
    solver = Solver("Forward Euler")
    properties = solver.properties

    assert len(solver.properties) == 1
    assert solver.property("Step") == "1.000000"
    assert solver.property("KISAO:0000483") == "1.000000"

    solver.set_property("Step", "1.2345")

    assert len(solver.properties) == 1
    assert solver.property("KISAO:0000483") == "1.2345"

    solver.set_property("KISAO:0000483", "7.89")

    assert len(solver.properties) == 1
    assert solver.property("Step") == "7.89"

    solver.set_property("Unknown property", "1.23")

    assert len(solver.properties) == 1
    assert solver.property("Step") == "7.89"
    assert solver.property("Unknown property") == ""

    solver.set_properties(properties)

    assert len(solver.properties) == 1
    assert solver.property("Step") == "1.000000"
