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

void fileApi(py::module_ &m)
{
    // File API.

    py::class_<libOpenCOR::File, libOpenCOR::Logger, std::shared_ptr<libOpenCOR::File>> file(m, "File");

    py::enum_<libOpenCOR::File::Type>(file, "Type")
        .value("UnknownFile", libOpenCOR::File::Type::UNKNOWN_FILE)
        .value("CellmlFile", libOpenCOR::File::Type::CELLML_FILE)
        .value("SedmlFile", libOpenCOR::File::Type::SEDML_FILE)
        .value("CombineArchive", libOpenCOR::File::Type::COMBINE_ARCHIVE)
        .value("IrretrievableFile", libOpenCOR::File::Type::IRRETRIEVABLE_FILE)
        .export_values();

    file.def(py::init(py::overload_cast<const std::string &>(&libOpenCOR::File::create)), "Create a File object.", py::arg("file_name_or_url"))
        .def(py::init(py::overload_cast<const std::string &, const std::vector<unsigned char> &>(&libOpenCOR::File::create)), "Create a File object.", py::arg("file_name_or_url"), py::arg("contents"))
        .def_property_readonly("type", &libOpenCOR::File::type, "Get the type of this File object.")
        .def_property_readonly("file_name", &libOpenCOR::File::fileName, "Get the file name for this File object.")
        .def_property_readonly("url", &libOpenCOR::File::url, "Get the URL for this File object.")
        .def_property_readonly("contents", &libOpenCOR::File::contents, "Get the contents of this File object.");
}
