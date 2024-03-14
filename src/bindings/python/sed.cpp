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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void sedApi(py::module_ &m)
{
    // SedBase API.

    py::class_<libOpenCOR::SedBase, libOpenCOR::Logger, libOpenCOR::SedBasePtr> sedBase(m, "SedBase");

    // SedAbstractTask API.

    py::class_<libOpenCOR::SedAbstractTask, libOpenCOR::SedBase, libOpenCOR::SedAbstractTaskPtr> sedAbstractTask(m, "SedAbstractTask");

    // SedDataDescription API.

    py::class_<libOpenCOR::SedDataDescription, libOpenCOR::SedBase, libOpenCOR::SedDataDescriptionPtr> sedDataDescription(m, "SedDataDescription");

    // SedDataGenerator API.

    py::class_<libOpenCOR::SedDataGenerator, libOpenCOR::SedBase, libOpenCOR::SedDataGeneratorPtr> sedDataGenerator(m, "SedDataGenerator");

    // SedDocument API.

    py::class_<libOpenCOR::SedDocument, libOpenCOR::Logger, libOpenCOR::SedDocumentPtr> sedDocument(m, "SedDocument");

    sedDocument.def(py::init(&libOpenCOR::SedDocument::create), "Create a SedDocument object.", py::arg("file") = libOpenCOR::FilePtr())
        .def("serialise", py::overload_cast<>(&libOpenCOR::SedDocument::serialise, py::const_), "Get the serialised version of this SedDocument object.")
        .def("serialise", py::overload_cast<const std::string &>(&libOpenCOR::SedDocument::serialise, py::const_), "Get the serialised version of this SedDocument object.", py::arg("base_path"))
        .def_property_readonly("has_models", &libOpenCOR::SedDocument::hasModels, "Return whether there are some models.")
        .def_property_readonly("models", &libOpenCOR::SedDocument::models, "Return the models.")
        .def("add_model", &libOpenCOR::SedDocument::addModel, "Add a model.")
        .def("remove_model", &libOpenCOR::SedDocument::removeModel, "Remove a model.")
        .def_property_readonly("has_simulations", &libOpenCOR::SedDocument::hasSimulations, "Return whether there are some simulations.")
        .def_property_readonly("simulations", &libOpenCOR::SedDocument::simulations, "Return the simulations.")
        .def("add_simulation", &libOpenCOR::SedDocument::addSimulation, "Add a simulation.")
        .def("remove_simulation", &libOpenCOR::SedDocument::removeSimulation, "Remove a simulation.")
        .def_property_readonly("has_tasks", &libOpenCOR::SedDocument::hasTasks, "Return whether there are some tasks.")
        .def_property_readonly("tasks", &libOpenCOR::SedDocument::tasks, "Return the tasks.")
        .def("add_task", &libOpenCOR::SedDocument::addTask, "Add a task.")
        .def("remove_task", &libOpenCOR::SedDocument::removeTask, "Remove a task.")
        .def("run", &libOpenCOR::SedDocument::run, "Run this SedDocument object.");

    // SedModel API.

    py::class_<libOpenCOR::SedModel, libOpenCOR::SedBase, libOpenCOR::SedModelPtr> sedModel(m, "SedModel");

    sedModel.def(py::init(&libOpenCOR::SedModel::create), "Create a SedModel object.", py::arg("document"), py::arg("file"));

    // SedOutput API.

    py::class_<libOpenCOR::SedOutput, libOpenCOR::SedBase, libOpenCOR::SedOutputPtr> sedOutput(m, "SedOutput");

    // SedRepeatedTask API.

    /*---GRY---
        py::class_<libOpenCOR::SedRepeatedTask, libOpenCOR::SedAbstractTask, libOpenCOR::SedRepeatedTaskPtr> sedRepeatedTask(m, "SedRepeatedTask");

        sedRepeatedTask.def(py::init(&libOpenCOR::SedRepeatedTask::create), "Create a SedRepeatedTask object.", py::arg("document"));
    */

    // SedSimulation API.

    py::class_<libOpenCOR::SedSimulation, libOpenCOR::SedBase, libOpenCOR::SedSimulationPtr> sedSimulation(m, "SedSimulation");

    sedSimulation.def_property("ode_solver", &libOpenCOR::SedSimulation::odeSolver, &libOpenCOR::SedSimulation::setOdeSolver, "The ODE solver for the SedSimulation object.")
        .def_property("nla_solver", &libOpenCOR::SedSimulation::nlaSolver, &libOpenCOR::SedSimulation::setNlaSolver, "The NLA solver for the SedSimulation object.");

    // SedSimulationOneStep API.

    py::class_<libOpenCOR::SedSimulationOneStep, libOpenCOR::SedSimulation, libOpenCOR::SedSimulationOneStepPtr> sedSimulationOneStep(m, "SedSimulationOneStep");

    sedSimulationOneStep.def(py::init(&libOpenCOR::SedSimulationOneStep::create), "Create a SedSimulationOneStep object.", py::arg("document"))
        .def_property("step", &libOpenCOR::SedSimulationOneStep::step, &libOpenCOR::SedSimulationOneStep::setStep, "The step of the SedSimulationOneStep object.");

    // SedSimulationSteadyState API.

    py::class_<libOpenCOR::SedSimulationSteadyState, libOpenCOR::SedSimulation, libOpenCOR::SedSimulationSteadyStatePtr> sedSimulationSteadyState(m, "SedSimulationSteadyState");

    sedSimulationSteadyState.def(py::init(&libOpenCOR::SedSimulationSteadyState::create), "Create a SedSimulationSteadyState object.", py::arg("document"));

    // SedSimulationUniformTimeCourse API.

    py::class_<libOpenCOR::SedSimulationUniformTimeCourse, libOpenCOR::SedSimulation, libOpenCOR::SedSimulationUniformTimeCoursePtr> sedSimulationUniformTimeCourse(m, "SedSimulationUniformTimeCourse");

    sedSimulationUniformTimeCourse.def(py::init(&libOpenCOR::SedSimulationUniformTimeCourse::create), "Create a SedSimulationUniformTimeCourse object.", py::arg("document"))
        .def_property("initial_time", &libOpenCOR::SedSimulationUniformTimeCourse::initialTime, &libOpenCOR::SedSimulationUniformTimeCourse::setInitialTime, "The initial time of the SedSimulationUniformTimeCourse object.")
        .def_property("output_start_time", &libOpenCOR::SedSimulationUniformTimeCourse::outputStartTime, &libOpenCOR::SedSimulationUniformTimeCourse::setOutputStartTime, "The output start time of the SedSimulationUniformTimeCourse object.")
        .def_property("output_end_time", &libOpenCOR::SedSimulationUniformTimeCourse::outputEndTime, &libOpenCOR::SedSimulationUniformTimeCourse::setOutputEndTime, "The output end time of the SedSimulationUniformTimeCourse object.")
        .def_property("number_of_steps", &libOpenCOR::SedSimulationUniformTimeCourse::numberOfSteps, &libOpenCOR::SedSimulationUniformTimeCourse::setNumberOfSteps, "The number of steps of the SedSimulationUniformTimeCourse object.");

    // SedStyle API.

    py::class_<libOpenCOR::SedStyle, libOpenCOR::SedBase, libOpenCOR::SedStylePtr> sedStyle(m, "SedStyle");

    // SedTask API.

    py::class_<libOpenCOR::SedTask, libOpenCOR::SedAbstractTask, libOpenCOR::SedTaskPtr> sedTask(m, "SedTask");

    sedTask.def(py::init(&libOpenCOR::SedTask::create), "Create a SedTask object.", py::arg("document"))
        .def_property("model", &libOpenCOR::SedTask::model, &libOpenCOR::SedTask::setModel, "The model of the SedTask object.")
        .def_property("simulation", &libOpenCOR::SedTask::simulation, &libOpenCOR::SedTask::setSimulation, "The simulation of the SedTask object.");
}
