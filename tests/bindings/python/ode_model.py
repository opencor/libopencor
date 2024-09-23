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


# from libopencor import File, Issue, SedDocument, SolverForwardEuler
# import utils
from utils import assert_values


def run(
    document,
    state_values,
    state_abs_tols,
    rate_values,
    rate_abs_tols,
    constant_values,
    constant_abs_tols,
    computed_constant_values,
    computed_constant_abs_tols,
    algebraic_values,
    algebraic_abs_tols,
    compiled=True,
):
    simulation = document.simulations[0]

    simulation.output_end_time = 50.0
    simulation.number_of_steps = 50000

    instance = document.instantiate(compiled)

    instance.run()

    instance_task = instance.tasks[0]

    assert_values(
        instance_task,
        13000,
        state_values,
        state_abs_tols,
        rate_values,
        rate_abs_tols,
        constant_values,
        constant_abs_tols,
        computed_constant_values,
        computed_constant_abs_tols,
        algebraic_values,
        algebraic_abs_tols,
    )
