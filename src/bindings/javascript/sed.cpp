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

    emscripten::class_<libOpenCOR::SedBase, emscripten::base<libOpenCOR::Logger>>("SedBase")
        .smart_ptr<libOpenCOR::SedBasePtr>("SedBase")
        .property("id", &libOpenCOR::SedBase::id, &libOpenCOR::SedBase::setId);

    // SedAbstractTask API.

    emscripten::class_<libOpenCOR::SedAbstractTask, emscripten::base<libOpenCOR::SedBase>>("SedAbstractTask")
        .smart_ptr<libOpenCOR::SedAbstractTaskPtr>("SedAbstractTask");

    // SedChange API.

    emscripten::class_<libOpenCOR::SedChange, emscripten::base<libOpenCOR::SedBase>>("SedChange")
        .smart_ptr<libOpenCOR::SedChangePtr>("SedChange")
        .property("target", &libOpenCOR::SedChange::target, &libOpenCOR::SedChange::setTarget);

    // SedChangeAttribute API.

    emscripten::class_<libOpenCOR::SedChangeAttribute, emscripten::base<libOpenCOR::SedChange>>("SedChangeAttribute")
        .smart_ptr_constructor("SedChangeAttribute", &libOpenCOR::SedChangeAttribute::create)
        .property("newValue", &libOpenCOR::SedChangeAttribute::newValue, &libOpenCOR::SedChangeAttribute::setNewValue);

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
        .property("hasModels", &libOpenCOR::SedDocument::hasModels)
        .property("modelCount", &libOpenCOR::SedDocument::modelCount)
        .property("models", &libOpenCOR::SedDocument::models)
        .function("model", &libOpenCOR::SedDocument::model)
        .function("addModel", &libOpenCOR::SedDocument::addModel)
        .function("removeModel", &libOpenCOR::SedDocument::removeModel)
        .property("hasSimulations", &libOpenCOR::SedDocument::hasSimulations)
        .property("simulationCount", &libOpenCOR::SedDocument::simulationCount)
        .property("simulations", &libOpenCOR::SedDocument::simulations)
        .function("simulation", &libOpenCOR::SedDocument::simulation)
        .function("addSimulation", &libOpenCOR::SedDocument::addSimulation)
        .function("removeSimulation", &libOpenCOR::SedDocument::removeSimulation)
        .property("hasTasks", &libOpenCOR::SedDocument::hasTasks)
        .property("taskCount", &libOpenCOR::SedDocument::taskCount)
        .property("tasks", &libOpenCOR::SedDocument::tasks)
        .function("task", &libOpenCOR::SedDocument::task)
        .function("addTask", &libOpenCOR::SedDocument::addTask)
        .function("removeTask", &libOpenCOR::SedDocument::removeTask)
        .function("instantiate", &libOpenCOR::SedDocument::instantiate);

    // SedInstance API.

    emscripten::class_<libOpenCOR::SedInstance, emscripten::base<libOpenCOR::Logger>>("SedInstance")
        .smart_ptr<libOpenCOR::SedInstancePtr>("SedInstance")
        .function("run", &libOpenCOR::SedInstance::run)
        .property("hasTasks", &libOpenCOR::SedInstance::hasTasks)
        .property("taskCount", &libOpenCOR::SedInstance::taskCount)
        .property("tasks", &libOpenCOR::SedInstance::tasks)
        .function("task", &libOpenCOR::SedInstance::task);

    // SedInstanceTask API.

    emscripten::class_<libOpenCOR::SedInstanceTask, emscripten::base<libOpenCOR::Logger>>("SedInstanceTask")
        .smart_ptr<libOpenCOR::SedInstanceTaskPtr>("SedInstanceTask")
        .property("voi", &libOpenCOR::SedInstanceTask::voi)
        .property("voiAsArray", &libOpenCOR::SedInstanceTask::voiAsArray)
        .property("voiName", &libOpenCOR::SedInstanceTask::voiName)
        .property("voiUnit", &libOpenCOR::SedInstanceTask::voiUnit)
        .property("stateCount", &libOpenCOR::SedInstanceTask::stateCount)
        .function("state", &libOpenCOR::SedInstanceTask::state)
        .function("stateAsArray", &libOpenCOR::SedInstanceTask::stateAsArray)
        .function("stateName", &libOpenCOR::SedInstanceTask::stateName)
        .function("stateUnit", &libOpenCOR::SedInstanceTask::stateUnit)
        .property("rateCount", &libOpenCOR::SedInstanceTask::rateCount)
        .function("rate", &libOpenCOR::SedInstanceTask::rate)
        .function("rateAsArray", &libOpenCOR::SedInstanceTask::rateAsArray)
        .function("rateName", &libOpenCOR::SedInstanceTask::rateName)
        .function("rateUnit", &libOpenCOR::SedInstanceTask::rateUnit)
        .property("constantCount", &libOpenCOR::SedInstanceTask::constantCount)
        .function("constant", &libOpenCOR::SedInstanceTask::constant)
        .function("constantAsArray", &libOpenCOR::SedInstanceTask::constantAsArray)
        .function("constantName", &libOpenCOR::SedInstanceTask::constantName)
        .function("constantUnit", &libOpenCOR::SedInstanceTask::constantUnit)
        .property("computedConstantCount", &libOpenCOR::SedInstanceTask::computedConstantCount)
        .function("computedConstant", &libOpenCOR::SedInstanceTask::computedConstant)
        .function("computedConstantAsArray", &libOpenCOR::SedInstanceTask::computedConstantAsArray)
        .function("computedConstantName", &libOpenCOR::SedInstanceTask::computedConstantName)
        .function("computedConstantUnit", &libOpenCOR::SedInstanceTask::computedConstantUnit)
        .property("algebraicCount", &libOpenCOR::SedInstanceTask::algebraicCount)
        .function("algebraic", &libOpenCOR::SedInstanceTask::algebraic)
        .function("algebraicAsArray", &libOpenCOR::SedInstanceTask::algebraicAsArray)
        .function("algebraicName", &libOpenCOR::SedInstanceTask::algebraicName)
        .function("algebraicUnit", &libOpenCOR::SedInstanceTask::algebraicUnit);

    // SedModel API.

    emscripten::class_<libOpenCOR::SedModel, emscripten::base<libOpenCOR::SedBase>>("SedModel")
        .smart_ptr_constructor("SedModel", &libOpenCOR::SedModel::create)
        .property("file", &libOpenCOR::SedModel::file);

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
        .property("odeSolver", &libOpenCOR::SedSimulation::odeSolver, &libOpenCOR::SedSimulation::setOdeSolver)
        .property("nlaSolver", &libOpenCOR::SedSimulation::nlaSolver, &libOpenCOR::SedSimulation::setNlaSolver);

    // SedAnalysis API.

    emscripten::class_<libOpenCOR::SedAnalysis, emscripten::base<libOpenCOR::SedSimulation>>("SedAnalysis")
        .smart_ptr_constructor("SedAnalysis", &libOpenCOR::SedAnalysis::create);

    // SedOneStep API.

    emscripten::class_<libOpenCOR::SedOneStep, emscripten::base<libOpenCOR::SedSimulation>>("SedOneStep")
        .smart_ptr_constructor("SedOneStep", &libOpenCOR::SedOneStep::create)
        .property("step", &libOpenCOR::SedOneStep::step, &libOpenCOR::SedOneStep::setStep);

    // SedSteadyState API.

    emscripten::class_<libOpenCOR::SedSteadyState, emscripten::base<libOpenCOR::SedSimulation>>("SedSteadyState")
        .smart_ptr_constructor("SedSteadyState", &libOpenCOR::SedSteadyState::create);

    // SedUniformTimeCourse API.

    emscripten::class_<libOpenCOR::SedUniformTimeCourse, emscripten::base<libOpenCOR::SedSimulation>>("SedUniformTimeCourse")
        .smart_ptr_constructor("SedUniformTimeCourse", &libOpenCOR::SedUniformTimeCourse::create)
        .property("initialTime", &libOpenCOR::SedUniformTimeCourse::initialTime, &libOpenCOR::SedUniformTimeCourse::setInitialTime)
        .property("outputStartTime", &libOpenCOR::SedUniformTimeCourse::outputStartTime, &libOpenCOR::SedUniformTimeCourse::setOutputStartTime)
        .property("outputEndTime", &libOpenCOR::SedUniformTimeCourse::outputEndTime, &libOpenCOR::SedUniformTimeCourse::setOutputEndTime)
        .property("numberOfSteps", &libOpenCOR::SedUniformTimeCourse::numberOfSteps, &libOpenCOR::SedUniformTimeCourse::setNumberOfSteps);

    // SedStyle API.

    emscripten::class_<libOpenCOR::SedStyle, emscripten::base<libOpenCOR::SedBase>>("SedStyle");

    // SedTask API.

    emscripten::class_<libOpenCOR::SedTask, emscripten::base<libOpenCOR::SedAbstractTask>>("SedTask")
        .smart_ptr_constructor("SedTask", &libOpenCOR::SedTask::create)
        .property("model", &libOpenCOR::SedTask::model, &libOpenCOR::SedTask::setModel)
        .property("simulation", &libOpenCOR::SedTask::simulation, &libOpenCOR::SedTask::setSimulation);
}
