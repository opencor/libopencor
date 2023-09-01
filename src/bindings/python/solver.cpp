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

    py::class_<libOpenCOR::Solver, libOpenCOR::Logger, std::shared_ptr<libOpenCOR::Solver>> solver(m, "Solver");
    py::class_<libOpenCOR::SolverForwardEuler, libOpenCOR::Solver, std::shared_ptr<libOpenCOR::SolverForwardEuler>> solverForwardEuler(m, "SolverForwardEuler");

    py::enum_<libOpenCOR::Solver::Type>(solver, "Type")
        .value("Ode", libOpenCOR::Solver::Type::ODE)
        .value("Nla", libOpenCOR::Solver::Type::NLA)
        .export_values();

    py::enum_<libOpenCOR::Solver::Method>(solver, "Method")
        .value("Cvode", libOpenCOR::Solver::Method::CVODE)
        .value("ForwardEuler", libOpenCOR::Solver::Method::FORWARD_EULER)
        .value("FourthOrderRungeKutta", libOpenCOR::Solver::Method::FOURTH_ORDER_RUNGE_KUTTA)
        .value("Heun", libOpenCOR::Solver::Method::HEUN)
        .value("Kinsol", libOpenCOR::Solver::Method::KINSOL)
        .value("SecondOrderRungeKutta", libOpenCOR::Solver::Method::SECOND_ORDER_RUNGE_KUTTA)
        .export_values();

    solver.def(py::init(&libOpenCOR::Solver::create), "Create a Solver object.")
        .def_property_readonly("type", &libOpenCOR::Solver::type, "Get the type of this Solver object.")
        .def_property_readonly("method", &libOpenCOR::Solver::method, "Get the method used by this Solver object.")
        .def_property_readonly("name", &libOpenCOR::Solver::name, "Get the name of this Solver object.");
}
