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
from utils import assert_hidden_properties


no_properties = {}
no_hidden_properties = []


def check_cvode_solver(solver_info):
    # Properties.

    solver_info_properties = solver_info.properties

    assert len(solver_info_properties) == 11

    property = solver_info_properties[0]

    assert property.type == SolverProperty.Type.DoubleGe0
    assert property.id == "KISAO:0000467"
    assert property.name == "Maximum step"
    assert property.default_value == "0"
    assert property.has_voi_unit == True

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[1]

    assert property.type == SolverProperty.Type.IntegerGt0
    assert property.id == "KISAO:0000415"
    assert property.name == "Maximum number of steps"
    assert property.default_value == "500"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[2]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000475"
    assert property.name == "Integration method"
    assert property.default_value == "BDF"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 2
    assert list_values[0] == "Adams-Moulton"
    assert list_values[1] == "BDF"

    property = solver_info_properties[3]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000476"
    assert property.name == "Iteration type"
    assert property.default_value == "Newton"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 2
    assert list_values[0] == "Functional"
    assert list_values[1] == "Newton"

    property = solver_info_properties[4]

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

    property = solver_info_properties[5]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000478"
    assert property.name == "Preconditioner"
    assert property.default_value == "Banded"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 2
    assert list_values[0] == "None"
    assert list_values[1] == "Banded"

    property = solver_info_properties[6]

    assert property.type == SolverProperty.Type.IntegerGe0
    assert property.id == "KISAO:0000479"
    assert property.name == "Upper half-bandwidth"
    assert property.default_value == "0"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[7]

    assert property.type == SolverProperty.Type.IntegerGe0
    assert property.id == "KISAO:0000480"
    assert property.name == "Lower half-bandwidth"
    assert property.default_value == "0"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[8]

    assert property.type == SolverProperty.Type.DoubleGe0
    assert property.id == "KISAO:0000209"
    assert property.name == "Relative tolerance"
    assert property.default_value == "1e-07"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[9]

    assert property.type == SolverProperty.Type.DoubleGe0
    assert property.id == "KISAO:0000211"
    assert property.name == "Absolute tolerance"
    assert property.default_value == "1e-07"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[10]

    assert property.type == SolverProperty.Type.Boolean
    assert property.id == "KISAO:0000481"
    assert property.name == "Interpolate solution"
    assert property.default_value == "True"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    # Hidden properties.

    hidden_properties_for_newton = no_hidden_properties
    hidden_properties_for_newton_dense = [
        "KISAO:0000478",
        "KISAO:0000479",
        "KISAO:0000480",
    ]
    hidden_properties_for_newton_banded = ["KISAO:0000478"]
    hidden_properties_for_newton_gmres = no_hidden_properties
    hidden_properties_for_newton_gmres_none = ["KISAO:0000479", "KISAO:0000480"]
    hidden_properties_for_newton_gmres_banded = no_hidden_properties
    hidden_properties_for_newton_bicgstab = no_hidden_properties
    hidden_properties_for_newton_bicgstab_none = hidden_properties_for_newton_gmres_none
    hidden_properties_for_newton_bicgstab_banded = no_hidden_properties
    hidden_properties_for_newton_tfqmr = no_hidden_properties
    hidden_properties_for_newton_tfqmr_none = hidden_properties_for_newton_gmres_none
    hidden_properties_for_newton_tfqmr_banded = no_hidden_properties
    hidden_properties_for_newton_diagonal = hidden_properties_for_newton_dense
    hidden_properties_for_functional = [
        "KISAO:0000477",
        "KISAO:0000478",
        "KISAO:0000479",
        "KISAO:0000480",
    ]

    assert_hidden_properties(
        solver_info.hidden_properties(no_properties), no_hidden_properties
    )

    properties = {}

    properties["Iteration type"] = "Newton"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton
    )

    properties["KISAO:0000477"] = "Dense"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton_dense
    )

    properties["Linear solver"] = "Banded"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton_dense
    )

    properties["KISAO:0000477"] = "Banded"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton_banded
    )

    properties["KISAO:0000477"] = "Diagonal"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton_diagonal
    )

    properties["KISAO:0000477"] = "GMRES"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton_gmres
    )

    properties["KISAO:0000477"] = "BiCGStab"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton_bicgstab
    )

    properties["KISAO:0000477"] = "TFQMR"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_newton_tfqmr
    )

    properties["KISAO:0000477"] = "GMRES"
    properties["Preconditioner"] = "None"

    assert_hidden_properties(
        solver_info.hidden_properties(properties),
        hidden_properties_for_newton_gmres_none,
    )

    properties["KISAO:0000477"] = "BiCGStab"

    assert_hidden_properties(
        solver_info.hidden_properties(properties),
        hidden_properties_for_newton_bicgstab_none,
    )

    properties["KISAO:0000477"] = "TFQMR"

    assert_hidden_properties(
        solver_info.hidden_properties(properties),
        hidden_properties_for_newton_tfqmr_none,
    )

    properties["KISAO:0000477"] = "GMRES"
    properties["KISAO:0000478"] = "Banded"

    assert_hidden_properties(
        solver_info.hidden_properties(properties),
        hidden_properties_for_newton_gmres_banded,
    )

    properties["KISAO:0000477"] = "BiCGStab"

    assert_hidden_properties(
        solver_info.hidden_properties(properties),
        hidden_properties_for_newton_bicgstab_banded,
    )

    properties["KISAO:0000477"] = "TFQMR"

    assert_hidden_properties(
        solver_info.hidden_properties(properties),
        hidden_properties_for_newton_tfqmr_banded,
    )

    properties["KISAO:0000476"] = "Functional"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_functional
    )

    properties["Iteration type"] = "Newton"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_functional
    )


def check_forward_euler_solver(solver_info):
    # Properties.

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

    # Hidden properties.

    assert_hidden_properties(
        solver_info.hidden_properties(no_properties), no_hidden_properties
    )


def check_fourth_order_runge_kutta_solver(solver_info):
    # Properties.

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

    # Hidden properties.

    assert_hidden_properties(
        solver_info.hidden_properties(no_properties), no_hidden_properties
    )


def check_heun_solver(solver_info):
    # Properties.

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

    # Hidden properties.

    assert_hidden_properties(
        solver_info.hidden_properties(no_properties), no_hidden_properties
    )


def check_kinsol_solver(solver_info):
    # Properties.

    solver_info_properties = solver_info.properties

    assert len(solver_info_properties) == 4

    property = solver_info_properties[0]

    assert property.type == SolverProperty.Type.IntegerGt0
    assert property.id == "KISAO:0000486"
    assert property.name == "Maximum number of iterations"
    assert property.default_value == "200"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[1]

    assert property.type == SolverProperty.Type.List
    assert property.id == "KISAO:0000477"
    assert property.name == "Linear solver"
    assert property.default_value == "Dense"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 5
    assert list_values[0] == "Dense"
    assert list_values[1] == "Banded"
    assert list_values[2] == "GMRES"
    assert list_values[3] == "BiCGStab"
    assert list_values[4] == "TFQMR"

    property = solver_info_properties[2]

    assert property.type == SolverProperty.Type.IntegerGe0
    assert property.id == "KISAO:0000479"
    assert property.name == "Upper half-bandwidth"
    assert property.default_value == "0"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    property = solver_info_properties[3]

    assert property.type == SolverProperty.Type.IntegerGe0
    assert property.id == "KISAO:0000480"
    assert property.name == "Lower half-bandwidth"
    assert property.default_value == "0"
    assert property.has_voi_unit == False

    list_values = property.list_values

    assert len(list_values) == 0

    # Hidden properties.

    hidden_properties_for_dense = ["KISAO:0000479", "KISAO:0000480"]
    hidden_properties_for_banded = no_hidden_properties
    hidden_properties_for_gmres = hidden_properties_for_dense
    hidden_properties_for_bicgstab = hidden_properties_for_dense
    hidden_properties_for_tfqmr = hidden_properties_for_dense

    assert_hidden_properties(
        solver_info.hidden_properties(no_properties), no_hidden_properties
    )

    properties = {}

    properties["KISAO:0000477"] = "Dense"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_dense
    )

    properties["Linear solver"] = "Banded"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_dense
    )

    properties["KISAO:0000477"] = "Banded"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_banded
    )

    properties["KISAO:0000477"] = "GMRES"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_gmres
    )

    properties["KISAO:0000477"] = "BiCGStab"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_bicgstab
    )

    properties["KISAO:0000477"] = "TFQMR"

    assert_hidden_properties(
        solver_info.hidden_properties(properties), hidden_properties_for_tfqmr
    )


def check_second_order_runge_kutta_solver(solver_info):
    # Properties.

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

    # Hidden properties.

    assert_hidden_properties(
        solver_info.hidden_properties(no_properties), no_hidden_properties
    )
