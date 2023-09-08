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
        .smart_ptr_constructor("Solver", &libOpenCOR::Solver::create)
        .function("solversInfo", &libOpenCOR::Solver::solversInfo);

    EM_ASM({
        Module['Solver']['Type'] = Module['Solver.Type'];

        delete Module['Solver.Type'];
    });

    // SolverInfo API.

    emscripten::class_<libOpenCOR::SolverInfo>("SolverInfo")
        .smart_ptr<libOpenCOR::SolverInfoPtr>("SolverInfo")
        .function("type", &libOpenCOR::SolverInfo::type)
        .function("name", &libOpenCOR::SolverInfo::name)
        .function("properties", &libOpenCOR::SolverInfo::properties);

    // SolverProperty API.

    emscripten::class_<libOpenCOR::SolverProperty>("SolverProperty");
}
