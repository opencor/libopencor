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

void fileApi();
void loggerApi();
void sedApi();
void solverApi();
void versionApi();

EMSCRIPTEN_BINDINGS(libOpenCOR)
{
    // Vectors.

    emscripten::register_vector<libOpenCOR::IssuePtr>("IssuePtrVector");

    // APIs.

    loggerApi(); // Note: it needs to be first since it is used by some other APIs.

    fileApi();
    sedApi();
    solverApi();
    versionApi();
}
