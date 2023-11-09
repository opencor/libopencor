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

void solverApi()
{
    // Solver API.

    emscripten::enum_<libOpenCOR::Solver::Type>("Solver.Type")
        .value("ODE", libOpenCOR::Solver::Type::ODE)
        .value("NLA", libOpenCOR::Solver::Type::NLA);

    emscripten::class_<libOpenCOR::Solver, emscripten::base<libOpenCOR::Logger>>("Solver")
        .function("type", &libOpenCOR::Solver::type)
        .function("id", &libOpenCOR::Solver::id)
        .function("name", &libOpenCOR::Solver::name);

    EM_ASM({
        Module["Solver"]["Type"] = Module["Solver.Type"];

        delete Module["Solver.Type"];
    });

    // SolverOde API.

    emscripten::class_<libOpenCOR::SolverOde, emscripten::base<libOpenCOR::Solver>>("SolverOde");

    // SolverOdeFixedStep API.

    emscripten::class_<libOpenCOR::SolverOdeFixedStep, emscripten::base<libOpenCOR::SolverOde>>("SolverOdeFixedStep")
        .function("step", &libOpenCOR::SolverOdeFixedStep::step)
        .function("setStep", &libOpenCOR::SolverOdeFixedStep::setStep);

    // SolverNla API.

    emscripten::class_<libOpenCOR::SolverNla, emscripten::base<libOpenCOR::Solver>>("SolverNla");

    // SolverCvode API.

    emscripten::enum_<libOpenCOR::SolverCvode::IntegrationMethod>("SolverCvode.IntegrationMethod")
        .value("ADAMS_MOULTON", libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON)
        .value("BDF", libOpenCOR::SolverCvode::IntegrationMethod::BDF);

    emscripten::enum_<libOpenCOR::SolverCvode::IterationType>("SolverCvode.IterationType")
        .value("FUNCTIONAL", libOpenCOR::SolverCvode::IterationType::FUNCTIONAL)
        .value("NEWTON", libOpenCOR::SolverCvode::IterationType::NEWTON);

    emscripten::enum_<libOpenCOR::SolverCvode::LinearSolver>("SolverCvode.LinearSolver")
        .value("DENSE", libOpenCOR::SolverCvode::LinearSolver::DENSE)
        .value("BANDED", libOpenCOR::SolverCvode::LinearSolver::BANDED)
        .value("DIAGONAL", libOpenCOR::SolverCvode::LinearSolver::DIAGONAL)
        .value("GMRES", libOpenCOR::SolverCvode::LinearSolver::GMRES)
        .value("BICGSTAB", libOpenCOR::SolverCvode::LinearSolver::BICGSTAB)
        .value("TFQMR", libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    emscripten::enum_<libOpenCOR::SolverCvode::Preconditioner>("SolverCvode.Preconditioner")
        .value("NO", libOpenCOR::SolverCvode::Preconditioner::NO)
        .value("BANDED", libOpenCOR::SolverCvode::Preconditioner::BANDED);

    emscripten::class_<libOpenCOR::SolverCvode, emscripten::base<libOpenCOR::SolverOde>>("SolverCvode")
        .smart_ptr_constructor("SolverCvode", &libOpenCOR::SolverCvode::create)
        .function("maximumStep", &libOpenCOR::SolverCvode::maximumStep)
        .function("setMaximumStep", &libOpenCOR::SolverCvode::setMaximumStep)
        .function("maximumNumberOfSteps", &libOpenCOR::SolverCvode::maximumNumberOfSteps)
        .function("setMaximumNumberOfSteps", &libOpenCOR::SolverCvode::setMaximumNumberOfSteps)
        .function("integrationMethod", &libOpenCOR::SolverCvode::integrationMethod)
        .function("setIntegrationMethod", &libOpenCOR::SolverCvode::setIntegrationMethod)
        .function("iterationType", &libOpenCOR::SolverCvode::iterationType)
        .function("setIterationType", &libOpenCOR::SolverCvode::setIterationType)
        .function("linearSolver", &libOpenCOR::SolverCvode::linearSolver)
        .function("setLinearSolver", &libOpenCOR::SolverCvode::setLinearSolver)
        .function("preconditioner", &libOpenCOR::SolverCvode::preconditioner)
        .function("setPreconditioner", &libOpenCOR::SolverCvode::setPreconditioner)
        .function("upperHalfBandwidth", &libOpenCOR::SolverCvode::upperHalfBandwidth)
        .function("setUpperHalfBandwidth", &libOpenCOR::SolverCvode::setUpperHalfBandwidth)
        .function("lowerHalfBandwidth", &libOpenCOR::SolverCvode::lowerHalfBandwidth)
        .function("setLowerHalfBandwidth", &libOpenCOR::SolverCvode::setLowerHalfBandwidth)
        .function("relativeTolerance", &libOpenCOR::SolverCvode::relativeTolerance)
        .function("setRelativeTolerance", &libOpenCOR::SolverCvode::setRelativeTolerance)
        .function("absoluteTolerance", &libOpenCOR::SolverCvode::absoluteTolerance)
        .function("setAbsoluteTolerance", &libOpenCOR::SolverCvode::setAbsoluteTolerance)
        .function("interpolateSolution", &libOpenCOR::SolverCvode::interpolateSolution)
        .function("setInterpolateSolution", &libOpenCOR::SolverCvode::setInterpolateSolution);

    EM_ASM({
        Module["SolverCvode"]["IntegrationMethod"] = Module["SolverCvode.IntegrationMethod"];
        Module["SolverCvode"]["IterationType"] = Module["SolverCvode.IterationType"];
        Module["SolverCvode"]["LinearSolver"] = Module["SolverCvode.LinearSolver"];
        Module["SolverCvode"]["Preconditioner"] = Module["SolverCvode.Preconditioner"];

        delete Module["SolverCvode.IntegrationMethod"];
        delete Module["SolverCvode.IterationType"];
        delete Module["SolverCvode.LinearSolver"];
        delete Module["SolverCvode.Preconditioner"];
    });

    // SolverForwardEuler API.

    emscripten::class_<libOpenCOR::SolverForwardEuler, emscripten::base<libOpenCOR::SolverOdeFixedStep>>("SolverForwardEuler")
        .smart_ptr_constructor("SolverForwardEuler", &libOpenCOR::SolverForwardEuler::create);

    // SolverFourthOrderRungeKutta API.

    emscripten::class_<libOpenCOR::SolverFourthOrderRungeKutta, emscripten::base<libOpenCOR::SolverOdeFixedStep>>("SolverFourthOrderRungeKutta")
        .smart_ptr_constructor("SolverFourthOrderRungeKutta", &libOpenCOR::SolverFourthOrderRungeKutta::create);

    // SolverHeun API.

    emscripten::class_<libOpenCOR::SolverHeun, emscripten::base<libOpenCOR::SolverOdeFixedStep>>("SolverHeun")
        .smart_ptr_constructor("SolverHeun", &libOpenCOR::SolverHeun::create);

    // SolverKinsol API.

    emscripten::enum_<libOpenCOR::SolverKinsol::LinearSolver>("SolverKinsol.LinearSolver")
        .value("DENSE", libOpenCOR::SolverKinsol::LinearSolver::DENSE)
        .value("BANDED", libOpenCOR::SolverKinsol::LinearSolver::BANDED)
        .value("GMRES", libOpenCOR::SolverKinsol::LinearSolver::GMRES)
        .value("BICGSTAB", libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB)
        .value("TFQMR", libOpenCOR::SolverKinsol::LinearSolver::TFQMR);

    emscripten::class_<libOpenCOR::SolverKinsol, emscripten::base<libOpenCOR::SolverNla>>("SolverKinsol")
        .smart_ptr_constructor("SolverKinsol", &libOpenCOR::SolverKinsol::create)
        .function("maximumNumberOfIterations", &libOpenCOR::SolverKinsol::maximumNumberOfIterations)
        .function("setMaximumNumberOfIterations", &libOpenCOR::SolverKinsol::setMaximumNumberOfIterations)
        .function("linearSolver", &libOpenCOR::SolverKinsol::linearSolver)
        .function("setLinearSolver", &libOpenCOR::SolverKinsol::setLinearSolver)
        .function("upperHalfBandwidth", &libOpenCOR::SolverKinsol::upperHalfBandwidth)
        .function("setUpperHalfBandwidth", &libOpenCOR::SolverKinsol::setUpperHalfBandwidth)
        .function("lowerHalfBandwidth", &libOpenCOR::SolverKinsol::lowerHalfBandwidth)
        .function("setLowerHalfBandwidth", &libOpenCOR::SolverKinsol::setLowerHalfBandwidth);

    EM_ASM({
        Module["SolverKinsol"]["LinearSolver"] = Module["SolverKinsol.LinearSolver"];

        delete Module["SolverKinsol.LinearSolver"];
    });

    // SolverSecondOrderRungeKutta API.

    emscripten::class_<libOpenCOR::SolverSecondOrderRungeKutta, emscripten::base<libOpenCOR::SolverOdeFixedStep>>("SolverSecondOrderRungeKutta")
        .smart_ptr_constructor("SolverSecondOrderRungeKutta", &libOpenCOR::SolverSecondOrderRungeKutta::create);
}
