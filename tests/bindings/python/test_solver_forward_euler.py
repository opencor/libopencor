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


from libopencor import Issue, SolverOde
import os.path
import sys

sys.path.append(
    os.path.abspath(
        os.path.dirname(os.path.abspath("__file__"))
        + "/../../../../tests/res/hh52/python"
    )
)

from model import *
from utils import assert_issues


def create_and_initialise_arrays_and_create_solver():
    states = create_states_array()
    rates = create_states_array()
    variables = create_variables_array()

    initialise_variables(states, rates, variables)
    compute_computed_constants(variables)
    compute_rates(0.0, states, rates, variables)
    compute_variables(0.0, states, rates, variables)

    ode_solver = SolverOde("Forward Euler")

    return states, rates, variables, ode_solver


def delete_arrays(states, rates, variables):
    del states
    del rates
    del variables


def test_non_floating_step_value():
    f = open("/Users/Alan/Desktop/output.txt", "w")

    # Create and initialise our various arrays and create our ODE solver.

    (
        states,
        rates,
        variables,
        ode_solver,
    ) = create_and_initialise_arrays_and_create_solver()

    # Customise and initialise our ODE solver using a step value that is not a floating point number.

    expected_issues = [
        [
            Issue.Type.Error,
            'The "Step" property has an invalid value ("abc"). It must be a floating point number greater than zero.',
        ],
    ]

    ode_solver.set_property("Step", "abc")
    f.write(states.__str__())
    # x = type(states)
    # f.write(x)

    assert (
        ode_solver.initialise(STATE_COUNT, states, rates, variables, compute_rates)
        == False
    )
    assert_issues(ode_solver, expected_issues)

    # Clean up after ourselves.

    delete_arrays(states, rates, variables)

    f.close()


# def test_invalid_step_value():
#     # Create and initialise our various arrays and create our ODE solver.

#     (
#         states,
#         rates,
#         variables,
#         ode_solver,
#     ) = create_and_initialise_arrays_and_create_solver()

#     # Customise and initialise our ODE solver using a step value that is not a floating point number.

#     expected_issues = [
#         [
#             Issue.Type.Error,
#             'The "Step" property has an invalid value ("0.0"). It must be a floating point number greater than zero.',
#         ],
#     ]

#     ode_solver.set_property("Step", "0.0")

#     assert (
#         ode_solver.initialise(
#             STATE_COUNT, states, rates, variables, compute_rates
#         )
#         == False
#     )
#     assert_issues(ode_solver, expected_issues)

#     # Clean up after ourselves.

#     delete_arrays(states, rates, variables)


# def test_main():
#     # Create and initialise our various arrays and create our ODE solver.

#     (
#         states,
#         rates,
#         variables,
#         ode_solver,
#     ) = create_and_initialise_arrays_and_create_solver()

#     # Customise and initialise our ODE solver.

#     ode_solver.set_property("Step", "0.0123")

#     assert (
#         ode_solver.initialise(
#             STATE_COUNT, states, rates, variables, compute_rates
#         )
#         == True
#     )
#     assert_doubles(states, [0.0, 0.6, 0.05, 0.325])

#     # Clean up after ourselves.

#     delete_arrays(states, rates, variables)
