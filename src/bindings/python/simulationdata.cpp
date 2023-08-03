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

void simulationDataApi(py::module_ &m)
{
    // SimulationData API.

    py::class_<libOpenCOR::SimulationData, libOpenCOR::Logger, std::shared_ptr<libOpenCOR::SimulationData>> simulationData(m, "SimulationData");

    simulationData.def("is_valid", &libOpenCOR::SimulationData::isValid, "Return whether the SimulationData object is valid.");
}
