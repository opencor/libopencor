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
    // SolverInfo API.

    emscripten::class_<libOpenCOR::SolverInfo>("SolverInfo")
        .smart_ptr<libOpenCOR::SolverInfoPtr>("SolverInfo")
        .function("type", &libOpenCOR::SolverInfo::type)
        .function("name", &libOpenCOR::SolverInfo::name)
        .function("kisaoId", &libOpenCOR::SolverInfo::kisaoId)
        .function("properties", &libOpenCOR::SolverInfo::properties);

    emscripten::register_vector<libOpenCOR::SolverInfoPtr>("SolversInfo");

    // SolverProperty API.

    emscripten::enum_<libOpenCOR::SolverProperty::Type>("SolverProperty.Type")
        .value("Boolean", libOpenCOR::SolverProperty::Type::Boolean)
        .value("Integer", libOpenCOR::SolverProperty::Type::Integer)
        .value("IntegerGt0", libOpenCOR::SolverProperty::Type::IntegerGt0)
        .value("IntegerGe0", libOpenCOR::SolverProperty::Type::IntegerGe0)
        .value("Double", libOpenCOR::SolverProperty::Type::Double)
        .value("DoubleGt0", libOpenCOR::SolverProperty::Type::DoubleGt0)
        .value("DoubleGe0", libOpenCOR::SolverProperty::Type::DoubleGe0)
        .value("List", libOpenCOR::SolverProperty::Type::List);

    emscripten::class_<libOpenCOR::SolverProperty>("SolverProperty")
        .smart_ptr<libOpenCOR::SolverPropertyPtr>("SolverProperty")
        .function("type", &libOpenCOR::SolverProperty::type)
        .function("name", &libOpenCOR::SolverProperty::name)
        .function("kisaoId", &libOpenCOR::SolverProperty::kisaoId)
        .function("listValues", &libOpenCOR::SolverProperty::listValues)
        .function("defaultValue", &libOpenCOR::SolverProperty::defaultValue)
        .function("hasVoiUnit", &libOpenCOR::SolverProperty::hasVoiUnit);

    emscripten::register_vector<libOpenCOR::SolverPropertyPtr>("SolverProperties");

    EM_ASM({
        Module['SolverProperty']['Type'] = Module['SolverProperty.Type'];

        delete Module['SolverProperty.Type'];
    });

    // Solver API.

    emscripten::enum_<libOpenCOR::Solver::Type>("Solver.Type")
        .value("ODE", libOpenCOR::Solver::Type::ODE)
        .value("NLA", libOpenCOR::Solver::Type::NLA);

    emscripten::register_map<std::string, std::string>("Properties");

    emscripten::class_<libOpenCOR::Solver, emscripten::base<libOpenCOR::Logger>>("Solver")
        .smart_ptr<libOpenCOR::SolverPtr>("Solver")
        .class_function("solversInfo", &libOpenCOR::Solver::solversInfo)
        .function("isValid", &libOpenCOR::Solver::isValid)
        .function("properties", &libOpenCOR::Solver::properties)
        .function("setProperties", &libOpenCOR::Solver::setProperties)
        .function("property", &libOpenCOR::Solver::property)
        .function("setProperty", &libOpenCOR::Solver::setProperty);

    EM_ASM({
        Module['Solver']['Type'] = Module['Solver.Type'];

        delete Module['Solver.Type'];
    });

    // SolverOde API.

    emscripten::class_<libOpenCOR::SolverOde, emscripten::base<libOpenCOR::Solver>>("SolverOde")
        .smart_ptr_constructor("SolverOde", &libOpenCOR::SolverOde::create)
        .function("initialise", &libOpenCOR::SolverOde::initialise, emscripten::allow_raw_pointers())
        .function("solve", &libOpenCOR::SolverOde::solve);
}
