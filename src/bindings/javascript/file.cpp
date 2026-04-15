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
        .property("type", &libOpenCOR::File::type)
        .property("fileName", &libOpenCOR::File::fileName)
        .property("url", &libOpenCOR::File::url)
        .property("path", &libOpenCOR::File::path)
        .function("contents", emscripten::optional_override([](const libOpenCOR::FilePtr &pThis) {
                      const auto &contents {pThis->contents()};
                      auto size = contents.size();

                      if (size == 0) {
                          return emscripten::val::global("Uint8Array").new_(0);
                      }

                      // Note: we avoid using emscripten::typed_memory_view() as an intermediate step since it creates a
                      //       JavaScript's TypedArray view directly backed by wasmMemory.buffer. This is fragile and
                      //       would break if ALLOW_MEMORY_GROWTH was to be enabled or if pthreads/SharedArrayBuffer
                      //       support were to be added. By performing the copy entirely within a single EM_ASM() block,
                      //       HEAPU8 is fetched and used atomically.

                      return emscripten::val::take_ownership(
                          static_cast<emscripten::EM_VAL>(EM_ASM_PTR({
                              var jsArray = new Uint8Array(HEAPU8.subarray($0, $0 + $1));

                              return Emval.toHandle(jsArray); }, contents.data(), size)));
                  }))
        .function("setContents", emscripten::optional_override([](const libOpenCOR::FilePtr &pThis, emscripten::val pContents) {
                      if (pContents.isNull() || pContents.isUndefined()) {
                          pThis->setContents(libOpenCOR::UnsignedChars {});

                          return;
                      }

                      // Note: avoid using emscripten::vecFromJSArray() since it internally uses typed_memory_view (see
                      //       the note in the contents() binding above).

                      auto length = pContents["length"].as<size_t>();
                      libOpenCOR::UnsignedChars contents(length);

                      if (length > 0) {
                          EM_ASM({ HEAPU8.set(Emval.toValue($0).subarray(0, $2), $1); }, pContents.as_handle(), contents.data(), length);
                      }

                      pThis->setContents(std::move(contents));
                  }))
        .property("hasChildFiles", &libOpenCOR::File::hasChildFiles)
        .property("childFileCount", &libOpenCOR::File::childFileCount)
        .property("childFileNames", &libOpenCOR::File::childFileNames)
        .property("childFiles", &libOpenCOR::File::childFiles)
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
        .property("hasFiles", &libOpenCOR::FileManager::hasFiles)
        .property("fileCount", &libOpenCOR::FileManager::fileCount)
        .property("files", &libOpenCOR::FileManager::files)
        .function("file", &libOpenCOR::FileManager::file)
        .function("fileFromFileNameOrUrl", &libOpenCOR::FileManager::fileFromFileNameOrUrl);
}
