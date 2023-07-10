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
    emscripten::class_<libOpenCOR::Logger>("Logger")
        .function("hasIssues", &libOpenCOR::Logger::hasIssues)
        .function("issueCount", &libOpenCOR::Logger::issueCount)
        .function("issue", &libOpenCOR::Logger::issue)
        .function("hasErrors", &libOpenCOR::Logger::hasErrors)
        .function("errorCount", &libOpenCOR::Logger::errorCount)
        .function("error", &libOpenCOR::Logger::error)
        /*---GRY---
                .function("hasWarnings", &libOpenCOR::Logger::hasWarnings)
                .function("warningCount", &libOpenCOR::Logger::warningCount)
                .function("warning", &libOpenCOR::Logger::warning)
                .function("hasMessages", &libOpenCOR::Logger::hasMessages)
                .function("messageCount", &libOpenCOR::Logger::messageCount)
                .function("message", &libOpenCOR::Logger::message)
        */
        ;
}
