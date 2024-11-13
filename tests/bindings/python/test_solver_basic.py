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


import libopencor as oc


def test_cvode_solver():
    solver = oc.SolverCvode()

    assert solver.type == oc.Solver.Type.Ode
    assert solver.id == "KISAO:0000019"
    assert solver.name == "CVODE"

    assert solver.maximum_step == 0.0
    assert solver.maximum_number_of_steps == 500
    assert solver.integration_method == oc.SolverCvode.IntegrationMethod.Bdf
    assert solver.iteration_type == oc.SolverCvode.IterationType.Newton
    assert solver.linear_solver == oc.SolverCvode.LinearSolver.Dense
    assert solver.preconditioner == oc.SolverCvode.Preconditioner.Banded
    assert solver.upper_half_bandwidth == 0
    assert solver.lower_half_bandwidth == 0
    assert solver.relative_tolerance == 1.0e-7
    assert solver.absolute_tolerance == 1.0e-7
    assert solver.interpolate_solution == True

    solver.maximum_step = 1.23
    solver.maximum_number_of_steps = 123
    solver.integration_method = oc.SolverCvode.IntegrationMethod.AdamsMoulton
    solver.iteration_type = oc.SolverCvode.IterationType.Functional
    solver.linear_solver = oc.SolverCvode.LinearSolver.Gmres
    solver.preconditioner = oc.SolverCvode.Preconditioner.No
    solver.upper_half_bandwidth = 3
    solver.lower_half_bandwidth = 5
    solver.relative_tolerance = 1.23e-5
    solver.absolute_tolerance = 3.45e-7
    solver.interpolate_solution = False

    assert solver.maximum_step == 1.23
    assert solver.maximum_number_of_steps == 123
    assert solver.integration_method == oc.SolverCvode.IntegrationMethod.AdamsMoulton
    assert solver.iteration_type == oc.SolverCvode.IterationType.Functional
    assert solver.linear_solver == oc.SolverCvode.LinearSolver.Gmres
    assert solver.preconditioner == oc.SolverCvode.Preconditioner.No
    assert solver.upper_half_bandwidth == 3
    assert solver.lower_half_bandwidth == 5
    assert solver.relative_tolerance == 1.23e-5
    assert solver.absolute_tolerance == 3.45e-7
    assert solver.interpolate_solution == False


def test_forward_euler_solver():
    solver = oc.SolverForwardEuler()

    assert solver.type == oc.Solver.Type.Ode
    assert solver.id == "KISAO:0000030"
    assert solver.name == "Forward Euler"

    assert solver.step == 1.0

    solver.step = 0.123

    assert solver.step == 0.123


def test_fourth_order_runge_kutta_solver():
    solver = oc.SolverFourthOrderRungeKutta()

    assert solver.type == oc.Solver.Type.Ode
    assert solver.id == "KISAO:0000032"
    assert solver.name == "Fourth-order Runge-Kutta"

    assert solver.step == 1.0

    solver.step = 0.123

    assert solver.step == 0.123


def test_heun_solver():
    solver = oc.SolverHeun()

    assert solver.type == oc.Solver.Type.Ode
    assert solver.id == "KISAO:0000301"
    assert solver.name == "Heun"

    assert solver.step == 1.0

    solver.step = 0.123

    assert solver.step == 0.123


def test_kinsol_solver():
    solver = oc.SolverKinsol()

    assert solver.type == oc.Solver.Type.Nla
    assert solver.id == "KISAO:0000282"
    assert solver.name == "KINSOL"

    assert solver.maximum_number_of_iterations == 200
    assert solver.linear_solver == oc.SolverKinsol.LinearSolver.Dense
    assert solver.upper_half_bandwidth == 0
    assert solver.lower_half_bandwidth == 0

    solver.maximum_number_of_iterations = 123
    solver.linear_solver = oc.SolverKinsol.LinearSolver.Gmres
    solver.upper_half_bandwidth = 3
    solver.lower_half_bandwidth = 5

    assert solver.maximum_number_of_iterations == 123
    assert solver.linear_solver == oc.SolverKinsol.LinearSolver.Gmres
    assert solver.upper_half_bandwidth == 3
    assert solver.lower_half_bandwidth == 5


def test_second_order_runge_kutta_solver():
    solver = oc.SolverSecondOrderRungeKutta()

    assert solver.type == oc.Solver.Type.Ode
    assert solver.id == "KISAO:0000381"
    assert solver.name == "Second-order Runge-Kutta"

    assert solver.step == 1.0

    solver.step = 0.123

    assert solver.step == 0.123
