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


from libopencor import Solver
from solvers import (
    check_cvode_solver,
    check_forward_euler_solver,
    check_fourth_order_runge_kutta_solver,
    check_heun_solver,
    check_kinsol_solver,
    check_second_order_runge_kutta_solver,
)


def test_solvers_info():
    solvers_info = Solver.solvers_info()

    assert len(solvers_info) == 6

    check_cvode_solver(solvers_info[0])
    check_forward_euler_solver(solvers_info[1])
    check_fourth_order_runge_kutta_solver(solvers_info[2])
    check_heun_solver(solvers_info[3])
    check_kinsol_solver(solvers_info[4])
    check_second_order_runge_kutta_solver(solvers_info[5])


def test_cvode_by_id():
    solver = Solver("KISAO:0000019")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000019"
    assert solver.name == "CVODE"

    assert solver != None


def test_cvode_by_name():
    solver = Solver("CVODE")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000019"
    assert solver.name == "CVODE"

    assert solver != None


def test_forward_euler_by_id():
    solver = Solver("KISAO:0000030")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000030"
    assert solver.name == "Forward Euler"

    assert solver != None


def test_forward_euler_by_name():
    solver = Solver("Forward Euler")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000030"
    assert solver.name == "Forward Euler"

    assert solver != None


def test_fourth_order_runge_kutta_by_id():
    solver = Solver("KISAO:0000032")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000032"
    assert solver.name == "Fourth-order Runge-Kutta"

    assert solver != None


def test_fourth_order_runge_kutta_by_name():
    solver = Solver("Fourth-order Runge-Kutta")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000032"
    assert solver.name == "Fourth-order Runge-Kutta"

    assert solver != None


def test_heun_by_id():
    solver = Solver("KISAO:0000301")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000301"
    assert solver.name == "Heun"

    assert solver != None


def test_heun_by_name():
    solver = Solver("Heun")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000301"
    assert solver.name == "Heun"

    assert solver != None


def test_kinsol_by_id():
    solver = Solver("KISAO:0000282")

    assert solver.type == Solver.Type.Nla
    assert solver.id == "KISAO:0000282"
    assert solver.name == "KINSOL"

    assert solver != None


def test_kinsol_by_name():
    solver = Solver("KINSOL")

    assert solver.type == Solver.Type.Nla
    assert solver.id == "KISAO:0000282"
    assert solver.name == "KINSOL"

    assert solver != None


def test_second_order_runge_kutta_by_id():
    solver = Solver("KISAO:0000381")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000381"
    assert solver.name == "Second-order Runge-Kutta"

    assert solver != None


def test_second_order_runge_kutta_by_name():
    solver = Solver("Second-order Runge-Kutta")

    assert solver.type == Solver.Type.Ode
    assert solver.id == "KISAO:0000381"
    assert solver.name == "Second-order Runge-Kutta"

    assert solver != None


def test_properties():
    solver = Solver("Forward Euler")
    properties = solver.properties

    assert len(solver.properties) == 1
    assert solver.property("KISAO:0000483") == "1"
    assert solver.property("Step") == "1"

    solver.set_property("Step", "1.23")

    assert len(solver.properties) == 1
    assert solver.property("KISAO:0000483") == "1.23"

    solver.set_property("KISAO:0000483", "7.89")

    assert len(solver.properties) == 1
    assert solver.property("Step") == "7.89"

    solver.set_property("Unknown", "1.23")

    assert len(solver.properties) == 1
    assert solver.property("Step") == "7.89"
    assert solver.property("Unknown") == ""

    properties["Step"] = "1.23"

    assert len(properties) == 2

    solver.set_properties(properties)

    assert len(solver.properties) == 1
    assert solver.property("Step") == "1"

    properties["Unknown"] = "1.23"

    assert len(properties) == 3

    solver.set_properties(properties)

    assert len(solver.properties) == 1
    assert solver.property("Step") == "1"

    properties["KISAO:0000483"] = "1.23"

    assert len(properties) == 3

    solver.set_properties(properties)

    assert len(solver.properties) == 1
    assert solver.property("Step") == "1.23"

    properties.pop("KISAO:0000483")

    assert len(properties) == 2

    solver.set_properties(properties)

    assert len(solver.properties) == 1
    assert solver.property("Step") == "1.23"
