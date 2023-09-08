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

namespace py = pybind11;

void loggerApi(py::module_ &m)
{
    // Logger API.

    py::class_<libOpenCOR::Logger, libOpenCOR::LoggerPtr> logger(m, "Logger");

    logger.def_property_readonly("has_issues", &libOpenCOR::Logger::hasIssues, "Return whether there are issues.")
        .def_property_readonly("issue_count", &libOpenCOR::Logger::issueCount, "Get the number of issues.")
        .def("issue", &libOpenCOR::Logger::issue, "Return the given issue.")
        .def_property_readonly("has_errors", &libOpenCOR::Logger::hasErrors, "Return whether there are errors.")
        .def_property_readonly("error_count", &libOpenCOR::Logger::errorCount, "Get the number of errors.")
        .def("error", &libOpenCOR::Logger::error, "Return the given error.")
        /*---GRY---
                .def_property_readonly("has_warnings", &libOpenCOR::Logger::hasWarnings, "Return whether there are warnings.")
                .def_property_readonly("warning_count", &libOpenCOR::Logger::warningCount, "Get the number of warnings.")
                .def("warning", &libOpenCOR::Logger::warning, "Return the given warning.")
                .def_property_readonly("has_messages", &libOpenCOR::Logger::hasMessages, "Return whether there are messages.")
                .def_property_readonly("message_count", &libOpenCOR::Logger::messageCount, "Get the number of messages.")
                .def("message", &libOpenCOR::Logger::message, "Return the given message.")
        */
        ;
}
