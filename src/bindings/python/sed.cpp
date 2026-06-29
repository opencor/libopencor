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

#include <nanobind/ndarray.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;

void sedApi(nb::module_ &m)
{
    // SedBase API.

    nb::class_<libOpenCOR::SedBase, libOpenCOR::Logger> sedBase(m, "SedBase");

    sedBase.def_prop_rw("id", &libOpenCOR::SedBase::id, &libOpenCOR::SedBase::setId, "The id.");

    // SedAbstractTask API.

    nb::class_<libOpenCOR::SedAbstractTask, libOpenCOR::SedBase> sedAbstractTask(m, "SedAbstractTask");

    // SedChange API.

    nb::class_<libOpenCOR::SedChange, libOpenCOR::SedBase> sedChange(m, "SedChange");

    sedChange.def_prop_ro("target", &libOpenCOR::SedChange::target, "Get the target.");

    // SedChangeAttribute API.

    nb::class_<libOpenCOR::SedChangeAttribute, libOpenCOR::SedChange> sedChangeAttribute(m, "SedChangeAttribute");

    sedChangeAttribute.def(nb::new_(&libOpenCOR::SedChangeAttribute::create), "Create a SedChangeAttribute object.", nb::arg("component"), nb::arg("variable"), nb::arg("new_value"))
        .def_prop_rw("component_name", &libOpenCOR::SedChangeAttribute::componentName, &libOpenCOR::SedChangeAttribute::setComponentName, "The name of the component.")
        .def_prop_rw("variable_name", &libOpenCOR::SedChangeAttribute::variableName, &libOpenCOR::SedChangeAttribute::setVariableName, "The name of the variable.")
        .def_prop_rw("new_value", &libOpenCOR::SedChangeAttribute::newValue, &libOpenCOR::SedChangeAttribute::setNewValue, "The new value.");

    // SedDataDescription API.

    nb::class_<libOpenCOR::SedDataDescription, libOpenCOR::SedBase> sedDataDescription(m, "SedDataDescription");

    // SedDataGenerator API.

    nb::class_<libOpenCOR::SedDataGenerator, libOpenCOR::SedBase> sedDataGenerator(m, "SedDataGenerator");

    // SedDocument API.

    nb::class_<libOpenCOR::SedDocument, libOpenCOR::Logger> sedDocument(m, "SedDocument");

    sedDocument.def(nb::new_([]() {
                        return libOpenCOR::SedDocument::create();
                    }),
                    "Create a SedDocument object.")
        .def(nb::new_(&libOpenCOR::SedDocument::create), "Create a SedDocument object.", nb::arg("file").none())
        .def("serialise", nb::overload_cast<>(&libOpenCOR::SedDocument::serialise, nb::const_), "Get the serialised version of this simulation experiment description.")
        .def("serialise", nb::overload_cast<const std::string &>(&libOpenCOR::SedDocument::serialise, nb::const_), "Get the serialised version of this simulation experiment description with the given base path.", nb::arg("base_path"))
        .def_prop_ro("has_models", &libOpenCOR::SedDocument::hasModels, "Return whether there are some models.")
        .def_prop_ro("model_count", &libOpenCOR::SedDocument::modelCount, "Return the number of models.")
        .def_prop_ro("models", &libOpenCOR::SedDocument::models, "Return the models.")
        .def("model", &libOpenCOR::SedDocument::model, "Return the model at the given index.", nb::arg("index"))
        .def("add_model", &libOpenCOR::SedDocument::addModel, "Add the given model.", nb::arg("model").none())
        .def("remove_model", &libOpenCOR::SedDocument::removeModel, "Remove the given model.", nb::arg("model").none())
        .def("remove_all_models", &libOpenCOR::SedDocument::removeAllModels, "Remove all the models.")
        .def_prop_ro("has_simulations", &libOpenCOR::SedDocument::hasSimulations, "Return whether there are some simulations.")
        .def_prop_ro("simulation_count", &libOpenCOR::SedDocument::simulationCount, "Return the number of simulations.")
        .def_prop_ro("simulations", &libOpenCOR::SedDocument::simulations, "Return the simulations.")
        .def("simulation", &libOpenCOR::SedDocument::simulation, "Return the simulation at the given index.", nb::arg("index"))
        .def("add_simulation", &libOpenCOR::SedDocument::addSimulation, "Add the given simulation.", nb::arg("simulation").none())
        .def("remove_simulation", &libOpenCOR::SedDocument::removeSimulation, "Remove the given simulation.", nb::arg("simulation").none())
        .def("remove_all_simulations", &libOpenCOR::SedDocument::removeAllSimulations, "Remove all the simulations.")
        .def_prop_ro("has_tasks", &libOpenCOR::SedDocument::hasTasks, "Return whether there are some tasks.")
        .def_prop_ro("task_count", &libOpenCOR::SedDocument::taskCount, "Return the number of tasks.")
        .def_prop_ro("tasks", &libOpenCOR::SedDocument::tasks, "Return the tasks.")
        .def("task", &libOpenCOR::SedDocument::task, "Return the task at the given index.", nb::arg("index"))
        .def("add_task", &libOpenCOR::SedDocument::addTask, "Add the given task.", nb::arg("task").none())
        .def("remove_task", &libOpenCOR::SedDocument::removeTask, "Remove the given task.", nb::arg("task").none())
        .def("remove_all_tasks", &libOpenCOR::SedDocument::removeAllTasks, "Remove all the tasks.")
        .def("instantiate", &libOpenCOR::SedDocument::instantiate, "Instantiate this simulation experiment description.");

    // SedInstance API.

    nb::class_<libOpenCOR::SedInstance, libOpenCOR::Logger> sedInstance(m, "SedInstance");

    sedInstance.def("run", &libOpenCOR::SedInstance::run, "Run all the tasks associated with this instance.", nb::call_guard<nb::gil_scoped_release>())
        .def("start_run", &libOpenCOR::SedInstance::startRun, "Start running, in a background thread, all the tasks associated with this instance.")
        .def_prop_ro("is_running", &libOpenCOR::SedInstance::isRunning, "Return whether this instance is currently running.")
        .def("wait_for_run", &libOpenCOR::SedInstance::waitForRun, "Wait for any currently-running instance to complete.", nb::call_guard<nb::gil_scoped_release>())
        .def("pause_run", &libOpenCOR::SedInstance::pauseRun, "Pause a currently-running instance.")
        .def("resume_run", &libOpenCOR::SedInstance::resumeRun, "Resume a currently-paused instance.")
        .def("stop_run", &libOpenCOR::SedInstance::stopRun, "Stop any currently-running instance.")
        .def_prop_ro("progress", &libOpenCOR::SedInstance::progress, "Return the progress of the current instance run.")
        .def_prop_ro("has_tasks", &libOpenCOR::SedInstance::hasTasks, "Return whether there are some tasks.")
        .def_prop_ro("task_count", &libOpenCOR::SedInstance::taskCount, "Return the number of tasks.")
        .def_prop_ro("tasks", &libOpenCOR::SedInstance::tasks, "Return all the tasks.")
        .def("task", &libOpenCOR::SedInstance::task, "Return the task at the given index.", nb::arg("index"))
        .def("__len__", &libOpenCOR::SedInstance::taskCount)
        .def("__iter__", [](const libOpenCOR::SedInstance &self) {
            return nb::cast(self.tasks()).attr("__iter__")();
        });

    // SedInstanceTask API.

    nb::class_<libOpenCOR::SedInstanceTask, libOpenCOR::Logger> sedInstanceTask(m, "SedInstanceTask");

    sedInstanceTask.def_prop_ro("progress", &libOpenCOR::SedInstanceTask::progress, "Return the progress of this task.")
        .def_prop_ro("voi", [](const libOpenCOR::SedInstanceTask &self) {
            const auto &data = self.voi();
            size_t shape[1] = {data.size()};

            return nb::ndarray<nb::numpy, const double>(data.data(), 1, shape, nb::handle());
        },
                     "Return the values of the variable of integration as a zero-copy NumPy array.")
        .def_prop_ro("voi_name", &libOpenCOR::SedInstanceTask::voiName, "Return the name of the variable of integration.")
        .def_prop_ro("voi_unit", &libOpenCOR::SedInstanceTask::voiUnit, "Return the unit of the variable of integration.")
        .def_prop_ro("state_count", &libOpenCOR::SedInstanceTask::stateCount, "Return the number of states.")
        .def("state", [](const libOpenCOR::SedInstanceTask &self, size_t pIndex) {
            const auto &data = self.state(pIndex);
            size_t shape[1] = {data.size()};

            return nb::ndarray<nb::numpy, const double>(data.data(), 1, shape, nb::handle());
        },
             "Return the values of the state at the given index as a zero-copy NumPy array.", nb::arg("index"))
        .def("state_name", &libOpenCOR::SedInstanceTask::stateName, "Return the name of the state at the given index.", nb::arg("index"))
        .def("state_unit", &libOpenCOR::SedInstanceTask::stateUnit, "Return the unit of the state at the given index.", nb::arg("index"))
        .def_prop_ro("rate_count", &libOpenCOR::SedInstanceTask::rateCount, "Return the number of rates.")
        .def("rate", [](const libOpenCOR::SedInstanceTask &self, size_t pIndex) {
            const auto &data = self.rate(pIndex);
            size_t shape[1] = {data.size()};

            return nb::ndarray<nb::numpy, const double>(data.data(), 1, shape, nb::handle());
        },
             "Return the values of the rate at the given index as a zero-copy NumPy array.", nb::arg("index"))
        .def("rate_name", &libOpenCOR::SedInstanceTask::rateName, "Return the name of the rate at the given index.", nb::arg("index"))
        .def("rate_unit", &libOpenCOR::SedInstanceTask::rateUnit, "Return the unit of the rate at the given index.", nb::arg("index"))
        .def_prop_ro("constant_count", &libOpenCOR::SedInstanceTask::constantCount, "Return the number of constants.")
        .def("constant", [](const libOpenCOR::SedInstanceTask &self, size_t pIndex) {
            const auto &data = self.constant(pIndex);
            size_t shape[1] = {data.size()};

            return nb::ndarray<nb::numpy, const double>(data.data(), 1, shape, nb::handle());
        },
             "Return the values of the constant at the given index as a zero-copy NumPy array.", nb::arg("index"))
        .def("constant_name", &libOpenCOR::SedInstanceTask::constantName, "Return the name of the constant at the given index.", nb::arg("index"))
        .def("constant_unit", &libOpenCOR::SedInstanceTask::constantUnit, "Return the unit of the constant at the given index.", nb::arg("index"))
        .def_prop_ro("computed_constant_count", &libOpenCOR::SedInstanceTask::computedConstantCount, "Return the number of computed constants.")
        .def("computed_constant", [](const libOpenCOR::SedInstanceTask &self, size_t pIndex) {
            const auto &data = self.computedConstant(pIndex);
            size_t shape[1] = {data.size()};

            return nb::ndarray<nb::numpy, const double>(data.data(), 1, shape, nb::handle());
        },
             "Return the values of the computed constant at the given index as a zero-copy NumPy array.", nb::arg("index"))
        .def("computed_constant_name", &libOpenCOR::SedInstanceTask::computedConstantName, "Return the name of the computed constant at the given index.", nb::arg("index"))
        .def("computed_constant_unit", &libOpenCOR::SedInstanceTask::computedConstantUnit, "Return the unit of the computed constant at the given index.", nb::arg("index"))
        .def_prop_ro("algebraic_variable_count", &libOpenCOR::SedInstanceTask::algebraicVariableCount, "Return the number of algebraic variables.")
        .def("algebraic_variable", [](const libOpenCOR::SedInstanceTask &self, size_t pIndex) {
            const auto &data = self.algebraicVariable(pIndex);
            size_t shape[1] = {data.size()};

            return nb::ndarray<nb::numpy, const double>(data.data(), 1, shape, nb::handle());
        },
             "Return the values of the algebraic variable at the given index as a zero-copy NumPy array.", nb::arg("index"))
        .def("algebraic_variable_name", &libOpenCOR::SedInstanceTask::algebraicVariableName, "Return the name of the algebraic variable at the given index.", nb::arg("index"))
        .def("algebraic_variable_unit", &libOpenCOR::SedInstanceTask::algebraicVariableUnit, "Return the unit of the algebraic variable at the given index.", nb::arg("index"));

    // SedModel API.

    nb::class_<libOpenCOR::SedModel, libOpenCOR::SedBase> sedModel(m, "SedModel");

    sedModel.def(nb::new_(&libOpenCOR::SedModel::create), "Create a SedModel object.", nb::arg("document"), nb::arg("file"))
        .def_prop_ro("file", &libOpenCOR::SedModel::file, "Return the file.")
        .def_prop_ro("has_changes", &libOpenCOR::SedModel::hasChanges, "Return whether there are some changes.")
        .def_prop_ro("change_count", &libOpenCOR::SedModel::changeCount, "Return the number of changes.")
        .def_prop_ro("changes", &libOpenCOR::SedModel::changes, "Return the changes.")
        .def("change", &libOpenCOR::SedModel::change, "Return the change at the given index.", nb::arg("index"))
        .def("add_change", &libOpenCOR::SedModel::addChange, "Add the given change.", nb::arg("change").none())
        .def("remove_change", &libOpenCOR::SedModel::removeChange, "Remove the given change.", nb::arg("change").none())
        .def("remove_all_changes", &libOpenCOR::SedModel::removeAllChanges, "Remove all the changes.");

    // SedOutput API.

    nb::class_<libOpenCOR::SedOutput, libOpenCOR::SedBase>
        sedOutput(m, "SedOutput");

    // SedRepeatedTask API.

    /*---GRY---
        nb::class_<libOpenCOR::SedRepeatedTask, libOpenCOR::SedAbstractTask> sedRepeatedTask(m, "SedRepeatedTask");

        sedRepeatedTask.def(nb::new_(&libOpenCOR::SedRepeatedTask::create), "Create a SedRepeatedTask object.", nb::arg("document"));
    */

    // SedSimulation API.

    nb::class_<libOpenCOR::SedSimulation, libOpenCOR::SedBase> sedSimulation(m, "SedSimulation");

    sedSimulation.def_prop_rw("ode_solver", &libOpenCOR::SedSimulation::odeSolver, &libOpenCOR::SedSimulation::setOdeSolver, "The ODE solver.", nb::arg("ode_solver").none())
        .def_prop_rw("nla_solver", &libOpenCOR::SedSimulation::nlaSolver, &libOpenCOR::SedSimulation::setNlaSolver, "The NLA solver.", nb::arg("nla_solver").none());

    // SedAnalysis API.

    nb::class_<libOpenCOR::SedAnalysis, libOpenCOR::SedSimulation> sedAnalysis(m, "SedAnalysis");

    sedAnalysis.def(nb::new_(&libOpenCOR::SedAnalysis::create), "Create a SedAnalysis object.", nb::arg("document"));

    // SedOneStep API.

    nb::class_<libOpenCOR::SedOneStep, libOpenCOR::SedSimulation> sedOneStep(m, "SedOneStep");

    sedOneStep.def(nb::new_(&libOpenCOR::SedOneStep::create), "Create a SedOneStep object.", nb::arg("document"))
        .def_prop_rw("step", &libOpenCOR::SedOneStep::step, &libOpenCOR::SedOneStep::setStep, "The step.");

    // SedSteadyState API.

    nb::class_<libOpenCOR::SedSteadyState, libOpenCOR::SedSimulation> sedSteadyState(m, "SedSteadyState");

    sedSteadyState.def(nb::new_(&libOpenCOR::SedSteadyState::create), "Create a SedSteadyState object.", nb::arg("document"));

    // SedUniformTimeCourse API.

    nb::class_<libOpenCOR::SedUniformTimeCourse, libOpenCOR::SedSimulation> sedUniformTimeCourse(m, "SedUniformTimeCourse");

    sedUniformTimeCourse.def(nb::new_(&libOpenCOR::SedUniformTimeCourse::create), "Create a SedUniformTimeCourse object.", nb::arg("document"))
        .def_prop_rw("initial_time", &libOpenCOR::SedUniformTimeCourse::initialTime, &libOpenCOR::SedUniformTimeCourse::setInitialTime, "The initial time.")
        .def_prop_rw("output_start_time", &libOpenCOR::SedUniformTimeCourse::outputStartTime, &libOpenCOR::SedUniformTimeCourse::setOutputStartTime, "The output start time.")
        .def_prop_rw("output_end_time", &libOpenCOR::SedUniformTimeCourse::outputEndTime, &libOpenCOR::SedUniformTimeCourse::setOutputEndTime, "The output end time.")
        .def_prop_rw("number_of_steps", &libOpenCOR::SedUniformTimeCourse::numberOfSteps, &libOpenCOR::SedUniformTimeCourse::setNumberOfSteps, "The number of steps.");

    // SedStyle API.

    nb::class_<libOpenCOR::SedStyle, libOpenCOR::SedBase> sedStyle(m, "SedStyle");

    // SedTask API.

    nb::class_<libOpenCOR::SedTask, libOpenCOR::SedAbstractTask> sedTask(m, "SedTask");

    sedTask.def(nb::new_(&libOpenCOR::SedTask::create), "Create a SedTask object.", nb::arg("document"), nb::arg("model"), nb::arg("simulation"))
        .def_prop_rw("model", &libOpenCOR::SedTask::model, &libOpenCOR::SedTask::setModel, "The model.", nb::arg("model").none())
        .def_prop_rw("simulation", &libOpenCOR::SedTask::simulation, &libOpenCOR::SedTask::setSimulation, "The simulation.", nb::arg("simulation").none());
}
