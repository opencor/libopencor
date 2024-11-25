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
        .def(nb::new_(&libOpenCOR::SedDocument::create), "Create a SedDocument object.", nb::arg("file").none())
        .def("serialise", nb::overload_cast<>(&libOpenCOR::SedDocument::serialise, nb::const_), "Get the serialised version of this SedDocument object.")
        .def("serialise", nb::overload_cast<const std::string &>(&libOpenCOR::SedDocument::serialise, nb::const_), "Get the serialised version of this SedDocument object.", nb::arg("base_path"))
        .def_prop_ro("has_models", &libOpenCOR::SedDocument::hasModels, "Return whether there are some models.")
        .def_prop_ro("model_count", &libOpenCOR::SedDocument::modelCount, "Return the number of models.")
        .def_prop_ro("models", &libOpenCOR::SedDocument::models, "Return the models.")
        .def("model", &libOpenCOR::SedDocument::model, "Return the model.")
        .def("add_model", &libOpenCOR::SedDocument::addModel, "Add a model.", nb::arg("model").none())
        .def("remove_model", &libOpenCOR::SedDocument::removeModel, "Remove a model.", nb::arg("model").none())
        .def_prop_ro("has_simulations", &libOpenCOR::SedDocument::hasSimulations, "Return whether there are some simulations.")
        .def_prop_ro("simulation_count", &libOpenCOR::SedDocument::simulationCount, "Return the number of simulations.")
        .def_prop_ro("simulations", &libOpenCOR::SedDocument::simulations, "Return the simulations.")
        .def("simulation", &libOpenCOR::SedDocument::simulation, "Return the simulation.")
        .def("add_simulation", &libOpenCOR::SedDocument::addSimulation, "Add a simulation.", nb::arg("simulation").none())
        .def("remove_simulation", &libOpenCOR::SedDocument::removeSimulation, "Remove a simulation.", nb::arg("simulation").none())
        .def_prop_ro("has_tasks", &libOpenCOR::SedDocument::hasTasks, "Return whether there are some tasks.")
        .def_prop_ro("task_count", &libOpenCOR::SedDocument::taskCount, "Return the number of tasks.")
        .def_prop_ro("tasks", &libOpenCOR::SedDocument::tasks, "Return the tasks.")
        .def("task", &libOpenCOR::SedDocument::task, "Return the task.")
        .def("add_task", &libOpenCOR::SedDocument::addTask, "Add a task.", nb::arg("task").none())
        .def("remove_task", &libOpenCOR::SedDocument::removeTask, "Remove a task.", nb::arg("task").none())
        .def("instantiate", &libOpenCOR::SedDocument::instantiate, "Instantiate this SedDocument object.", nb::arg("compiled") = true);

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
        .def_prop_ro("constant_count", &libOpenCOR::SedInstanceTask::constantCount, "Return the number of constants.")
        .def("constant", &libOpenCOR::SedInstanceTask::constant, "Return the values of a constant.")
        .def("constant_name", &libOpenCOR::SedInstanceTask::constantName, "Return the name of a constant.")
        .def("constant_unit", &libOpenCOR::SedInstanceTask::constantUnit, "Return the unit of a constant.")
        .def_prop_ro("computed_constant_count", &libOpenCOR::SedInstanceTask::computedConstantCount, "Return the number of computed constants.")
        .def("computed_constant", &libOpenCOR::SedInstanceTask::computedConstant, "Return the values of a computed constant.")
        .def("computed_constant_name", &libOpenCOR::SedInstanceTask::computedConstantName, "Return the name of a computed constant.")
        .def("computed_constant_unit", &libOpenCOR::SedInstanceTask::computedConstantUnit, "Return the unit of a computed constant.")
        .def_prop_ro("algebraic_count", &libOpenCOR::SedInstanceTask::algebraicCount, "Return the number of algebraic variables.")
        .def("algebraic", &libOpenCOR::SedInstanceTask::algebraic, "Return the values of an algebraic variable.")
        .def("algebraic_name", &libOpenCOR::SedInstanceTask::algebraicName, "Return the name of an algebraic variable.")
        .def("algebraic_unit", &libOpenCOR::SedInstanceTask::algebraicUnit, "Return the unit of an algebraic variable.");

    // SedModel API.

    nb::class_<libOpenCOR::SedModel, libOpenCOR::SedBase> sedModel(m, "SedModel");

    sedModel.def(nb::new_(&libOpenCOR::SedModel::create), "Create a SedModel object.", nb::arg("document"), nb::arg("file"))
        .def_prop_ro("file", &libOpenCOR::SedModel::file, "Return the file.");

    // SedOutput API.

    nb::class_<libOpenCOR::SedOutput, libOpenCOR::SedBase> sedOutput(m, "SedOutput");

    // SedRepeatedTask API.

    /*---GRY---
        nb::class_<libOpenCOR::SedRepeatedTask, libOpenCOR::SedAbstractTask> sedRepeatedTask(m, "SedRepeatedTask");

        sedRepeatedTask.def(nb::new_(&libOpenCOR::SedRepeatedTask::create), "Create a SedRepeatedTask object.", nb::arg("document"));
    */

    // SedSimulation API.

    nb::class_<libOpenCOR::SedSimulation, libOpenCOR::SedBase> sedSimulation(m, "SedSimulation");

    sedSimulation.def_prop_rw("ode_solver", &libOpenCOR::SedSimulation::odeSolver, &libOpenCOR::SedSimulation::setOdeSolver, "The ODE solver for the SedSimulation object.", nb::arg("ode_solver").none())
        .def_prop_rw("nla_solver", &libOpenCOR::SedSimulation::nlaSolver, &libOpenCOR::SedSimulation::setNlaSolver, "The NLA solver for the SedSimulation object.", nb::arg("nla_solver").none());

    // SedAnalysis API.

    nb::class_<libOpenCOR::SedAnalysis, libOpenCOR::SedSimulation> sedAnalysis(m, "SedAnalysis");

    sedAnalysis.def(nb::new_(&libOpenCOR::SedAnalysis::create), "Create a SedAnalysis object.", nb::arg("document"));

    // SedOneStep API.

    nb::class_<libOpenCOR::SedOneStep, libOpenCOR::SedSimulation> sedOneStep(m, "SedOneStep");

    sedOneStep.def(nb::new_(&libOpenCOR::SedOneStep::create), "Create a SedOneStep object.", nb::arg("document"))
        .def_prop_rw("step", &libOpenCOR::SedOneStep::step, &libOpenCOR::SedOneStep::setStep, "The step of the SedOneStep object.");

    // SedSteadyState API.

    nb::class_<libOpenCOR::SedSteadyState, libOpenCOR::SedSimulation> sedSteadyState(m, "SedSteadyState");

    sedSteadyState.def(nb::new_(&libOpenCOR::SedSteadyState::create), "Create a SedSteadyState object.", nb::arg("document"));

    // SedUniformTimeCourse API.

    nb::class_<libOpenCOR::SedUniformTimeCourse, libOpenCOR::SedSimulation> sedUniformTimeCourse(m, "SedUniformTimeCourse");

    sedUniformTimeCourse.def(nb::new_(&libOpenCOR::SedUniformTimeCourse::create), "Create a SedUniformTimeCourse object.", nb::arg("document"))
        .def_prop_rw("initial_time", &libOpenCOR::SedUniformTimeCourse::initialTime, &libOpenCOR::SedUniformTimeCourse::setInitialTime, "The initial time of the SedUniformTimeCourse object.")
        .def_prop_rw("output_start_time", &libOpenCOR::SedUniformTimeCourse::outputStartTime, &libOpenCOR::SedUniformTimeCourse::setOutputStartTime, "The output start time of the SedUniformTimeCourse object.")
        .def_prop_rw("output_end_time", &libOpenCOR::SedUniformTimeCourse::outputEndTime, &libOpenCOR::SedUniformTimeCourse::setOutputEndTime, "The output end time of the SedUniformTimeCourse object.")
        .def_prop_rw("number_of_steps", &libOpenCOR::SedUniformTimeCourse::numberOfSteps, &libOpenCOR::SedUniformTimeCourse::setNumberOfSteps, "The number of steps of the SedUniformTimeCourse object.");

    // SedStyle API.

    nb::class_<libOpenCOR::SedStyle, libOpenCOR::SedBase> sedStyle(m, "SedStyle");

    // SedTask API.

    nb::class_<libOpenCOR::SedTask, libOpenCOR::SedAbstractTask> sedTask(m, "SedTask");

    sedTask.def(nb::new_(&libOpenCOR::SedTask::create), "Create a SedTask object.", nb::arg("document"), nb::arg("model"), nb::arg("simulation"))
        .def_prop_rw("model", &libOpenCOR::SedTask::model, &libOpenCOR::SedTask::setModel, "The model of the SedTask object.", nb::arg("model").none())
        .def_prop_rw("simulation", &libOpenCOR::SedTask::simulation, &libOpenCOR::SedTask::setSimulation, "The simulation of the SedTask object.", nb::arg("simulation").none());
}
