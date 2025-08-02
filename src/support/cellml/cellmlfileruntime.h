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

#include "libopencor/logger.h"

#include <functional>

namespace libOpenCOR {

class CellmlFile;
using CellmlFilePtr = std::shared_ptr<CellmlFile>;

class CellmlFileRuntime;
using CellmlFileRuntimePtr = std::shared_ptr<CellmlFileRuntime>;

class CellmlFileRuntime: public Logger
{
public:
#ifndef __EMSCRIPTEN__
    using InitialiseVariablesForAlgebraicModel = void (*)(double *pConstants, double *pComputedConstants, double *pAlgebraic);
    using InitialiseVariablesForDifferentialModel = void (*)(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic);
    using ComputeComputedConstants = void (*)(double *pConstants, double *pComputedConstants);
    using ComputeRates = void (*)(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic);
    using ComputeVariablesForAlgebraicModel = void (*)(double *pConstants, double *pComputedConstants, double *pAlgebraic);
    using ComputeVariablesForDifferentialModel = void (*)(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic);
#endif

    CellmlFileRuntime() = delete;
    ~CellmlFileRuntime() override;

    CellmlFileRuntime(const CellmlFileRuntime &pOther) = delete;
    CellmlFileRuntime(CellmlFileRuntime &&pOther) noexcept = delete;

    CellmlFileRuntime &operator=(const CellmlFileRuntime &pRhs) = delete;
    CellmlFileRuntime &operator=(CellmlFileRuntime &&pRhs) noexcept = delete;

    static CellmlFileRuntimePtr create(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver);

#ifdef __EMSCRIPTEN__
    void initialiseVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void initialiseVariablesForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeComputedConstants(double *pConstants, double *pComputedConstants) const;
    void computeRates(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
    void computeVariablesForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const;
#else
    InitialiseVariablesForAlgebraicModel initialiseVariablesForAlgebraicModel() const;
    InitialiseVariablesForDifferentialModel initialiseVariablesForDifferentialModel() const;
    ComputeComputedConstants computeComputedConstants() const;
    ComputeRates computeRates() const;
    ComputeVariablesForAlgebraicModel computeVariablesForAlgebraicModel() const;
    ComputeVariablesForDifferentialModel computeVariablesForDifferentialModel() const;
#endif

private:
    class Impl;

    explicit CellmlFileRuntime(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver);

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR
