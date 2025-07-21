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

#pragma once

#include "logger_p.h"

#include "compiler.h"

#ifndef __EMSCRIPTEN__
#    include "llvmbegin.h"
#    include "llvm/ExecutionEngine/Orc/LLJIT.h"
#    include "llvmend.h"
#endif

namespace libOpenCOR {

class Compiler::Impl: public Logger::Impl
{
public:
#ifdef __EMSCRIPTEN__
    bool mVfsInitialised = false;

    bool initialiseVfs(std::string &pErrorMessage);
    bool writeToVfs(std::string &pVirtualPath, const std::string &pCode, std::string &pErrorMessage);
    void cleanUpVfs(const std::string &pVirtualPath);

    bool compile(const std::string &pCode, UnsignedChars &pWasmModule);
#else
    std::unique_ptr<llvm::orc::LLJIT> mLljit;

    bool compile(const std::string &pCode);

    bool addFunction(const std::string &pName, void *pFunction);

    void *function(const std::string &pName) const;
#endif
};

} // namespace libOpenCOR
