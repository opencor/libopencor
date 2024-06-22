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

#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>

namespace nb = nanobind;

void solverApi(nb::module_ &m)
{
    // Solver API.

    nb::class_<libOpenCOR::Solver, libOpenCOR::Logger> solver(m, "Solver");

    nb::enum_<libOpenCOR::Solver::Type>(solver, "Type")
        .value("Ode", libOpenCOR::Solver::Type::ODE)
        .value("Nla", libOpenCOR::Solver::Type::NLA)
        .export_values();

    solver.def_prop_ro("type", &libOpenCOR::Solver::type, "Get the type of the Solver object.")
        .def_prop_ro("id", &libOpenCOR::Solver::id, "Get the (KiSAO) id of the Solver object.")
        .def_prop_ro("name", &libOpenCOR::Solver::name, "Get the name of the Solver object.");

    // SolverOde API.

    nb::class_<libOpenCOR::SolverOde, libOpenCOR::Solver> solverOde(m, "SolverOde");

    // SolverOdeFixedStep API.

    nb::class_<libOpenCOR::SolverOdeFixedStep, libOpenCOR::SolverOde> solverOdeFixedStep(m, "SolverOdeFixedStep");

    solverOdeFixedStep.def_prop_rw("step", &libOpenCOR::SolverOdeFixedStep::step, &libOpenCOR::SolverOdeFixedStep::setStep, "The step of the SolverOdeFixedStep object.");

    // SolverNla API.

    nb::class_<libOpenCOR::SolverNla, libOpenCOR::Solver> solverNla(m, "SolverNla");

    // SolverCvode API.

    nb::class_<libOpenCOR::SolverCvode, libOpenCOR::SolverOde> solverCvode(m, "SolverCvode");

    nb::enum_<libOpenCOR::SolverCvode::IntegrationMethod>(solverCvode, "IntegrationMethod")
        .value("AdamsMoulton", libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON)
        .value("Bdf", libOpenCOR::SolverCvode::IntegrationMethod::BDF)
        .export_values();

    nb::enum_<libOpenCOR::SolverCvode::IterationType>(solverCvode, "IterationType")
        .value("Functional", libOpenCOR::SolverCvode::IterationType::FUNCTIONAL)
        .value("Newton", libOpenCOR::SolverCvode::IterationType::NEWTON)
        .export_values();

    nb::enum_<libOpenCOR::SolverCvode::LinearSolver>(solverCvode, "LinearSolver")
        .value("Dense", libOpenCOR::SolverCvode::LinearSolver::DENSE)
        .value("Banded", libOpenCOR::SolverCvode::LinearSolver::BANDED)
        .value("Diagonal", libOpenCOR::SolverCvode::LinearSolver::DIAGONAL)
        .value("Gmres", libOpenCOR::SolverCvode::LinearSolver::GMRES)
        .value("Bicgstab", libOpenCOR::SolverCvode::LinearSolver::BICGSTAB)
        .value("Tfqmr", libOpenCOR::SolverCvode::LinearSolver::TFQMR)
        .export_values();

    nb::enum_<libOpenCOR::SolverCvode::Preconditioner>(solverCvode, "Preconditioner")
        .value("No", libOpenCOR::SolverCvode::Preconditioner::NO)
        .value("Banded", libOpenCOR::SolverCvode::Preconditioner::BANDED)
        .export_values();

    solverCvode.def(nb::new_(&libOpenCOR::SolverCvode::create), "Create a SolverCvode object.")
        .def_prop_rw("maximum_step", &libOpenCOR::SolverCvode::maximumStep, &libOpenCOR::SolverCvode::setMaximumStep, "The maximum step of the SolverCvode object.")
        .def_prop_rw("maximum_number_of_steps", &libOpenCOR::SolverCvode::maximumNumberOfSteps, &libOpenCOR::SolverCvode::setMaximumNumberOfSteps, "The maximum number of steps of the SolverCvode object.")
        .def_prop_rw("integration_method", &libOpenCOR::SolverCvode::integrationMethod, &libOpenCOR::SolverCvode::setIntegrationMethod, "The integration method of the SolverCvode object.")
        .def_prop_rw("iteration_type", &libOpenCOR::SolverCvode::iterationType, &libOpenCOR::SolverCvode::setIterationType, "The iteration type of the SolverCvode object.")
        .def_prop_rw("linear_solver", &libOpenCOR::SolverCvode::linearSolver, &libOpenCOR::SolverCvode::setLinearSolver, "The linear solver of the SolverCvode object.")
        .def_prop_rw("preconditioner", &libOpenCOR::SolverCvode::preconditioner, &libOpenCOR::SolverCvode::setPreconditioner, "The preconditioner of the SolverCvode object.")
        .def_prop_rw("upper_half_bandwidth", &libOpenCOR::SolverCvode::upperHalfBandwidth, &libOpenCOR::SolverCvode::setUpperHalfBandwidth, "The upper half-bandwidth of the SolverCvode object.")
        .def_prop_rw("lower_half_bandwidth", &libOpenCOR::SolverCvode::lowerHalfBandwidth, &libOpenCOR::SolverCvode::setLowerHalfBandwidth, "The lower half-bandwidth of the SolverCvode object.")
        .def_prop_rw("relative_tolerance", &libOpenCOR::SolverCvode::relativeTolerance, &libOpenCOR::SolverCvode::setRelativeTolerance, "The relative tolerance of the SolverCvode object.")
        .def_prop_rw("absolute_tolerance", &libOpenCOR::SolverCvode::absoluteTolerance, &libOpenCOR::SolverCvode::setAbsoluteTolerance, "The absolute tolerance of the SolverCvode object.")
        .def_prop_rw("interpolate_solution", &libOpenCOR::SolverCvode::interpolateSolution, &libOpenCOR::SolverCvode::setInterpolateSolution, "The interpolate solution of the SolverCvode object.");

    // SolverForwardEuler API.

    nb::class_<libOpenCOR::SolverForwardEuler, libOpenCOR::SolverOdeFixedStep> solverForwardEuler(m, "SolverForwardEuler");

    solverForwardEuler.def(nb::new_(&libOpenCOR::SolverForwardEuler::create), "Create a SolverForwardEuler object.");

    // SolverFourthOrderRungeKutta API.

    nb::class_<libOpenCOR::SolverFourthOrderRungeKutta, libOpenCOR::SolverOdeFixedStep> solverFourthOrderRungeKutta(m, "SolverFourthOrderRungeKutta");

    solverFourthOrderRungeKutta.def(nb::new_(&libOpenCOR::SolverFourthOrderRungeKutta::create), "Create a SolverFourthOrderRungeKutta object.");

    // SolverHeun API.

    nb::class_<libOpenCOR::SolverHeun, libOpenCOR::SolverOdeFixedStep> solverHeun(m, "SolverHeun");

    solverHeun.def(nb::new_(&libOpenCOR::SolverHeun::create), "Create a SolverHeun object.");

    // SolverKinsol API.

    nb::class_<libOpenCOR::SolverKinsol, libOpenCOR::SolverNla> solverKinsol(m, "SolverKinsol");

    nb::enum_<libOpenCOR::SolverKinsol::LinearSolver>(solverKinsol, "LinearSolver")
        .value("Dense", libOpenCOR::SolverKinsol::LinearSolver::DENSE)
        .value("Banded", libOpenCOR::SolverKinsol::LinearSolver::BANDED)
        .value("Gmres", libOpenCOR::SolverKinsol::LinearSolver::GMRES)
        .value("Bicgstab", libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB)
        .value("Tfqmr", libOpenCOR::SolverKinsol::LinearSolver::TFQMR)
        .export_values();

    solverKinsol.def(nb::new_(&libOpenCOR::SolverKinsol::create), "Create a SolverKinsol object.")
        .def_prop_rw("maximum_number_of_iterations", &libOpenCOR::SolverKinsol::maximumNumberOfIterations, &libOpenCOR::SolverKinsol::setMaximumNumberOfIterations, "The maximum number of iterations of the SolverKinsol object.")
        .def_prop_rw("linear_solver", &libOpenCOR::SolverKinsol::linearSolver, &libOpenCOR::SolverKinsol::setLinearSolver, "The linear solver of the SolverKinsol object.")
        .def_prop_rw("upper_half_bandwidth", &libOpenCOR::SolverKinsol::upperHalfBandwidth, &libOpenCOR::SolverKinsol::setUpperHalfBandwidth, "The upper half-bandwidth of the SolverKinsol object.")
        .def_prop_rw("lower_half_bandwidth", &libOpenCOR::SolverKinsol::lowerHalfBandwidth, &libOpenCOR::SolverKinsol::setLowerHalfBandwidth, "The lower half-bandwidth of the SolverKinsol object.");

    // SolverSecondOrderRungeKutta API.

    nb::class_<libOpenCOR::SolverSecondOrderRungeKutta, libOpenCOR::SolverOdeFixedStep> solverSecondOrderRungeKutta(m, "SolverSecondOrderRungeKutta");

    solverSecondOrderRungeKutta.def(nb::new_(&libOpenCOR::SolverSecondOrderRungeKutta::create), "Create a SolverSecondOrderRungeKutta object.");
}
