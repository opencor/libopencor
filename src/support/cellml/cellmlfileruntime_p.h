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
    CompilerPtr mCompiler = nullptr;
    std::string mNlaSolverAddress;
#ifdef __EMSCRIPTEN__
    UnsignedChars mWasmModule;
    intptr_t mWasmInstanceFunctionsId = 0;
#endif

#ifndef __EMSCRIPTEN__
    InitialiseVariablesForAlgebraicModel mInitialiseVariablesForAlgebraicModel = nullptr;
    InitialiseVariablesForDifferentialModel mInitialiseVariablesForDifferentialModel = nullptr;
    ComputeComputedConstantsForAlgebraicModel mComputeComputedConstantsForAlgebraicModel = nullptr;
    ComputeComputedConstantsForDifferentialModel mComputeComputedConstantsForDifferentialModel = nullptr;
    ComputeRates mComputeRates = nullptr;
    ComputeVariablesForAlgebraicModel mComputeVariablesForAlgebraicModel = nullptr;
    ComputeVariablesForDifferentialModel mComputeVariablesForDifferentialModel = nullptr;
#endif

    explicit Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver);

#ifdef __EMSCRIPTEN__
    void initialiseVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void initialiseVariablesForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeComputedConstantsForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeComputedConstantsForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeRates(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeVariablesForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
#else
    CellmlFileRuntime::InitialiseVariablesForAlgebraicModel initialiseVariablesForAlgebraicModel() const;
    CellmlFileRuntime::InitialiseVariablesForDifferentialModel initialiseVariablesForDifferentialModel() const;
    CellmlFileRuntime::ComputeComputedConstantsForAlgebraicModel computeComputedConstantsForAlgebraicModel() const;
    CellmlFileRuntime::ComputeComputedConstantsForDifferentialModel computeComputedConstantsForDifferentialModel() const;
    CellmlFileRuntime::ComputeRates computeRates() const;
    CellmlFileRuntime::ComputeVariablesForAlgebraicModel computeVariablesForAlgebraicModel() const;
    CellmlFileRuntime::ComputeVariablesForDifferentialModel computeVariablesForDifferentialModel() const;
#endif
};

} // namespace libOpenCOR
