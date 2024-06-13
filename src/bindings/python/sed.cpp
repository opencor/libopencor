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
        .def("create_instance", &libOpenCOR::SedDocument::createInstance, "Create an instance of this SedDocument object.", py::arg("compiled") = true);

    // SedInstance API.

    py::class_<libOpenCOR::SedInstance, libOpenCOR::Logger, libOpenCOR::SedInstancePtr> sedInstance(m, "SedInstance");

    sedInstance.def("run", &libOpenCOR::SedInstance::run, "Run the tasks associated with this SedInstance object.")
        .def_property_readonly("tasks", &libOpenCOR::SedInstance::tasks, "Return the tasks.");

    // SedInstanceTask API.

    py::class_<libOpenCOR::SedInstanceTask, libOpenCOR::Logger, libOpenCOR::SedInstanceTaskPtr> sedInstanceTask(m, "SedInstanceTask");

    sedInstanceTask.def_property_readonly("voi", &libOpenCOR::SedInstanceTask::voi, "Return the values of the variable of integration.")
        .def_property_readonly("voi_name", &libOpenCOR::SedInstanceTask::voiName, "Return the name of the variable of integration.")
        .def_property_readonly("voi_unit", &libOpenCOR::SedInstanceTask::voiUnit, "Return the unit of the variable of integration.")
        .def_property_readonly("state_count", &libOpenCOR::SedInstanceTask::stateCount, "Return the number of states.")
        .def("state", &libOpenCOR::SedInstanceTask::state, "Return the values of a state.")
        .def("state_name", &libOpenCOR::SedInstanceTask::stateName, "Return the name of a state.")
        .def("state_unit", &libOpenCOR::SedInstanceTask::stateUnit, "Return the unit of a state.")
        .def_property_readonly("rate_count", &libOpenCOR::SedInstanceTask::rateCount, "Return the number of rates.")
        .def("rate", &libOpenCOR::SedInstanceTask::rate, "Return the values of a rate.")
        .def("rate_name", &libOpenCOR::SedInstanceTask::rateName, "Return the name of a rate.")
        .def("rate_unit", &libOpenCOR::SedInstanceTask::rateUnit, "Return the unit of a rate.")
        .def_property_readonly("variable_count", &libOpenCOR::SedInstanceTask::variableCount, "Return the number of variables.")
        .def("variable", &libOpenCOR::SedInstanceTask::variable, "Return the values of a variable.")
        .def("variable_name", &libOpenCOR::SedInstanceTask::variableName, "Return the name of a variable.")
        .def("variable_unit", &libOpenCOR::SedInstanceTask::variableUnit, "Return the unit of a variable.");

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

    // SedAnalysis API.

    py::class_<libOpenCOR::SedAnalysis, libOpenCOR::SedSimulation, libOpenCOR::SedAnalysisPtr> sedAnalysis(m, "SedAnalysis");

    sedAnalysis.def(py::init(&libOpenCOR::SedAnalysis::create), "Create a SedAnalysis object.", py::arg("document"));

    // SedOneStep API.

    py::class_<libOpenCOR::SedOneStep, libOpenCOR::SedSimulation, libOpenCOR::SedOneStepPtr> sedOneStep(m, "SedOneStep");

    sedOneStep.def(py::init(&libOpenCOR::SedOneStep::create), "Create a SedOneStep object.", py::arg("document"))
        .def_property("step", &libOpenCOR::SedOneStep::step, &libOpenCOR::SedOneStep::setStep, "The step of the SedOneStep object.");

    // SedSteadyState API.

    py::class_<libOpenCOR::SedSteadyState, libOpenCOR::SedSimulation, libOpenCOR::SedSteadyStatePtr> sedSteadyState(m, "SedSteadyState");

    sedSteadyState.def(py::init(&libOpenCOR::SedSteadyState::create), "Create a SedSteadyState object.", py::arg("document"));

    // SedUniformTimeCourse API.

    py::class_<libOpenCOR::SedUniformTimeCourse, libOpenCOR::SedSimulation, libOpenCOR::SedUniformTimeCoursePtr> sedUniformTimeCourse(m, "SedUniformTimeCourse");

    sedUniformTimeCourse.def(py::init(&libOpenCOR::SedUniformTimeCourse::create), "Create a SedUniformTimeCourse object.", py::arg("document"))
        .def_property("initial_time", &libOpenCOR::SedUniformTimeCourse::initialTime, &libOpenCOR::SedUniformTimeCourse::setInitialTime, "The initial time of the SedUniformTimeCourse object.")
        .def_property("output_start_time", &libOpenCOR::SedUniformTimeCourse::outputStartTime, &libOpenCOR::SedUniformTimeCourse::setOutputStartTime, "The output start time of the SedUniformTimeCourse object.")
        .def_property("output_end_time", &libOpenCOR::SedUniformTimeCourse::outputEndTime, &libOpenCOR::SedUniformTimeCourse::setOutputEndTime, "The output end time of the SedUniformTimeCourse object.")
        .def_property("number_of_steps", &libOpenCOR::SedUniformTimeCourse::numberOfSteps, &libOpenCOR::SedUniformTimeCourse::setNumberOfSteps, "The number of steps of the SedUniformTimeCourse object.");

    // SedStyle API.

    py::class_<libOpenCOR::SedStyle, libOpenCOR::SedBase, libOpenCOR::SedStylePtr> sedStyle(m, "SedStyle");

    // SedTask API.

    py::class_<libOpenCOR::SedTask, libOpenCOR::SedAbstractTask, libOpenCOR::SedTaskPtr> sedTask(m, "SedTask");

    sedTask.def(py::init(&libOpenCOR::SedTask::create), "Create a SedTask object.", py::arg("document"), py::arg("model"), py::arg("simulation"))
        .def_property("model", &libOpenCOR::SedTask::model, &libOpenCOR::SedTask::setModel, "The model of the SedTask object.")
        .def_property("simulation", &libOpenCOR::SedTask::simulation, &libOpenCOR::SedTask::setSimulation, "The simulation of the SedTask object.");
}
