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

void loggerApi(nb::module_ &m)
{
    // Logger API.

    nb::class_<libOpenCOR::Logger> logger(m, "Logger");

    logger.def_prop_ro("has_issues", &libOpenCOR::Logger::hasIssues, "Return whether there are some issues.")
        .def_prop_ro("issues", &libOpenCOR::Logger::issues, "Return the issues.")
        .def_prop_ro("has_errors", &libOpenCOR::Logger::hasErrors, "Return whether there are some errors.")
        .def_prop_ro("errors", &libOpenCOR::Logger::errors, "Return the errors.")
        .def_prop_ro("has_warnings", &libOpenCOR::Logger::hasWarnings, "Return whether there are some warnings.")
        .def_prop_ro("warnings", &libOpenCOR::Logger::warnings, "Return the warnings.")
        .def_prop_ro("has_messages", &libOpenCOR::Logger::hasMessages, "Return whether there are some messages.")
        .def_prop_ro("messages", &libOpenCOR::Logger::messages, "Return the messages.");

    // Issue API.

    nb::class_<libOpenCOR::Issue> issue(m, "Issue");

    nb::enum_<libOpenCOR::Issue::Type>(issue, "Type")
        .value("Error", libOpenCOR::Issue::Type::ERROR)
        .value("Warning", libOpenCOR::Issue::Type::WARNING)
        .value("Message", libOpenCOR::Issue::Type::MESSAGE)
        .export_values();

    issue.def_prop_ro("type", &libOpenCOR::Issue::type, "Get the type of this Issue object.")
        .def_prop_ro("type_as_string", &libOpenCOR::Issue::typeAsString, "Get the type of this Issue object as a string.")
        .def_prop_ro("description", &libOpenCOR::Issue::description, "Get the description for this Issue object.");
}
