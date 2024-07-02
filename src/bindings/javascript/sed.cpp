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

    emscripten::class_<libOpenCOR::SedAbstractTask, emscripten::base<libOpenCOR::SedBase>>("SedAbstractTask")
        .smart_ptr<libOpenCOR::SedAbstractTaskPtr>("SedAbstractTask");

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
        .function("hasModels", &libOpenCOR::SedDocument::hasModels)
        .function("modelCount", &libOpenCOR::SedDocument::modelCount)
        .function("models", &libOpenCOR::SedDocument::models)
        .function("addModel", &libOpenCOR::SedDocument::addModel)
        .function("removeModel", &libOpenCOR::SedDocument::removeModel)
        .function("hasSimulations", &libOpenCOR::SedDocument::hasSimulations)
        .function("simulationCount", &libOpenCOR::SedDocument::simulationCount)
        .function("simulations", &libOpenCOR::SedDocument::simulations)
        .function("addSimulation", &libOpenCOR::SedDocument::addSimulation)
        .function("removeSimulation", &libOpenCOR::SedDocument::removeSimulation)
        .function("hasTasks", &libOpenCOR::SedDocument::hasTasks)
        .function("taskCount", &libOpenCOR::SedDocument::taskCount)
        .function("tasks", &libOpenCOR::SedDocument::tasks)
        .function("addTask", &libOpenCOR::SedDocument::addTask)
        .function("removeTask", &libOpenCOR::SedDocument::removeTask)
        .function("instantiate", &libOpenCOR::SedDocument::instantiate);

    // SedInstance API.

    emscripten::class_<libOpenCOR::SedInstance, emscripten::base<libOpenCOR::Logger>>("SedInstance")
        .smart_ptr<libOpenCOR::SedInstancePtr>("SedInstance")
        .function("run", &libOpenCOR::SedInstance::run)
        .function("removeAllInitialConditions", &libOpenCOR::SedInstance::removeAllInitialConditions) //---ISAN---
        .function("addInitialCondition", &libOpenCOR::SedInstance::addInitialCondition) //---ISAN---
        .function("tasks", &libOpenCOR::SedInstance::tasks);

    // SedInstanceTask API.

    emscripten::class_<libOpenCOR::SedInstanceTask, emscripten::base<libOpenCOR::Logger>>("SedInstanceTask")
        .smart_ptr<libOpenCOR::SedInstanceTaskPtr>("SedInstanceTask")
        .function("voi", &libOpenCOR::SedInstanceTask::voi)
        .function("voiAsArray", &libOpenCOR::SedInstanceTask::voiAsArray)
        .function("voiName", &libOpenCOR::SedInstanceTask::voiName)
        .function("voiUnit", &libOpenCOR::SedInstanceTask::voiUnit)
        .function("stateCount", &libOpenCOR::SedInstanceTask::stateCount)
        .function("state", &libOpenCOR::SedInstanceTask::state)
        .function("stateAsArray", &libOpenCOR::SedInstanceTask::stateAsArray)
        .function("stateName", &libOpenCOR::SedInstanceTask::stateName)
        .function("stateUnit", &libOpenCOR::SedInstanceTask::stateUnit)
        .function("rateCount", &libOpenCOR::SedInstanceTask::rateCount)
        .function("rate", &libOpenCOR::SedInstanceTask::rate)
        .function("rateAsArray", &libOpenCOR::SedInstanceTask::rateAsArray)
        .function("rateName", &libOpenCOR::SedInstanceTask::rateName)
        .function("rateUnit", &libOpenCOR::SedInstanceTask::rateUnit)
        .function("variableCount", &libOpenCOR::SedInstanceTask::variableCount)
        .function("variable", &libOpenCOR::SedInstanceTask::variable)
        .function("variableAsArray", &libOpenCOR::SedInstanceTask::variableAsArray)
        .function("variableName", &libOpenCOR::SedInstanceTask::variableName)
        .function("variableUnit", &libOpenCOR::SedInstanceTask::variableUnit);

    // SedModel API.

    emscripten::class_<libOpenCOR::SedModel, emscripten::base<libOpenCOR::SedBase>>("SedModel")
        .smart_ptr_constructor("SedModel", &libOpenCOR::SedModel::create)
        .function("file", &libOpenCOR::SedModel::file);

    // SedOutput API.

    emscripten::class_<libOpenCOR::SedOutput, emscripten::base<libOpenCOR::SedBase>>("SedOutput");

    // SedRepeatedTask API.

    /*---GRY---
        emscripten::class_<libOpenCOR::SedRepeatedTask, emscripten::base<libOpenCOR::SedAbstractTask>>("SedRepeatedTask")
            .smart_ptr_constructor("SedRepeatedTask", &libOpenCOR::SedRepeatedTask::create);
    */

    // SedSimulation API.

    emscripten::class_<libOpenCOR::SedSimulation, emscripten::base<libOpenCOR::SedBase>>("SedSimulation")
        .smart_ptr<libOpenCOR::SedSimulationPtr>("SedSimulation")
        .function("odeSolver", &libOpenCOR::SedSimulation::odeSolver)
        .function("setOdeSolver", &libOpenCOR::SedSimulation::setOdeSolver)
        .function("nlaSolver", &libOpenCOR::SedSimulation::nlaSolver)
        .function("setNlaSolver", &libOpenCOR::SedSimulation::setNlaSolver);

    // SedAnalysis API.

    emscripten::class_<libOpenCOR::SedAnalysis, emscripten::base<libOpenCOR::SedSimulation>>("SedAnalysis")
        .smart_ptr_constructor("SedAnalysis", &libOpenCOR::SedAnalysis::create);

    // SedOneStep API.

    emscripten::class_<libOpenCOR::SedOneStep, emscripten::base<libOpenCOR::SedSimulation>>("SedOneStep")
        .smart_ptr_constructor("SedOneStep", &libOpenCOR::SedOneStep::create)
        .function("step", &libOpenCOR::SedOneStep::step)
        .function("setStep", &libOpenCOR::SedOneStep::setStep);

    // SedSteadyState API.

    emscripten::class_<libOpenCOR::SedSteadyState, emscripten::base<libOpenCOR::SedSimulation>>("SedSteadyState")
        .smart_ptr_constructor("SedSteadyState", &libOpenCOR::SedSteadyState::create);

    // SedUniformTimeCourse API.

    emscripten::class_<libOpenCOR::SedUniformTimeCourse, emscripten::base<libOpenCOR::SedSimulation>>("SedUniformTimeCourse")
        .smart_ptr_constructor("SedUniformTimeCourse", &libOpenCOR::SedUniformTimeCourse::create)
        .function("initialTime", &libOpenCOR::SedUniformTimeCourse::initialTime)
        .function("setInitialTime", &libOpenCOR::SedUniformTimeCourse::setInitialTime)
        .function("outputStartTime", &libOpenCOR::SedUniformTimeCourse::outputStartTime)
        .function("setOutputStartTime", &libOpenCOR::SedUniformTimeCourse::setOutputStartTime)
        .function("outputEndTime", &libOpenCOR::SedUniformTimeCourse::outputEndTime)
        .function("setOutputEndTime", &libOpenCOR::SedUniformTimeCourse::setOutputEndTime)
        .function("numberOfSteps", &libOpenCOR::SedUniformTimeCourse::numberOfSteps)
        .function("setNumberOfSteps", &libOpenCOR::SedUniformTimeCourse::setNumberOfSteps);

    // SedStyle API.

    emscripten::class_<libOpenCOR::SedStyle, emscripten::base<libOpenCOR::SedBase>>("SedStyle");

    // SedTask API.

    emscripten::class_<libOpenCOR::SedTask, emscripten::base<libOpenCOR::SedAbstractTask>>("SedTask")
        .smart_ptr_constructor("SedTask", &libOpenCOR::SedTask::create)
        .function("model", &libOpenCOR::SedTask::model)
        .function("setModel", &libOpenCOR::SedTask::setModel)
        .function("simulation", &libOpenCOR::SedTask::simulation)
        .function("setSimulation", &libOpenCOR::SedTask::setSimulation);
}
