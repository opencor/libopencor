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

#include "stringconverter.h"

#include <libopencor>

#include <emscripten/threading.h>

namespace {

constexpr std::string_view INVALID_ID_ERROR {"The id is not a valid SED-ML identifier."};
constexpr std::string_view ALREADY_USED_ID_ERROR {"The id is already used."};
constexpr std::string_view COMPONENT_NAME_ERROR {"The component name is not a valid CellML identifier."};
constexpr std::string_view VARIABLE_NAME_ERROR {"The variable name is not a valid CellML identifier."};
constexpr std::string_view NEW_VALUE_ERROR {"The new value is not a valid double value."};
constexpr std::string_view CHANGE_ATTRIBUTE_ERROR {"The component and variable names must be valid CellML identifiers and the new value must be a valid double value."};
constexpr std::string_view START_RUN_ERROR {"The instance cannot be run in a background thread since threads are not supported (in a Web browser, the page must be cross-origin isolated)."};

void setId(libOpenCOR::SedBase &pThis, const JsString &pId)
{
    // Note: a property setter cannot return whether the id was set, so we throw a RangeError if it wasn't, making sure
    //       that the std::string returned by fromJsString() is destroyed before we do so (see stringconverter.h).

    std::string_view error;

    {
        auto id {fromJsString(pId)};

        if (!libOpenCOR::SedBase::isValidId(id)) {
            error = INVALID_ID_ERROR;
        } else if (!pThis.setId(id)) {
            error = ALREADY_USED_ID_ERROR;
        }
    }

    if (!error.empty()) {
        throwRangeError(error);
    }
}

} // namespace

void sedApi()
{
    // SedBase API.

    emscripten::class_<libOpenCOR::SedBase, emscripten::base<libOpenCOR::Logger>>("SedBase")
        .smart_ptr<libOpenCOR::SedBasePtr>("SedBase")
        // clang-format off
        .class_function("isValidId", emscripten::optional_override([](const JsString &pId) {
            return libOpenCOR::SedBase::isValidId(fromJsString(pId));
        }))
        .function("isDuplicateId", emscripten::optional_override([](const libOpenCOR::SedBase &pThis, const JsString &pId) {
            return pThis.isDuplicateId(fromJsString(pId));
        })) // clang-format on
        .property("id", &JsStringResult<&libOpenCOR::SedBase::id>::call, &setId);

    // SedAbstractTask API.

    emscripten::class_<libOpenCOR::SedAbstractTask, emscripten::base<libOpenCOR::SedBase>>("SedAbstractTask")
        .smart_ptr<libOpenCOR::SedAbstractTaskPtr>("SedAbstractTask");

    // SedChange API.

    emscripten::class_<libOpenCOR::SedChange, emscripten::base<libOpenCOR::SedBase>>("SedChange")
        .smart_ptr<libOpenCOR::SedChangePtr>("SedChange")
        .property("target", &JsStringResult<&libOpenCOR::SedChange::target>::call);

    // SedChangeAttribute API.

    emscripten::class_<libOpenCOR::SedChangeAttribute, emscripten::base<libOpenCOR::SedChange>>("SedChangeAttribute")
        // clang-format off
        .smart_ptr_constructor("SedChangeAttribute", emscripten::optional_override([](const JsString &pComponentName, const JsString &pVariableName, const JsString &pNewValue) {
            // Note: a constructor cannot return nullptr, so we throw a RangeError if the object couldn't be created.

            auto res {libOpenCOR::SedChangeAttribute::create(fromJsString(pComponentName), fromJsString(pVariableName), fromJsString(pNewValue))};

            if (res == nullptr) {
                throwRangeError(CHANGE_ATTRIBUTE_ERROR);
            }

            return res;
        })) // clang-format on
        .property("componentName", &JsStringResult<&libOpenCOR::SedChangeAttribute::componentName>::call, &JsStringArgument<&libOpenCOR::SedChangeAttribute::setComponentName, COMPONENT_NAME_ERROR>::call)
        .property("variableName", &JsStringResult<&libOpenCOR::SedChangeAttribute::variableName>::call, &JsStringArgument<&libOpenCOR::SedChangeAttribute::setVariableName, VARIABLE_NAME_ERROR>::call)
        .property("newValue", &JsStringResult<&libOpenCOR::SedChangeAttribute::newValue>::call, &JsStringArgument<&libOpenCOR::SedChangeAttribute::setNewValue, NEW_VALUE_ERROR>::call);

    // SedDataDescription API.

    emscripten::class_<libOpenCOR::SedDataDescription, emscripten::base<libOpenCOR::SedBase>>("SedDataDescription")
        .smart_ptr<libOpenCOR::SedDataDescriptionPtr>("SedDataDescription");

    // SedDataGenerator API.

    emscripten::class_<libOpenCOR::SedDataGenerator, emscripten::base<libOpenCOR::SedBase>>("SedDataGenerator")
        .smart_ptr<libOpenCOR::SedDataGeneratorPtr>("SedDataGenerator");

    // SedDocument API.

    emscripten::class_<libOpenCOR::SedDocument, emscripten::base<libOpenCOR::Logger>>("SedDocument")
        .smart_ptr<libOpenCOR::SedDocumentPtr>("SedDocument")
        .constructor(&libOpenCOR::SedDocument::create)
        .constructor(&libOpenCOR::SedDocument::defaultCreate)
        // clang-format off
        .function("serialise", emscripten::optional_override([](const libOpenCOR::SedDocument &pThis) {
            return toJsString(pThis.serialise());
        }))
        .function("serialise", emscripten::optional_override([](const libOpenCOR::SedDocument &pThis, const JsString &pBasePath) {
            return toJsString(pThis.serialise(fromJsString(pBasePath)));
        })) // clang-format on
        .property("hasModels", &libOpenCOR::SedDocument::hasModels)
        .property("modelCount", &libOpenCOR::SedDocument::modelCount)
        .property("models", &libOpenCOR::SedDocument::models)
        .function("model", &libOpenCOR::SedDocument::model)
        .function("addModel", &libOpenCOR::SedDocument::addModel)
        .function("removeModel", &libOpenCOR::SedDocument::removeModel)
        .function("removeAllModels", &libOpenCOR::SedDocument::removeAllModels)
        .property("hasSimulations", &libOpenCOR::SedDocument::hasSimulations)
        .property("simulationCount", &libOpenCOR::SedDocument::simulationCount)
        .property("simulations", &libOpenCOR::SedDocument::simulations)
        .function("simulation", &libOpenCOR::SedDocument::simulation)
        .function("addSimulation", &libOpenCOR::SedDocument::addSimulation)
        .function("removeSimulation", &libOpenCOR::SedDocument::removeSimulation)
        .function("removeAllSimulations", &libOpenCOR::SedDocument::removeAllSimulations)
        .property("hasTasks", &libOpenCOR::SedDocument::hasTasks)
        .property("taskCount", &libOpenCOR::SedDocument::taskCount)
        .property("tasks", &libOpenCOR::SedDocument::tasks)
        .function("task", &libOpenCOR::SedDocument::task)
        .function("addTask", &libOpenCOR::SedDocument::addTask)
        .function("removeTask", &libOpenCOR::SedDocument::removeTask)
        .function("removeAllTasks", &libOpenCOR::SedDocument::removeAllTasks)
        .function("instantiate", &libOpenCOR::SedDocument::instantiate);

    // SedInstance API.

    emscripten::enum_<libOpenCOR::SedInstance::Status>("SedInstance.Status")
        .value("IDLE", libOpenCOR::SedInstance::Status::IDLE)
        .value("RUNNING", libOpenCOR::SedInstance::Status::RUNNING)
        .value("PAUSED", libOpenCOR::SedInstance::Status::PAUSED);

    emscripten::class_<libOpenCOR::SedInstance, emscripten::base<libOpenCOR::Logger>>("SedInstance")
        .smart_ptr<libOpenCOR::SedInstancePtr>("SedInstance")
        .property("status", &libOpenCOR::SedInstance::status)
        .function("run", &libOpenCOR::SedInstance::run)
        // clang-format off
        .function("startRun", emscripten::optional_override([](libOpenCOR::SedInstance &pThis) {
            // Note: threads require SharedArrayBuffer, which a Web browser only provides to a page that is cross-origin
            //       isolated (i.e. served with the "Cross-Origin-Opener-Policy: same-origin" and
            //       "Cross-Origin-Embedder-Policy: require-corp" HTTP headers). Without it, startRun() would fail to
            //       create its thread and throw a C++ exception, which would reach JavaScript as an opaque
            //       WebAssembly.Exception, so we throw an Error with a meaningful message instead.

            if (!emscripten_has_threading_support()) {
                throwError(START_RUN_ERROR);
            }

            return pThis.startRun();
        })) // clang-format on
        .function("waitForRun", &libOpenCOR::SedInstance::waitForRun)
        .function("pauseRun", &libOpenCOR::SedInstance::pauseRun)
        .function("resumeRun", &libOpenCOR::SedInstance::resumeRun)
        .function("stopRun", &libOpenCOR::SedInstance::stopRun)
        .property("progress", &libOpenCOR::SedInstance::progress)
        .property("hasTasks", &libOpenCOR::SedInstance::hasTasks)
        .property("taskCount", &libOpenCOR::SedInstance::taskCount)
        .property("tasks", &libOpenCOR::SedInstance::tasks)
        .function("task", &libOpenCOR::SedInstance::task);

    EM_ASM({
        if (Module["SedInstance"]) {
            Module["SedInstance"]["Status"] = Module["SedInstance.Status"];

            delete Module["SedInstance.Status"];
        }
    });

    // SedInstanceTask API.

    emscripten::class_<libOpenCOR::SedInstanceTask, emscripten::base<libOpenCOR::Logger>>("SedInstanceTask")
        .smart_ptr<libOpenCOR::SedInstanceTaskPtr>("SedInstanceTask")
        .property("progress", &libOpenCOR::SedInstanceTask::progress)
        .property("voi", &libOpenCOR::SedInstanceTask::voi)
        .property("voiName", &JsStringResult<&libOpenCOR::SedInstanceTask::voiName>::call)
        .property("voiUnit", &JsStringResult<&libOpenCOR::SedInstanceTask::voiUnit>::call)
        .property("stateCount", &libOpenCOR::SedInstanceTask::stateCount)
        .function("state", &libOpenCOR::SedInstanceTask::state)
        .function("stateName", &JsStringResult<&libOpenCOR::SedInstanceTask::stateName>::call)
        .function("stateUnit", &JsStringResult<&libOpenCOR::SedInstanceTask::stateUnit>::call)
        .property("rateCount", &libOpenCOR::SedInstanceTask::rateCount)
        .function("rate", &libOpenCOR::SedInstanceTask::rate)
        .function("rateName", &JsStringResult<&libOpenCOR::SedInstanceTask::rateName>::call)
        .function("rateUnit", &JsStringResult<&libOpenCOR::SedInstanceTask::rateUnit>::call)
        .property("constantCount", &libOpenCOR::SedInstanceTask::constantCount)
        .function("constant", &libOpenCOR::SedInstanceTask::constant)
        .function("constantName", &JsStringResult<&libOpenCOR::SedInstanceTask::constantName>::call)
        .function("constantUnit", &JsStringResult<&libOpenCOR::SedInstanceTask::constantUnit>::call)
        .property("computedConstantCount", &libOpenCOR::SedInstanceTask::computedConstantCount)
        .function("computedConstant", &libOpenCOR::SedInstanceTask::computedConstant)
        .function("computedConstantName", &JsStringResult<&libOpenCOR::SedInstanceTask::computedConstantName>::call)
        .function("computedConstantUnit", &JsStringResult<&libOpenCOR::SedInstanceTask::computedConstantUnit>::call)
        .property("algebraicVariableCount", &libOpenCOR::SedInstanceTask::algebraicVariableCount)
        .function("algebraicVariable", &libOpenCOR::SedInstanceTask::algebraicVariable)
        .function("algebraicVariableName", &JsStringResult<&libOpenCOR::SedInstanceTask::algebraicVariableName>::call)
        .function("algebraicVariableUnit", &JsStringResult<&libOpenCOR::SedInstanceTask::algebraicVariableUnit>::call);

    // SedModel API.

    emscripten::class_<libOpenCOR::SedModel, emscripten::base<libOpenCOR::SedBase>>("SedModel")
        .smart_ptr_constructor("SedModel", &libOpenCOR::SedModel::create)
        .property("file", &libOpenCOR::SedModel::file)
        .property("hasChanges", &libOpenCOR::SedModel::hasChanges)
        .property("changeCount", &libOpenCOR::SedModel::changeCount)
        .property("changes", &libOpenCOR::SedModel::changes)
        .function("change", &libOpenCOR::SedModel::change)
        .function("addChange", &libOpenCOR::SedModel::addChange)
        .function("removeChange", &libOpenCOR::SedModel::removeChange)
        .function("removeAllChanges", &libOpenCOR::SedModel::removeAllChanges);

    // SedOutput API.

    emscripten::class_<libOpenCOR::SedOutput, emscripten::base<libOpenCOR::SedBase>>("SedOutput")
        .smart_ptr<libOpenCOR::SedOutputPtr>("SedOutput");

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

    emscripten::class_<libOpenCOR::SedStyle, emscripten::base<libOpenCOR::SedBase>>("SedStyle")
        .smart_ptr<libOpenCOR::SedStylePtr>("SedStyle");

    // SedTask API.

    emscripten::class_<libOpenCOR::SedTask, emscripten::base<libOpenCOR::SedAbstractTask>>("SedTask")
        .smart_ptr_constructor("SedTask", &libOpenCOR::SedTask::create)
        .property("model", &libOpenCOR::SedTask::model, &libOpenCOR::SedTask::setModel)
        .property("simulation", &libOpenCOR::SedTask::simulation, &libOpenCOR::SedTask::setSimulation);
}
