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

    solver.def_property_readonly("type", &libOpenCOR::Solver::type, "Get the type of the Solver object.")
        .def_property_readonly("id", &libOpenCOR::Solver::id, "Get the (KiSAO) id of the Solver object.")
        .def_property_readonly("name", &libOpenCOR::Solver::name, "Get the name of the Solver object.");

    // SolverOde API.

    py::class_<libOpenCOR::SolverOde, libOpenCOR::Solver, libOpenCOR::SolverOdePtr> solverOde(m, "SolverOde");

    // SolverOdeFixedStep API.

    py::class_<libOpenCOR::SolverOdeFixedStep, libOpenCOR::SolverOde, libOpenCOR::SolverOdeFixedStepPtr> solverOdeFixedStep(m, "SolverOdeFixedStep");

    solverOdeFixedStep.def_property("step", &libOpenCOR::SolverOdeFixedStep::step, &libOpenCOR::SolverOdeFixedStep::setStep, "The step of the SolverOdeFixedStep object.");

    // SolverNla API.

    py::class_<libOpenCOR::SolverNla, libOpenCOR::Solver, libOpenCOR::SolverNlaPtr> solverNla(m, "SolverNla");

    // SolverCvode API.

    py::class_<libOpenCOR::SolverCvode, libOpenCOR::SolverOde, libOpenCOR::SolverCvodePtr> solverCvode(m, "SolverCvode");

    py::enum_<libOpenCOR::SolverCvode::IntegrationMethod>(solverCvode, "IntegrationMethod")
        .value("AdamsMoulton", libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON)
        .value("Bdf", libOpenCOR::SolverCvode::IntegrationMethod::BDF)
        .export_values();

    py::enum_<libOpenCOR::SolverCvode::IterationType>(solverCvode, "IterationType")
        .value("Functional", libOpenCOR::SolverCvode::IterationType::FUNCTIONAL)
        .value("Newton", libOpenCOR::SolverCvode::IterationType::NEWTON)
        .export_values();

    py::enum_<libOpenCOR::SolverCvode::LinearSolver>(solverCvode, "LinearSolver")
        .value("Dense", libOpenCOR::SolverCvode::LinearSolver::DENSE)
        .value("Banded", libOpenCOR::SolverCvode::LinearSolver::BANDED)
        .value("Diagonal", libOpenCOR::SolverCvode::LinearSolver::DIAGONAL)
        .value("Gmres", libOpenCOR::SolverCvode::LinearSolver::GMRES)
        .value("Bicgstab", libOpenCOR::SolverCvode::LinearSolver::BICGSTAB)
        .value("Tfqmr", libOpenCOR::SolverCvode::LinearSolver::TFQMR)
        .export_values();

    py::enum_<libOpenCOR::SolverCvode::Preconditioner>(solverCvode, "Preconditioner")
        .value("No", libOpenCOR::SolverCvode::Preconditioner::NO)
        .value("Banded", libOpenCOR::SolverCvode::Preconditioner::BANDED)
        .export_values();

    solverCvode.def(py::init(&libOpenCOR::SolverCvode::create), "Create a SolverCvode object.")
        .def_property("maximum_step", &libOpenCOR::SolverCvode::maximumStep, &libOpenCOR::SolverCvode::setMaximumStep, "The maximum step of the SolverCvode object.")
        .def_property("maximum_number_of_steps", &libOpenCOR::SolverCvode::maximumNumberOfSteps, &libOpenCOR::SolverCvode::setMaximumNumberOfSteps, "The maximum number of steps of the SolverCvode object.")
        .def_property("integration_method", &libOpenCOR::SolverCvode::integrationMethod, &libOpenCOR::SolverCvode::setIntegrationMethod, "The integration method of the SolverCvode object.")
        .def_property("iteration_type", &libOpenCOR::SolverCvode::iterationType, &libOpenCOR::SolverCvode::setIterationType, "The iteration type of the SolverCvode object.")
        .def_property("linear_solver", &libOpenCOR::SolverCvode::linearSolver, &libOpenCOR::SolverCvode::setLinearSolver, "The linear solver of the SolverCvode object.")
        .def_property("preconditioner", &libOpenCOR::SolverCvode::preconditioner, &libOpenCOR::SolverCvode::setPreconditioner, "The preconditioner of the SolverCvode object.")
        .def_property("upper_half_bandwidth", &libOpenCOR::SolverCvode::upperHalfBandwidth, &libOpenCOR::SolverCvode::setUpperHalfBandwidth, "The upper half-bandwidth of the SolverCvode object.")
        .def_property("lower_half_bandwidth", &libOpenCOR::SolverCvode::lowerHalfBandwidth, &libOpenCOR::SolverCvode::setLowerHalfBandwidth, "The lower half-bandwidth of the SolverCvode object.")
        .def_property("relative_tolerance", &libOpenCOR::SolverCvode::relativeTolerance, &libOpenCOR::SolverCvode::setRelativeTolerance, "The relative tolerance of the SolverCvode object.")
        .def_property("absolute_tolerance", &libOpenCOR::SolverCvode::absoluteTolerance, &libOpenCOR::SolverCvode::setAbsoluteTolerance, "The absolute tolerance of the SolverCvode object.")
        .def_property("interpolate_solution", &libOpenCOR::SolverCvode::interpolateSolution, &libOpenCOR::SolverCvode::setInterpolateSolution, "The interpolate solution of the SolverCvode object.");

    // SolverForwardEuler API.

    py::class_<libOpenCOR::SolverForwardEuler, libOpenCOR::SolverOdeFixedStep, libOpenCOR::SolverForwardEulerPtr> solverForwardEuler(m, "SolverForwardEuler");

    solverForwardEuler.def(py::init(&libOpenCOR::SolverForwardEuler::create), "Create a SolverForwardEuler object.");

    // SolverFourthOrderRungeKutta API.

    py::class_<libOpenCOR::SolverFourthOrderRungeKutta, libOpenCOR::SolverOdeFixedStep, libOpenCOR::SolverFourthOrderRungeKuttaPtr> solverFourthOrderRungeKutta(m, "SolverFourthOrderRungeKutta");

    solverFourthOrderRungeKutta.def(py::init(&libOpenCOR::SolverFourthOrderRungeKutta::create), "Create a SolverFourthOrderRungeKutta object.");

    // SolverHeun API.

    py::class_<libOpenCOR::SolverHeun, libOpenCOR::SolverOdeFixedStep, libOpenCOR::SolverHeunPtr> solverHeun(m, "SolverHeun");

    solverHeun.def(py::init(&libOpenCOR::SolverHeun::create), "Create a SolverHeun object.");

    // SolverKinsol API.

    py::class_<libOpenCOR::SolverKinsol, libOpenCOR::SolverNla, libOpenCOR::SolverKinsolPtr> solverKinsol(m, "SolverKinsol");

    py::enum_<libOpenCOR::SolverKinsol::LinearSolver>(solverKinsol, "LinearSolver")
        .value("Dense", libOpenCOR::SolverKinsol::LinearSolver::DENSE)
        .value("Banded", libOpenCOR::SolverKinsol::LinearSolver::BANDED)
        .value("Gmres", libOpenCOR::SolverKinsol::LinearSolver::GMRES)
        .value("Bicgstab", libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB)
        .value("Tfqmr", libOpenCOR::SolverKinsol::LinearSolver::TFQMR)
        .export_values();

    solverKinsol.def(py::init(&libOpenCOR::SolverKinsol::create), "Create a SolverKinsol object.")
        .def_property("maximum_number_of_iterations", &libOpenCOR::SolverKinsol::maximumNumberOfIterations, &libOpenCOR::SolverKinsol::setMaximumNumberOfIterations, "The maximum number of iterations of the SolverKinsol object.")
        .def_property("linear_solver", &libOpenCOR::SolverKinsol::linearSolver, &libOpenCOR::SolverKinsol::setLinearSolver, "The linear solver of the SolverKinsol object.")
        .def_property("upper_half_bandwidth", &libOpenCOR::SolverKinsol::upperHalfBandwidth, &libOpenCOR::SolverKinsol::setUpperHalfBandwidth, "The upper half-bandwidth of the SolverKinsol object.")
        .def_property("lower_half_bandwidth", &libOpenCOR::SolverKinsol::lowerHalfBandwidth, &libOpenCOR::SolverKinsol::setLowerHalfBandwidth, "The lower half-bandwidth of the SolverKinsol object.");

    // SolverSecondOrderRungeKutta API.

    py::class_<libOpenCOR::SolverSecondOrderRungeKutta, libOpenCOR::SolverOdeFixedStep, libOpenCOR::SolverSecondOrderRungeKuttaPtr> solverSecondOrderRungeKutta(m, "SolverSecondOrderRungeKutta");

    solverSecondOrderRungeKutta.def(py::init(&libOpenCOR::SolverSecondOrderRungeKutta::create), "Create a SolverSecondOrderRungeKutta object.");
}
