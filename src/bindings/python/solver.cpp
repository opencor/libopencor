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
#include <pybind11/stl.h>

namespace py = pybind11;

void solverApi(py::module_ &m)
{
    py::enum_<libOpenCOR::Solver::Type>(solver, "Type")
        .value("Ode", libOpenCOR::Solver::Type::ODE)
        .value("Nla", libOpenCOR::Solver::Type::NLA)
        .export_values();

    solver.def(py::init(&libOpenCOR::Solver::create), "Create a Solver object.")
        .def("property", &libOpenCOR::Solver::property, "Get the property of the Solver object.")
        .def("set_property", &libOpenCOR::Solver::setProperty, "Set the property of the Solver object.")
        .def_property_readonly("properties", &libOpenCOR::Solver::properties, "Get the properties of the Solver object.")
        .def("set_properties", &libOpenCOR::Solver::setProperties, "Set the properties of the Solver object.")
        .def_property_readonly("type", &libOpenCOR::Solver::type, "Get the type of the Solver object.")
        .def_property_readonly("id", &libOpenCOR::Solver::id, "Get the (KiSAO) id of the Solver object.")
        .def_property_readonly("name", &libOpenCOR::Solver::name, "Get the name of the Solver object.");
}
