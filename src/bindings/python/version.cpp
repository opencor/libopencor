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

#include <nanobind/stl/string.h>

namespace nb = nanobind;

void versionApi(nb::module_ &m)
{
    // Version API.

    m.def("version", &libOpenCOR::version, "Get the version number of libOpenCOR.")
        .def("version_string", &libOpenCOR::versionString, "Get the version string of libOpenCOR.")
        .def("clang_version", &libOpenCOR::clangVersion, "Get the version number of Clang.")
        .def("clang_version_string", &libOpenCOR::clangVersionString, "Get the version string of Clang.")
        .def("libcellml_version", &libOpenCOR::libcellmlVersion, "Get the version number of libCellML.")
        .def("libcellml_version_string", &libOpenCOR::libcellmlVersionString, "Get the version string of libCellML.")
        .def("libcombine_version", &libOpenCOR::libcombineVersion, "Get the version number of libCOMBINE.")
        .def("libcombine_version_string", &libOpenCOR::libcombineVersionString, "Get the version string of libCOMBINE.")
        .def("libcurl_version", &libOpenCOR::libcurlVersion, "Get the version number of libcurl.")
        .def("libcurl_version_string", &libOpenCOR::libcurlVersionString, "Get the version string of libcurl.")
        .def("libsedml_version", &libOpenCOR::libsedmlVersion, "Get the version number of libSEDML.")
        .def("libsedml_version_string", &libOpenCOR::libsedmlVersionString, "Get the version string of libSEDML.")
        .def("llvm_version", &libOpenCOR::llvmVersion, "Get the version number of LLVM.")
        .def("llvm_version_string", &libOpenCOR::llvmVersionString, "Get the version string of LLVM.")
        .def("sundials_version", &libOpenCOR::sundialsVersion, "Get the version number of SUNDIALS.")
        .def("sundials_version_string", &libOpenCOR::sundialsVersionString, "Get the version string of SUNDIALS.");
}
