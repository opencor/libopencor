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
        .def_property_readonly("simulations", &libOpenCOR::SedDocument::simulations, "Return the simulations.")
        .def("add_simulation", &libOpenCOR::SedDocument::addSimulation, "Add the simulations.")
        .def("remove_simulation", &libOpenCOR::SedDocument::removeSimulation, "Remove the simulations.");

    // SedModel API.

    py::class_<libOpenCOR::SedModel, libOpenCOR::SedBase, libOpenCOR::SedModelPtr> sedModel(m, "SedModel");

    sedModel.def(py::init(&libOpenCOR::SedModel::create), "Create a SedModel object.", py::arg("file"), py::arg("document"));

    // SedOutput API.

    py::class_<libOpenCOR::SedOutput, libOpenCOR::SedBase, libOpenCOR::SedOutputPtr> sedOutput(m, "SedOutput");

    // SedSimulation API.

    py::class_<libOpenCOR::SedSimulation, libOpenCOR::SedBase, libOpenCOR::SedSimulationPtr> sedSimulation(m, "SedSimulation");

    // SedSimulationOneStep API.

    py::class_<libOpenCOR::SedSimulationOneStep, libOpenCOR::SedSimulation, libOpenCOR::SedSimulationOneStepPtr> sedSimulationOneStep(m, "SedSimulationOneStep");

    sedSimulationOneStep.def(py::init(&libOpenCOR::SedSimulationOneStep::create), "Create a SedSimulationOneStep object.", py::arg("document"));

    // SedSimulationSteadyState API.

    py::class_<libOpenCOR::SedSimulationSteadyState, libOpenCOR::SedSimulation, libOpenCOR::SedSimulationSteadyStatePtr> sedSimulationSteadyState(m, "SedSimulationSteadyState");

    sedSimulationSteadyState.def(py::init(&libOpenCOR::SedSimulationSteadyState::create), "Create a SedSimulationSteadyState object.", py::arg("document"));

    // SedSimulationUniformTimeCourse API.

    py::class_<libOpenCOR::SedSimulationUniformTimeCourse, libOpenCOR::SedSimulation, libOpenCOR::SedSimulationUniformTimeCoursePtr> sedSimulationUniformTimeCourse(m, "SedSimulationUniformTimeCourse");

    sedSimulationUniformTimeCourse.def(py::init(&libOpenCOR::SedSimulationUniformTimeCourse::create), "Create a SedSimulationUniformTimeCourse object.", py::arg("document"));

    // SedStyle API.

    py::class_<libOpenCOR::SedStyle, libOpenCOR::SedBase, libOpenCOR::SedStylePtr> sedStyle(m, "SedStyle");
}
