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


def check_cvode_solver(solver_info):
    assert solver_info.type == Solver.Type.Ode
    assert solver_info.id == "KISAO:0000019"
    assert solver_info.name == "CVODE"

    properties = solver_info.properties

    assert len(properties) == 11

    property = properties[0]

    assert property.type == SolverProperty.Type.DoubleGe0
    assert property.id == "KISAO:0000467"
    assert property.name == "Maximum step"
    assert property.default_value == "0"
    assert property.has_voi_unit == True

    list_values = property.list_values

    assert len(list_values) == 0

    property = properties[1]

    assert property.type == SolverProperty.Type.IntegerGt0
    assert property.id == "KISAO:0000415"
    assert property.name == "Maximum number of steps"
    assert property.default_value == "500"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = properties[2]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000475"
    assert property.name == "Integration method"
    assert property.default_value == "BDF"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 2
    assert list_values[0] == "Adams-Moulton"
    assert list_values[1] == "BDF"

    property = properties[3]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000476"
    assert property.name == "Iteration type"
    assert property.default_value == "Newton"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 2
    assert list_values[0] == "Functional"
    assert list_values[1] == "Newton"

    property = properties[4]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000477"
    assert property.name == "Linear solver"
    assert property.default_value == "Dense"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 6
    assert list_values[0] == "Dense"
    assert list_values[1] == "Banded"
    assert list_values[2] == "Diagonal"
    assert list_values[3] == "GMRES"
    assert list_values[4] == "BiCGStab"
    assert list_values[5] == "TFQMR"

    property = properties[5]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000478"
    assert property.name == "Preconditioner"
    assert property.default_value == "Banded"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 2
    assert list_values[0] == "None"
    assert list_values[1] == "Banded"

    property = properties[6]

    assert property.type == SolverProperty.Type.IntegerGe0
    assert property.id == "KISAO:0000479"
    assert property.name == "Upper half-bandwidth"
    assert property.default_value == "0"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = properties[7]

    assert property.type == SolverProperty.Type.IntegerGe0
    assert property.id == "KISAO:0000480"
    assert property.name == "Lower half-bandwidth"
    assert property.default_value == "0"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = properties[8]

    assert property.type == SolverProperty.Type.DoubleGe0
    assert property.id == "KISAO:0000209"
    assert property.name == "Relative tolerance"
    assert property.default_value == "1e-07"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = properties[9]

    assert property.type == SolverProperty.Type.DoubleGe0
    assert property.id == "KISAO:0000211"
    assert property.name == "Absolute tolerance"
    assert property.default_value == "1e-07"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = properties[10]

    assert property.type == SolverProperty.Type.Boolean
    assert property.id == "KISAO:0000481"
    assert property.name == "Interpolate solution"
    assert property.default_value == "True"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0


def check_forward_euler_solver(solver_info):
    assert solver_info.type == Solver.Type.Ode
    assert solver_info.id == "KISAO:0000030"
    assert solver_info.name == "Forward Euler"

    properties = solver_info.properties

    assert len(properties) == 1

    property = properties[0]

    assert property.type == SolverProperty.Type.DoubleGt0
    assert property.id == "KISAO:0000483"
    assert property.name == "Step"
    assert property.default_value == "1"
    assert property.has_voi_unit == True

    list_values = property.list_values

    assert len(list_values) == 0


def check_fourth_order_runge_kutta_solver(solver_info):
    assert solver_info.type == Solver.Type.Ode
    assert solver_info.id == "KISAO:0000032"
    assert solver_info.name == "Fourth-order Runge-Kutta"

    properties = solver_info.properties

    assert len(properties) == 1

    property = properties[0]

    assert property.type == SolverProperty.Type.DoubleGt0
    assert property.id == "KISAO:0000483"
    assert property.name == "Step"
    assert property.default_value == "1"
    assert property.has_voi_unit == True

    list_values = property.list_values

    assert len(list_values) == 0


def check_heun_solver(solver_info):
    assert solver_info.type == Solver.Type.Ode
    assert solver_info.id == "KISAO:0000301"
    assert solver_info.name == "Heun"

    properties = solver_info.properties

    assert len(properties) == 1

    property = properties[0]

    assert property.type == SolverProperty.Type.DoubleGt0
    assert property.id == "KISAO:0000483"
    assert property.name == "Step"
    assert property.default_value == "1"
    assert property.has_voi_unit == True

    list_values = property.list_values

    assert len(list_values) == 0


def check_second_order_runge_kutta_solver(solver_info):
    assert solver_info.type == Solver.Type.Ode
    assert solver_info.id == "KISAO:0000381"
    assert solver_info.name == "Second-order Runge-Kutta"

    properties = solver_info.properties

    assert len(properties) == 1

    property = properties[0]

    assert property.type == SolverProperty.Type.DoubleGt0
    assert property.id == "KISAO:0000483"
    assert property.name == "Step"
    assert property.default_value == "1"
    assert property.has_voi_unit == True

    list_values = property.list_values

    assert len(list_values) == 0
