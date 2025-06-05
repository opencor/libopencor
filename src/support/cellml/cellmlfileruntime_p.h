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
#ifndef __EMSCRIPTEN__
    CompilerPtr mCompiler = nullptr;
#endif

    char *mNlaSolverAddress = nullptr;

    InitialiseCompiledVariablesForAlgebraicModel mInitialiseCompiledVariablesForAlgebraicModel = nullptr;
    InitialiseCompiledVariablesForDifferentialModel mInitialiseCompiledVariablesForDifferentialModel = nullptr;
    ComputeCompiledComputedConstants mComputeCompiledComputedConstants = nullptr;
    ComputeCompiledRates mComputeCompiledRates = nullptr;
    ComputeCompiledVariablesForAlgebraicModel mComputeCompiledVariablesForAlgebraicModel = nullptr;
    ComputeCompiledVariablesForDifferentialModel mComputeCompiledVariablesForDifferentialModel = nullptr;

    InitialiseInterpretedVariablesForAlgebraicModel mInitialiseInterpretedVariablesForAlgebraicModel = nullptr;
    InitialiseInterpretedVariablesForDifferentialModel mInitialiseInterpretedVariablesForDifferentialModel = nullptr;
    ComputeInterpretedComputedConstants mComputeInterpretedComputedConstants = nullptr;
    ComputeInterpretedRates mComputeInterpretedRates = nullptr;
    ComputeInterpretedVariablesForAlgebraicModel mComputeInterpretedVariablesForAlgebraicModel = nullptr;
    ComputeInterpretedVariablesForDifferentialModel mComputeInterpretedVariablesForDifferentialModel = nullptr;

    explicit Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver, bool pCompiled);
    ~Impl() override;

    CellmlFileRuntime::InitialiseCompiledVariablesForAlgebraicModel initialiseCompiledVariablesForAlgebraicModel() const;
    CellmlFileRuntime::InitialiseCompiledVariablesForDifferentialModel initialiseCompiledVariablesForDifferentialModel() const;
    CellmlFileRuntime::ComputeCompiledComputedConstants computeCompiledComputedConstants() const;
    CellmlFileRuntime::ComputeCompiledRates computeCompiledRates() const;
    CellmlFileRuntime::ComputeCompiledVariablesForAlgebraicModel computeCompiledVariablesForAlgebraicModel() const;
    CellmlFileRuntime::ComputeCompiledVariablesForDifferentialModel computeCompiledVariablesForDifferentialModel() const;

    CellmlFileRuntime::InitialiseInterpretedVariablesForAlgebraicModel initialiseInterpretedVariablesForAlgebraicModel() const;
    CellmlFileRuntime::InitialiseInterpretedVariablesForDifferentialModel initialiseInterpretedVariablesForDifferentialModel() const;
    CellmlFileRuntime::ComputeInterpretedComputedConstants computeInterpretedComputedConstants() const;
    CellmlFileRuntime::ComputeInterpretedRates computeInterpretedRates() const;
    CellmlFileRuntime::ComputeInterpretedVariablesForAlgebraicModel computeInterpretedVariablesForAlgebraicModel() const;
    CellmlFileRuntime::ComputeInterpretedVariablesForDifferentialModel computeInterpretedVariablesForDifferentialModel() const;
};

} // namespace libOpenCOR
