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
    // SolverInfo API.

    py::class_<libOpenCOR::SolverInfo, libOpenCOR::SolverInfoPtr> solverInfo(m, "SolverInfo");

    solverInfo.def_property_readonly("properties", &libOpenCOR::SolverInfo::properties, "Get the properties of the Solver object.")
        .def("hidden_properties", &libOpenCOR::SolverInfo::hiddenProperties, "Get the properties of the Solver object that should be hidden.");

    // SolverProperty API.

    py::class_<libOpenCOR::SolverProperty, libOpenCOR::SolverPropertyPtr> solverProperty(m, "SolverProperty");

    py::enum_<libOpenCOR::SolverProperty::Type>(solverProperty, "Type")
        .value("Boolean", libOpenCOR::SolverProperty::Type::Boolean)
        .value("Integer", libOpenCOR::SolverProperty::Type::Integer)
        .value("IntegerGt0", libOpenCOR::SolverProperty::Type::IntegerGt0)
        .value("IntegerGe0", libOpenCOR::SolverProperty::Type::IntegerGe0)
        .value("Double", libOpenCOR::SolverProperty::Type::Double)
        .value("DoubleGt0", libOpenCOR::SolverProperty::Type::DoubleGt0)
        .value("DoubleGe0", libOpenCOR::SolverProperty::Type::DoubleGe0)
        .value("List", libOpenCOR::SolverProperty::Type::List)
        .export_values();

    solverProperty.def_property_readonly("type", &libOpenCOR::SolverProperty::type, "Get the type of the SolverProperty object.")
        .def_property_readonly("id", &libOpenCOR::SolverProperty::id, "Get the (KiSAO) id of the SolverProperty object.")
        .def_property_readonly("name", &libOpenCOR::SolverProperty::name, "Get the name of the SolverProperty object.")
        .def_property_readonly("list_values", &libOpenCOR::SolverProperty::listValues, "Get the list of values of the SolverProperty object.")
        .def_property_readonly("default_value", &libOpenCOR::SolverProperty::defaultValue, "Get the default value of the SolverProperty object.")
        .def_property_readonly("has_voi_unit", &libOpenCOR::SolverProperty::hasVoiUnit, "Get whether the SolverProperty object has VOI unit.");

    // Solver API.

    py::class_<libOpenCOR::Solver, libOpenCOR::Logger, libOpenCOR::SolverPtr> solver(m, "Solver");

    py::enum_<libOpenCOR::Solver::Type>(solver, "Type")
        .value("Ode", libOpenCOR::Solver::Type::ODE)
        .value("Nla", libOpenCOR::Solver::Type::NLA)
        .export_values();

    solver.def(py::init(&libOpenCOR::Solver::create), "Create a Solver object.")
        .def_static("solvers_info", &libOpenCOR::Solver::solversInfo, "Get the solvers information.")
        .def_property_readonly("is_valid", &libOpenCOR::Solver::isValid, "Get whether the Solver object is valid.")
        .def("property", &libOpenCOR::Solver::property, "Get the property of the Solver object.")
        .def("set_property", &libOpenCOR::Solver::setProperty, "Set the property of the Solver object.")
        .def_property_readonly("properties", &libOpenCOR::Solver::properties, "Get the properties of the Solver object.")
        .def("set_properties", &libOpenCOR::Solver::setProperties, "Set the properties of the Solver object.")
        .def_property_readonly("type", &libOpenCOR::Solver::type, "Get the type of the Solver object.")
        .def_property_readonly("id", &libOpenCOR::Solver::id, "Get the (KiSAO) id of the Solver object.")
        .def_property_readonly("name", &libOpenCOR::Solver::name, "Get the name of the Solver object.");
}
