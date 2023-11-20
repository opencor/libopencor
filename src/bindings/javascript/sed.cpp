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

void sedApi()
{
    // SedBase API.

    emscripten::class_<libOpenCOR::SedBase, emscripten::base<libOpenCOR::Logger>>("SedBase");

    // SedAbstractTask API.

    emscripten::class_<libOpenCOR::SedAbstractTask, emscripten::base<libOpenCOR::SedBase>>("SedAbstractTask");

    // SedDataDescription API.

    emscripten::class_<libOpenCOR::SedDataDescription, emscripten::base<libOpenCOR::SedBase>>("SedDataDescription");

    // SedDataGenerator API.

    emscripten::class_<libOpenCOR::SedDataGenerator, emscripten::base<libOpenCOR::SedBase>>("SedDataGenerator");

    // SedDocument API.

    emscripten::class_<libOpenCOR::SedDocument, emscripten::base<libOpenCOR::Logger>>("SedDocument")
        .smart_ptr<libOpenCOR::SedDocumentPtr>("SedDocument")
        .constructor(&libOpenCOR::SedDocument::create)
        .constructor(&libOpenCOR::SedDocument::defaultCreate)
        .function("serialise", emscripten::select_overload<std::string() const>(&libOpenCOR::SedDocument::serialise))
        .function("serialise", emscripten::select_overload<std::string(const std::string &) const>(&libOpenCOR::SedDocument::serialise))
        .function("models", &libOpenCOR::SedDocument::models)
        .function("addModel", &libOpenCOR::SedDocument::addModel)
        .function("removeModel", &libOpenCOR::SedDocument::removeModel)
        .function("simulations", &libOpenCOR::SedDocument::simulations)
        .function("addSimulation", &libOpenCOR::SedDocument::addSimulation)
        .function("removeSimulation", &libOpenCOR::SedDocument::removeSimulation);

    // SedModel API.

    emscripten::class_<libOpenCOR::SedModel, emscripten::base<libOpenCOR::SedBase>>("SedModel")
        .smart_ptr_constructor("SedModel", &libOpenCOR::SedModel::create);

    // SedOutput API.

    emscripten::class_<libOpenCOR::SedOutput, emscripten::base<libOpenCOR::SedBase>>("SedOutput");

    // SedSimulation API.

    emscripten::class_<libOpenCOR::SedSimulation, emscripten::base<libOpenCOR::SedBase>>("SedSimulation")
        .smart_ptr<libOpenCOR::SedSimulationPtr>("SedSimulation");

    // SedSimulationOneStep API.

    emscripten::class_<libOpenCOR::SedSimulationOneStep, emscripten::base<libOpenCOR::SedSimulation>>("SedSimulationOneStep")
        .smart_ptr_constructor("SedSimulationOneStep", &libOpenCOR::SedSimulationOneStep::create);

    // SedSimulationSteadyState API.

    emscripten::class_<libOpenCOR::SedSimulationSteadyState, emscripten::base<libOpenCOR::SedSimulation>>("SedSimulationSteadyState")
        .smart_ptr_constructor("SedSimulationSteadyState", &libOpenCOR::SedSimulationSteadyState::create);

    // SedSimulationUniformTimeCourse API.

    emscripten::class_<libOpenCOR::SedSimulationUniformTimeCourse, emscripten::base<libOpenCOR::SedSimulation>>("SedSimulationUniformTimeCourse")
        .smart_ptr_constructor("SedSimulationUniformTimeCourse", &libOpenCOR::SedSimulationUniformTimeCourse::create);

    // SedStyle API.

    emscripten::class_<libOpenCOR::SedStyle, emscripten::base<libOpenCOR::SedBase>>("SedStyle");
}
