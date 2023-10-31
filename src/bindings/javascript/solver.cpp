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
    emscripten::enum_<libOpenCOR::Solver::Type>("Solver.Type")
        .value("ODE", libOpenCOR::Solver::Type::ODE)
        .value("NLA", libOpenCOR::Solver::Type::NLA);

    emscripten::class_<libOpenCOR::Solver, emscripten::base<libOpenCOR::Logger>>("Solver")
        .smart_ptr_constructor("Solver", &libOpenCOR::Solver::create)
        .function("property", &libOpenCOR::Solver::property)
        .function("setProperty", &libOpenCOR::Solver::setProperty)
        .function("properties", &libOpenCOR::Solver::properties)
        .function("setProperties", &libOpenCOR::Solver::setProperties)
        .function("type", &libOpenCOR::Solver::type)
        .function("id", &libOpenCOR::Solver::id)
        .function("name", &libOpenCOR::Solver::name);

    EM_ASM({
        Module["Solver"]["Type"] = Module["Solver.Type"];

        delete Module["Solver.Type"];
    });
}
