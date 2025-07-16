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

void versionApi()
{
    // Version API.

    emscripten::function("version", &libOpenCOR::version);
    emscripten::function("versionString", &libOpenCOR::versionString);
    emscripten::function("clangVersion", &libOpenCOR::clangVersion);
    emscripten::function("clangVersionString", &libOpenCOR::clangVersionString);
    emscripten::function("libcellmlVersion", &libOpenCOR::libcellmlVersion);
    emscripten::function("libcellmlVersionString", &libOpenCOR::libcellmlVersionString);
    emscripten::function("libcombineVersion", &libOpenCOR::libcombineVersion);
    emscripten::function("libcombineVersionString", &libOpenCOR::libcombineVersionString);
#ifndef __EMSCRIPTEN__
    emscripten::function("libcurlVersion", &libOpenCOR::libcurlVersion);
    emscripten::function("libcurlVersionString", &libOpenCOR::libcurlVersionString);
#endif
    emscripten::function("libsedmlVersion", &libOpenCOR::libsedmlVersion);
    emscripten::function("libsedmlVersionString", &libOpenCOR::libsedmlVersionString);
    emscripten::function("llvmVersion", &libOpenCOR::llvmVersion);
    emscripten::function("llvmVersionString", &libOpenCOR::llvmVersionString);
    emscripten::function("sundialsVersion", &libOpenCOR::sundialsVersion);
    emscripten::function("sundialsVersionString", &libOpenCOR::sundialsVersionString);
}
