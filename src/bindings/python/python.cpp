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

#include "file_p.h"

#include <libopencor>

#include <pybind11/pybind11.h>

namespace py = pybind11;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

PYBIND11_MODULE(module, m)
{
    // Version.

    m.attr("__version__") = MACRO_STRINGIFY(PROJECT_VERSION);

    // Documentation.

    m.doc() = "libOpenCOR is the backend library to OpenCOR, an open source cross-platform modelling environment.";

    // File API.

/*---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
    py::class_<libOpenCOR::File, std::shared_ptr<libOpenCOR::File>> file(m, "File");

    py::enum_<libOpenCOR::File::Type>(file, "Type")
        .value("Unresolved", libOpenCOR::File::Type::UNRESOLVED)
        .value("CellmlFile", libOpenCOR::File::Type::CELLML_FILE)
        .value("SedmlFile", libOpenCOR::File::Type::SEDML_FILE)
        .value("CombineArchive", libOpenCOR::File::Type::COMBINE_ARCHIVE)
        .value("UnknownFile", libOpenCOR::File::Type::UNKNOWN_FILE)
        .export_values();

    py::enum_<libOpenCOR::File::Status>(file, "Status")
        .value("Ok", libOpenCOR::File::Status::OK)
        .value("NonRetrievableFile", libOpenCOR::File::Status::NON_RETRIEVABLE_FILE)
        .value("NonInstantiableFile", libOpenCOR::File::Status::NON_INSTANTIABLE_FILE)
        .export_values();

    file.def(py::init(&libOpenCOR::File::create), "Create a File object.", py::arg("file_name_or_url"))
        .def_property_readonly("type", &libOpenCOR::File::type, "Get the type of this File object.")
        .def_property_readonly("file_name", &libOpenCOR::File::fileName, "Get the file name for this File object.")
        .def_property_readonly("url", &libOpenCOR::File::url, "Get the URL for this File object.")
        .def("resolve", &libOpenCOR::File::resolve, "Resolve this File object.")
        .def("instantiate", &libOpenCOR::File::instantiate, "Instantiate this File object.");

    file.def(
        "__repr__", [](const libOpenCOR::File &pFile) {
            auto fileName = pFile.fileName();

            if (!fileName.empty()) {
                return "Local file: " + fileName;
            }

            return "Remote file: " + pFile.url();
        },
        "Get the string representation of this File object.");
*/

    // Version API.

    m.def("version", &libOpenCOR::version, "Get the version number of libOpenCOR.");
    m.def("version_string", &libOpenCOR::versionString, "Get the version string of libOpenCOR.");
/*---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
    m.def("clang_version", &libOpenCOR::clangVersion, "Get the version number of Clang.");
    m.def("clang_version_string", &libOpenCOR::clangVersionString, "Get the version string of Clang.");
*/
    m.def("libcellml_version", &libOpenCOR::libcellmlVersion, "Get the version number of libCellML.");
    m.def("libcellml_version_string", &libOpenCOR::libcellmlVersionString, "Get the version string of libCellML.");
/*---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
    m.def("libcombine_version", &libOpenCOR::libcombineVersion, "Get the version number of libCOMBINE.");
    m.def("libcombine_version_string", &libOpenCOR::libcombineVersionString, "Get the version string of libCOMBINE.");
    m.def("libcurl_version", &libOpenCOR::libcurlVersion, "Get the version number of libcurl.");
    m.def("libcurl_version_string", &libOpenCOR::libcurlVersionString, "Get the version string of libcurl.");
    m.def("libsedml_version", &libOpenCOR::libsedmlVersion, "Get the version number of libSEDML.");
    m.def("libsedml_version_string", &libOpenCOR::libsedmlVersionString, "Get the version string of libSEDML.");
    m.def("llvm_version", &libOpenCOR::llvmVersion, "Get the version number of LLVM.");
    m.def("llvm_version_string", &libOpenCOR::llvmVersionString, "Get the version string of LLVM.");
*/
    m.def("sundials_version", &libOpenCOR::sundialsVersion, "Get the version number of SUNDIALS.");
    m.def("sundials_version_string", &libOpenCOR::sundialsVersionString, "Get the version string of SUNDIALS.");
}
