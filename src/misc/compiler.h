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

#include "unittestingexport.h"

#include "libopencor/logger.h"

#include <string>

namespace libOpenCOR {

class Compiler;
using CompilerPtr = std::shared_ptr<Compiler>;

class LIBOPENCOR_UNIT_TESTING_EXPORT Compiler: public Logger
{
public:
    ~Compiler() override;

    Compiler(const Compiler &pOther) = delete;
    Compiler(Compiler &&pOther) noexcept = delete;

    Compiler &operator=(const Compiler &pRhs) = delete;
    Compiler &operator=(Compiler &&pRhs) noexcept = delete;

    static CompilerPtr create();

#ifdef __EMSCRIPTEN__
    bool compile(const std::string &pCode, UnsignedChars &pWasmModule);
#else
    bool compile(const std::string &pCode);

    bool addFunction(const std::string &pName, void *pFunction);

    void *function(const std::string &pName) const;
#endif

private:
    class Impl;

    Impl *pimpl();
    const Impl *pimpl() const;

    explicit Compiler();
};

} // namespace libOpenCOR
