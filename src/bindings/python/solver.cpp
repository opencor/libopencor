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
    // Solver API.

    py::class_<libOpenCOR::Solver, libOpenCOR::Logger, libOpenCOR::SolverPtr> solver(m, "Solver");

    py::enum_<libOpenCOR::Solver::Type>(solver, "Type")
        .value("Ode", libOpenCOR::Solver::Type::ODE)
        .value("Nla", libOpenCOR::Solver::Type::NLA)
        .export_values();

    solver.def(py::init(&libOpenCOR::Solver::create), "Create a Solver object.")
        .def_static("solvers_info", &libOpenCOR::Solver::solversInfo, "Get the solvers information.");

    // SolverInfo API.

    py::class_<libOpenCOR::SolverInfo, libOpenCOR::SolverInfoPtr> solverInfo(m, "SolverInfo");

    solverInfo.def_property_readonly("type", &libOpenCOR::SolverInfo::type, "Get the type of the Solver object.")
        .def_property_readonly("name", &libOpenCOR::SolverInfo::name, "Get the name of the Solver object.")
        .def_property_readonly("properties", &libOpenCOR::SolverInfo::properties, "Get the properties of the Solver object.");

    // SolverProperty API.

    py::class_<libOpenCOR::SolverProperty, libOpenCOR::SolverPropertyPtr> solverProperty(m, "SolverProperty");
}
