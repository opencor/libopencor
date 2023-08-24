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

void sedDocumentApi()
{
    emscripten::class_<libOpenCOR::SedDocument, emscripten::base<libOpenCOR::SedBase>>("SedDocument")
        .smart_ptr_constructor("SedDocument", &libOpenCOR::SedDocument::create)
        .function("initialise", &libOpenCOR::SedDocument::initialise)
        .function("serialise", emscripten::select_overload<std::string() const>(&libOpenCOR::SedDocument::serialise))
        .function("serialise", emscripten::select_overload<std::string(const std::string &) const>(&libOpenCOR::SedDocument::serialise));
}
