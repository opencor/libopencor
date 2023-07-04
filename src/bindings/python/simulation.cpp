/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <libopencor>

#include <pybind11/pybind11.h>

namespace py = pybind11;

void simulationApi(py::module_ &m)
{
    // Simulation API.

    py::class_<libOpenCOR::Simulation, std::shared_ptr<libOpenCOR::Simulation>> simulation(m, "Simulation");

    simulation.def(py::init(&libOpenCOR::Simulation::create), "Create a Simulation object.", py::arg("file"));
}
