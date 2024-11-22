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

void fileApi(nb::module_ &m)
{
    // File API.

    nb::class_<libOpenCOR::File, libOpenCOR::Logger> file(m, "File");

    nb::enum_<libOpenCOR::File::Type>(file, "Type")
        .value("UnknownFile", libOpenCOR::File::Type::UNKNOWN_FILE)
        .value("CellmlFile", libOpenCOR::File::Type::CELLML_FILE)
        .value("SedmlFile", libOpenCOR::File::Type::SEDML_FILE)
        .value("CombineArchive", libOpenCOR::File::Type::COMBINE_ARCHIVE)
        .value("IrretrievableFile", libOpenCOR::File::Type::IRRETRIEVABLE_FILE)
        .export_values();

    file.def(nb::new_(&libOpenCOR::File::create), "Create a File object.", nb::arg("file_name_or_url"))
        .def_prop_ro("type", &libOpenCOR::File::type, "Get the type of this File object.")
        .def_prop_ro("file_name", &libOpenCOR::File::fileName, "Get the file name for this File object.")
        .def_prop_ro("url", &libOpenCOR::File::url, "Get the URL for this File object.")
        .def_prop_ro("path", &libOpenCOR::File::path, "Get the path for this File object.")
        .def_prop_rw("contents", &libOpenCOR::File::contents, &libOpenCOR::File::setContents, "The contents of this File object.")
        .def_prop_ro("has_child_files", &libOpenCOR::File::hasChildFiles, "Return whether this File object has some child files.")
        .def_prop_ro("child_file_count", &libOpenCOR::File::childFileCount, "Return the number of child files for this File object.")
        .def_prop_ro("child_file_names", &libOpenCOR::File::childFileNames, "Return the child file names for this File object.")
        .def_prop_ro("child_files", &libOpenCOR::File::childFiles, "Return the child files for this File object.")
        .def("child_file", nb::overload_cast<size_t>(&libOpenCOR::File::childFile, nb::const_), "Get the requested child file for this File object.", nb::arg("index"))
        .def("child_file", nb::overload_cast<const std::string &>(&libOpenCOR::File::childFile, nb::const_), "Get the requested child file for this File object.", nb::arg("file_name"));

    // FileManager API.

    nb::class_<libOpenCOR::FileManager> fileManager(m, "FileManager");

    fileManager.def_static("instance", &libOpenCOR::FileManager::instance, "Get the file manager instance.")
        .def("manage", &libOpenCOR::FileManager::manage, "Manage the requested file.", nb::arg("file"))
        .def("unmanage", &libOpenCOR::FileManager::unmanage, "Unmanage the requested file.", nb::arg("file"))
        .def("reset", &libOpenCOR::FileManager::reset, "Reset the file manager.")
        .def_prop_ro("has_files", &libOpenCOR::FileManager::hasFiles, "Return whether there are managed files.")
        .def_prop_ro("file_count", &libOpenCOR::FileManager::fileCount, "Return the number of managed files.")
        .def_prop_ro("files", &libOpenCOR::FileManager::files, "Return the managed files.")
        .def("file", nb::overload_cast<size_t>(&libOpenCOR::FileManager::file, nb::const_), "Get the requested managed file.", nb::arg("index"))
        .def("file", nb::overload_cast<const std::string &>(&libOpenCOR::FileManager::file, nb::const_), "Get the requested managed file.", nb::arg("file_name_or_url"));
}
