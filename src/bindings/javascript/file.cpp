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

void fileApi()
{
    // File API.

    emscripten::enum_<libOpenCOR::File::Type>("File.Type")
        .value("UNKNOWN_FILE", libOpenCOR::File::Type::UNKNOWN_FILE)
        .value("CELLML_FILE", libOpenCOR::File::Type::CELLML_FILE)
        .value("SEDML_FILE", libOpenCOR::File::Type::SEDML_FILE)
        .value("COMBINE_ARCHIVE", libOpenCOR::File::Type::COMBINE_ARCHIVE);

    emscripten::class_<libOpenCOR::File, emscripten::base<libOpenCOR::Logger>>("File")
        .smart_ptr_constructor("File", &libOpenCOR::File::create)
        .function("type", &libOpenCOR::File::type)
        .function("fileName", &libOpenCOR::File::fileName)
        .function("url", &libOpenCOR::File::url)
        .function("path", &libOpenCOR::File::path)
        .function("contents", emscripten::optional_override([](libOpenCOR::FilePtr &pThis) {
                      auto contents = pThis->contents();
                      auto view = emscripten::typed_memory_view(contents.size(), contents.data());
                      auto res = emscripten::val::global("Uint8Array").new_(contents.size());

                      res.call<void>("set", view);

                      return res;
                  }))
        .function("setContents", emscripten::optional_override([](libOpenCOR::FilePtr &pThis, uintptr_t pContents, size_t pSize) {
                      auto contents = reinterpret_cast<unsigned char *>(pContents);

                      pThis->setContents(libOpenCOR::UnsignedChars(contents, contents + pSize));
                  }))
        .function("hasChildFiles", &libOpenCOR::File::hasChildFiles)
        .function("childFileCount", &libOpenCOR::File::childFileCount)
        .function("childFileNames", &libOpenCOR::File::childFileNames)
        .function("childFiles", &libOpenCOR::File::childFiles)
        .function("childFile", &libOpenCOR::File::childFile)
        .function("childFileFromFileName", &libOpenCOR::File::childFileFromFileName);

    EM_ASM({
        Module["File"]["Type"] = Module["File.Type"];

        delete Module["File.Type"];
    });

    // FileManager API.

    emscripten::class_<libOpenCOR::FileManager>("FileManager")
        .class_function("instance", &libOpenCOR::FileManager::instance)
        .function("manage", &libOpenCOR::FileManager::manage)
        .function("unmanage", &libOpenCOR::FileManager::unmanage)
        .function("reset", &libOpenCOR::FileManager::reset)
        .function("hasFiles", &libOpenCOR::FileManager::hasFiles)
        .function("fileCount", &libOpenCOR::FileManager::fileCount)
        .function("files", &libOpenCOR::FileManager::files)
        .function("file", &libOpenCOR::FileManager::file)
        .function("fileFromFileNameOrUrl", &libOpenCOR::FileManager::fileFromFileNameOrUrl);
}
