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

void loggerApi()
{
    // Logger API.

    emscripten::class_<libOpenCOR::Logger>("Logger")
        .property("hasIssues", &libOpenCOR::Logger::hasIssues)
        .property("issueCount", &libOpenCOR::Logger::issueCount)
        .property("issues", &libOpenCOR::Logger::issues)
        .function("issue", &libOpenCOR::Logger::issue)
        .property("hasErrors", &libOpenCOR::Logger::hasErrors)
        .property("errorCount", &libOpenCOR::Logger::errorCount)
        .property("errors", &libOpenCOR::Logger::errors)
        .function("error", &libOpenCOR::Logger::error)
        .property("hasWarnings", &libOpenCOR::Logger::hasWarnings)
        .property("warningCount", &libOpenCOR::Logger::warningCount)
        .property("warnings", &libOpenCOR::Logger::warnings)
        .function("warning", &libOpenCOR::Logger::warning);

    // Issue API.

    emscripten::enum_<libOpenCOR::Issue::Type>("Issue.Type")
        .value("ERROR", libOpenCOR::Issue::Type::ERROR)
        .value("WARNING", libOpenCOR::Issue::Type::WARNING);

    emscripten::class_<libOpenCOR::Issue>("Issue")
        .smart_ptr<libOpenCOR::IssuePtr>("Issue")
        .property("type", &libOpenCOR::Issue::type)
        .property("typeAsString", &libOpenCOR::Issue::typeAsString)
        .property("description", &libOpenCOR::Issue::description);

    EM_ASM({
        Module["Issue"]["Type"] = Module["Issue.Type"];

        delete Module["Issue.Type"];
    });
}
