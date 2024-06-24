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

void fileApi(py::module_ &m)
{
    // File API.

    py::class_<libOpenCOR::File, libOpenCOR::Logger, libOpenCOR::FilePtr> file(m, "File");

    py::enum_<libOpenCOR::File::Type>(file, "Type")
        .value("UnknownFile", libOpenCOR::File::Type::UNKNOWN_FILE)
        .value("CellmlFile", libOpenCOR::File::Type::CELLML_FILE)
        .value("SedmlFile", libOpenCOR::File::Type::SEDML_FILE)
        .value("CombineArchive", libOpenCOR::File::Type::COMBINE_ARCHIVE)
        .value("IrretrievableFile", libOpenCOR::File::Type::IRRETRIEVABLE_FILE)
        .export_values();

    file.def(py::init(&libOpenCOR::File::create), "Create a File object.", py::arg("file_name_or_url"), py::arg("managed") = true)
        .def_property_readonly("type", &libOpenCOR::File::type, "Get the type of this File object.")
        .def_property_readonly("file_name", &libOpenCOR::File::fileName, "Get the file name for this File object.")
        .def_property_readonly("url", &libOpenCOR::File::url, "Get the URL for this File object.")
        .def_property_readonly("path", &libOpenCOR::File::path, "Get the path for this File object.")
        .def_property("contents", &libOpenCOR::File::contents, &libOpenCOR::File::setContents, "The contents of this File object.")
        .def_property_readonly("has_child_files", &libOpenCOR::File::hasChildFiles, "Return whether this File object has some child files.")
        .def_property_readonly("child_file_count", &libOpenCOR::File::childFileCount, "Return the number of child files for this File object.")
        .def_property_readonly("child_file_names", &libOpenCOR::File::childFileNames, "Return the child file names for this File object.")
        .def_property_readonly("child_files", &libOpenCOR::File::childFiles, "Return the child files for this File object.")
        .def("child_file", &libOpenCOR::File::childFile, "Get the requested child file for this File object.", py::arg("file_name"));

    // FileManager API.

    py::class_<libOpenCOR::FileManager, std::unique_ptr<libOpenCOR::FileManager, py::nodelete>> fileManager(m, "FileManager");

    fileManager.def_static("instance", &libOpenCOR::FileManager::instance, "Get the file manager instance.")
        .def("manage", &libOpenCOR::FileManager::manage, "Manage the requested file.", py::arg("file"))
        .def("unmanage", &libOpenCOR::FileManager::unmanage, "Unmanage the requested file.", py::arg("file"))
        .def_property_readonly("has_files", &libOpenCOR::FileManager::hasFiles, "Return whether there are managed files.")
        .def_property_readonly("file_count", &libOpenCOR::FileManager::fileCount, "Return the number of managed files.")
        .def_property_readonly("files", &libOpenCOR::FileManager::files, "Return the managed files.")
        .def("file", &libOpenCOR::FileManager::file, "Get the requested managed file.", py::arg("file_name_or_url"));
}
