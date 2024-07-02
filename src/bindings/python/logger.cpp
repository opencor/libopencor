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

#include <pybind11/stl.h>

namespace py = pybind11;

void loggerApi(py::module_ &m)
{
    // Logger API.

    py::class_<libOpenCOR::Logger, libOpenCOR::LoggerPtr> logger(m, "Logger");

    logger.def_property_readonly("has_issues", &libOpenCOR::Logger::hasIssues, "Return whether there are some issues.")
        .def_property_readonly("issue_count", &libOpenCOR::Logger::issueCount, "Return the number of issues.")
        .def_property_readonly("issues", &libOpenCOR::Logger::issues, "Return the issues.")
        .def_property_readonly("has_errors", &libOpenCOR::Logger::hasErrors, "Return whether there are some errors.")
        .def_property_readonly("error_count", &libOpenCOR::Logger::errorCount, "Return the number of errors.")
        .def_property_readonly("errors", &libOpenCOR::Logger::errors, "Return the errors.")
        .def_property_readonly("has_warnings", &libOpenCOR::Logger::hasWarnings, "Return whether there are some warnings.")
        .def_property_readonly("warning_count", &libOpenCOR::Logger::warningCount, "Return the number of warnings.")
        .def_property_readonly("warnings", &libOpenCOR::Logger::warnings, "Return the warnings.")
        .def_property_readonly("has_messages", &libOpenCOR::Logger::hasMessages, "Return whether there are some messages.")
        .def_property_readonly("message_count", &libOpenCOR::Logger::messageCount, "Return the number of messages.")
        .def_property_readonly("messages", &libOpenCOR::Logger::messages, "Return the messages.");

    // Issue API.

    py::class_<libOpenCOR::Issue, libOpenCOR::IssuePtr> issue(m, "Issue");

    py::enum_<libOpenCOR::Issue::Type>(issue, "Type")
        .value("Error", libOpenCOR::Issue::Type::ERROR)
        .value("Warning", libOpenCOR::Issue::Type::WARNING)
        .value("Message", libOpenCOR::Issue::Type::MESSAGE)
        .export_values();

    issue.def_property_readonly("type", &libOpenCOR::Issue::type, "Get the type of this Issue object.")
        .def_property_readonly("type_as_string", &libOpenCOR::Issue::typeAsString, "Get the type of this Issue object as a string.")
        .def_property_readonly("description", &libOpenCOR::Issue::description, "Get the description for this Issue object.");
}
