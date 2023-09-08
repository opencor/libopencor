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


def test_solvers_info():
    solvers_info = Solver.solvers_info()

    assert len(solvers_info) == 1


def test_unknown_solver():
    assert Solver("Unknown solver") == None


def test_forward_euler():
    assert Solver("Forward Euler") != None
