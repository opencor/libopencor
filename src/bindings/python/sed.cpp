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
#include <nanobind/stl/vector.h>

namespace nb = nanobind;
using namespace nb::literals;

void sedApi(nb::module_ &m)
{
    // SedBase API.

    nb::class_<libOpenCOR::SedBase, libOpenCOR::Logger> sedBase(m, "SedBase");

    // SedAbstractTask API.

    nb::class_<libOpenCOR::SedAbstractTask, libOpenCOR::SedBase> sedAbstractTask(m, "SedAbstractTask");

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
        .def(nb::new_([](const libOpenCOR::FilePtr &pFile) {
                 return libOpenCOR::SedDocument::create(pFile);
             }),
             "Create a SedDocument object.")
        .def("serialise", nb::overload_cast<>(&libOpenCOR::SedDocument::serialise, nb::const_), "Get the serialised version of this SedDocument object.")
        .def("serialise", nb::overload_cast<const std::string &>(&libOpenCOR::SedDocument::serialise, nb::const_), "base_path"_a, "Get the serialised version of this SedDocument object.")
        .def_prop_ro("has_models", &libOpenCOR::SedDocument::hasModels, "Return whether there are some models.")
        .def_prop_ro("models", &libOpenCOR::SedDocument::models, "Return the models.")
        .def("add_model", &libOpenCOR::SedDocument::addModel, "model"_a.none(), "Add a model.")
        .def("remove_model", &libOpenCOR::SedDocument::removeModel, "model"_a.none(), "Remove a model.")
        .def_prop_ro("has_simulations", &libOpenCOR::SedDocument::hasSimulations, "Return whether there are some simulations.")
        .def_prop_ro("simulations", &libOpenCOR::SedDocument::simulations, "Return the simulations.")
        .def("add_simulation", &libOpenCOR::SedDocument::addSimulation, "simulation"_a.none(), "Add a simulation.")
        .def("remove_simulation", &libOpenCOR::SedDocument::removeSimulation, "simulation"_a.none(), "Remove a simulation.")
        .def_prop_ro("has_tasks", &libOpenCOR::SedDocument::hasTasks, "Return whether there are some tasks.")
        .def_prop_ro("tasks", &libOpenCOR::SedDocument::tasks, "Return the tasks.")
        .def("add_task", &libOpenCOR::SedDocument::addTask, "task"_a.none(), "Add a task.")
        .def("remove_task", &libOpenCOR::SedDocument::removeTask, "task"_a.none(), "Remove a task.")
        .def("create_instance", &libOpenCOR::SedDocument::createInstance, "compiled"_a = true, "Create an instance of this SedDocument object.");

    // SedInstance API.

    nb::class_<libOpenCOR::SedInstance, libOpenCOR::Logger> sedInstance(m, "SedInstance");

    sedInstance.def("run", &libOpenCOR::SedInstance::run, "Run the tasks associated with this SedInstance object.")
        .def_prop_ro("tasks", &libOpenCOR::SedInstance::tasks, "Return the tasks.");

    // SedInstanceTask API.

    nb::class_<libOpenCOR::SedInstanceTask, libOpenCOR::Logger> sedInstanceTask(m, "SedInstanceTask");

    sedInstanceTask.def_prop_ro("voi", &libOpenCOR::SedInstanceTask::voi, "Return the values of the variable of integration.")
        .def_prop_ro("voi_name", &libOpenCOR::SedInstanceTask::voiName, "Return the name of the variable of integration.")
        .def_prop_ro("voi_unit", &libOpenCOR::SedInstanceTask::voiUnit, "Return the unit of the variable of integration.")
        .def_prop_ro("state_count", &libOpenCOR::SedInstanceTask::stateCount, "Return the number of states.")
        .def("state", &libOpenCOR::SedInstanceTask::state, "Return the values of a state.")
        .def("state_name", &libOpenCOR::SedInstanceTask::stateName, "Return the name of a state.")
        .def("state_unit", &libOpenCOR::SedInstanceTask::stateUnit, "Return the unit of a state.")
        .def_prop_ro("rate_count", &libOpenCOR::SedInstanceTask::rateCount, "Return the number of rates.")
        .def("rate", &libOpenCOR::SedInstanceTask::rate, "Return the values of a rate.")
        .def("rate_name", &libOpenCOR::SedInstanceTask::rateName, "Return the name of a rate.")
        .def("rate_unit", &libOpenCOR::SedInstanceTask::rateUnit, "Return the unit of a rate.")
        .def_prop_ro("variable_count", &libOpenCOR::SedInstanceTask::variableCount, "Return the number of variables.")
        .def("variable", &libOpenCOR::SedInstanceTask::variable, "Return the values of a variable.")
        .def("variable_name", &libOpenCOR::SedInstanceTask::variableName, "Return the name of a variable.")
        .def("variable_unit", &libOpenCOR::SedInstanceTask::variableUnit, "Return the unit of a variable.");

    // SedModel API.

    nb::class_<libOpenCOR::SedModel, libOpenCOR::SedBase> sedModel(m, "SedModel");

    sedModel.def(nb::new_(&libOpenCOR::SedModel::create), "Create a SedModel object.");

    // SedOutput API.

    nb::class_<libOpenCOR::SedOutput, libOpenCOR::SedBase> sedOutput(m, "SedOutput");

    // SedRepeatedTask API.

    /*---GRY---
        nb::class_<libOpenCOR::SedRepeatedTask, libOpenCOR::SedAbstractTask> sedRepeatedTask(m, "SedRepeatedTask");

        sedRepeatedTask.def(nb::new_(&libOpenCOR::SedRepeatedTask::create), "document"_a, "Create a SedRepeatedTask object.");
    */

    // SedSimulation API.

    nb::class_<libOpenCOR::SedSimulation, libOpenCOR::SedBase> sedSimulation(m, "SedSimulation");

    sedSimulation.def_prop_rw("ode_solver", &libOpenCOR::SedSimulation::odeSolver, &libOpenCOR::SedSimulation::setOdeSolver, "ode_solver"_a.none(), "The ODE solver for the SedSimulation object.")
        .def_prop_rw("nla_solver", &libOpenCOR::SedSimulation::nlaSolver, &libOpenCOR::SedSimulation::setNlaSolver, "nla_solver"_a.none(), "The NLA solver for the SedSimulation object.");

    // SedOneStep API.

    nb::class_<libOpenCOR::SedOneStep, libOpenCOR::SedSimulation> sedOneStep(m, "SedOneStep");

    sedOneStep.def(nb::new_(&libOpenCOR::SedOneStep::create), "Create a SedOneStep object.")
        .def_prop_rw("step", &libOpenCOR::SedOneStep::step, &libOpenCOR::SedOneStep::setStep, "The step of the SedOneStep object.");

    // SedSteadyState API.

    nb::class_<libOpenCOR::SedSteadyState, libOpenCOR::SedSimulation> sedSteadyState(m, "SedSteadyState");

    sedSteadyState.def(nb::new_(&libOpenCOR::SedSteadyState::create), "Create a SedSteadyState object.");

    // SedUniformTimeCourse API.

    nb::class_<libOpenCOR::SedUniformTimeCourse, libOpenCOR::SedSimulation> sedUniformTimeCourse(m, "SedUniformTimeCourse");

    sedUniformTimeCourse.def(nb::new_(&libOpenCOR::SedUniformTimeCourse::create), "Create a SedUniformTimeCourse object.")
        .def_prop_rw("initial_time", &libOpenCOR::SedUniformTimeCourse::initialTime, &libOpenCOR::SedUniformTimeCourse::setInitialTime, "The initial time of the SedUniformTimeCourse object.")
        .def_prop_rw("output_start_time", &libOpenCOR::SedUniformTimeCourse::outputStartTime, &libOpenCOR::SedUniformTimeCourse::setOutputStartTime, "The output start time of the SedUniformTimeCourse object.")
        .def_prop_rw("output_end_time", &libOpenCOR::SedUniformTimeCourse::outputEndTime, &libOpenCOR::SedUniformTimeCourse::setOutputEndTime, "The output end time of the SedUniformTimeCourse object.")
        .def_prop_rw("number_of_steps", &libOpenCOR::SedUniformTimeCourse::numberOfSteps, &libOpenCOR::SedUniformTimeCourse::setNumberOfSteps, "The number of steps of the SedUniformTimeCourse object.");

    // SedStyle API.

    nb::class_<libOpenCOR::SedStyle, libOpenCOR::SedBase> sedStyle(m, "SedStyle");

    // SedTask API.

    nb::class_<libOpenCOR::SedTask, libOpenCOR::SedAbstractTask> sedTask(m, "SedTask");

    sedTask.def(nb::new_(&libOpenCOR::SedTask::create), "Create a SedTask object.")
        .def_prop_rw("model", &libOpenCOR::SedTask::model, &libOpenCOR::SedTask::setModel, "model"_a.none(), "The model of the SedTask object.")
        .def_prop_rw("simulation", &libOpenCOR::SedTask::simulation, &libOpenCOR::SedTask::setSimulation, "simulation"_a.none(), "The simulation of the SedTask object.");
}
