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

#include "cellmlfile.h"

#include "libopencor/logger.h"

namespace libOpenCOR {

class CellmlFile;
using CellmlFilePtr = std::shared_ptr<CellmlFile>;

class CellmlFileRuntime;
using CellmlFileRuntimePtr = std::shared_ptr<CellmlFileRuntime>;

class CellmlFileRuntime: public Logger
{
public:
    using InitialiseVariablesForAlgebraicModelFunction = void (*)(double *pVariables);
    using InitialiseVariablesForDifferentialModelFunction = void (*)(double *pStates, double *pRates, double *pVariables);
    using ComputeComputedConstantsFunction = void (*)(double *pVariables);
    using ComputeRatesFunction = void (*)(double pVoi, double *pStates, double *pRates, double *pVariables);
    using ComputeVariablesForAlgebraicModelFunction = void (*)(double *pVariables);
    using ComputeVariablesForDifferentialModelFunction = void (*)(double pVoi, double *pStates, double *pRates, double *pVariables);

    CellmlFileRuntime() = delete;
    ~CellmlFileRuntime() override;

    CellmlFileRuntime(const CellmlFileRuntime &pOther) = delete;
    CellmlFileRuntime(CellmlFileRuntime &&pOther) noexcept = delete;

    CellmlFileRuntime &operator=(const CellmlFileRuntime &pRhs) = delete;
    CellmlFileRuntime &operator=(CellmlFileRuntime &&pRhs) noexcept = delete;

    static CellmlFileRuntimePtr create(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver);

    InitialiseVariablesForAlgebraicModelFunction initialiseVariablesForAlgebraicModel() const;
    InitialiseVariablesForDifferentialModelFunction initialiseVariablesForDifferentialModel() const;
    ComputeComputedConstantsFunction computeComputedConstants() const;
    ComputeRatesFunction computeRates() const;
    ComputeVariablesForAlgebraicModelFunction computeVariablesForAlgebraicModel() const;
    ComputeVariablesForDifferentialModelFunction computeVariablesForDifferentialModel() const;

private:
    class Impl;

    explicit CellmlFileRuntime(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver);

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR
