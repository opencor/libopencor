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
using namespace nb::literals;

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

    file.def(nb::new_(&libOpenCOR::File::create), "Create a File object.")
        .def_prop_ro("type", &libOpenCOR::File::type, "Get the type of this File object.")
        .def_prop_ro("file_name", &libOpenCOR::File::fileName, "Get the file name for this File object.")
        .def_prop_ro("url", &libOpenCOR::File::url, "Get the URL for this File object.")
        .def_prop_ro("path", &libOpenCOR::File::path, "Get the path for this File object.")
        .def_prop_rw("contents", &libOpenCOR::File::contents, &libOpenCOR::File::setContents, "The contents of this File object.");
}
