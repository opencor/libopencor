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
#include "cellmlfileruntime.h"

namespace libOpenCOR {

class CellmlFileRuntime::Impl: public Logger::Impl
{
public:
    CompilerPtr mCompiler {nullptr};
#ifdef __EMSCRIPTEN__
    // A unique identifier for our WebAssembly code, so that each JavaScript worker can cache its compiled version (see
    // initialiseWorkerWasmJS()).
    // Note: we don't use the address of our WebAssembly code since, once we are deleted, another runtime may reuse it.

    int mWasmModuleId {0};

    // The size, in bytes, of the stack of our WebAssembly instances (see initialiseWorkerWasm()), i.e. the size that our
    // compiler tells us our WebAssembly code needs plus a safety margin.

    static constexpr size_t WASM_STACK_MARGIN_SIZE {65536};

    UnsignedChars mWasmModule;
    size_t mWasmStackSize {WASM_STACK_MARGIN_SIZE};
#endif

    InitialiseArraysForAlgebraicModel mInitialiseArraysForAlgebraicModel {nullptr};
    InitialiseArraysForDifferentialModel mInitialiseArraysForDifferentialModel {nullptr};
    ComputeComputedConstantsForAlgebraicModel mComputeComputedConstantsForAlgebraicModel {nullptr};
    ComputeComputedConstantsForDifferentialModel mComputeComputedConstantsForDifferentialModel {nullptr};
    ComputeRates mComputeRates {nullptr};
    ComputeVariablesForAlgebraicModel mComputeVariablesForAlgebraicModel {nullptr};
    ComputeVariablesForDifferentialModel mComputeVariablesForDifferentialModel {nullptr};

    explicit Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver);
#ifdef __EMSCRIPTEN__
    ~Impl() override;

    void initialiseWorkerWasm() const;
#endif

    CellmlFileRuntime::InitialiseArraysForAlgebraicModel initialiseArraysForAlgebraicModel() const;
    CellmlFileRuntime::InitialiseArraysForDifferentialModel initialiseArraysForDifferentialModel() const;
    CellmlFileRuntime::ComputeComputedConstantsForAlgebraicModel computeComputedConstantsForAlgebraicModel() const;
    CellmlFileRuntime::ComputeComputedConstantsForDifferentialModel computeComputedConstantsForDifferentialModel() const;
    CellmlFileRuntime::ComputeRates computeRates() const;
    CellmlFileRuntime::ComputeVariablesForAlgebraicModel computeVariablesForAlgebraicModel() const;
    CellmlFileRuntime::ComputeVariablesForDifferentialModel computeVariablesForDifferentialModel() const;
};

} // namespace libOpenCOR
