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
#include <libcellml>

namespace libOpenCOR {

class CellmlFile;
using CellmlFilePtr = std::shared_ptr<CellmlFile>;

class CellmlFileRuntime;
using CellmlFileRuntimePtr = std::shared_ptr<CellmlFileRuntime>;

class CellmlFileRuntime: public Logger
{
public:
    using InitialiseCompiledVariablesForAlgebraicModel = void (*)(double *pVariables);
    using InitialiseCompiledVariablesForDifferentialModel = void (*)(double *pStates, double *pRates, double *pVariables);
    using ComputeCompiledComputedConstants = void (*)(double *pVariables);
    using ComputeCompiledRates = void (*)(double pVoi, double *pStates, double *pRates, double *pVariables);
    using ComputeCompiledVariablesForAlgebraicModel = void (*)(double *pVariables);
    using ComputeCompiledVariablesForDifferentialModel = void (*)(double pVoi, double *pStates, double *pRates, double *pVariables);

    using InitialiseInterpretedVariablesForAlgebraicModel = std::function<void(double *pVariables)>;
    using InitialiseInterpretedVariablesForDifferentialModel = std::function<void(double *pStates, double *pRates, double *pVariables)>;
    using ComputeInterpretedComputedConstants = std::function<void(double *pVariables)>;
    using ComputeInterpretedRates = std::function<void(double pVoi, double *pStates, double *pRates, double *pVariables)>;
    using ComputeInterpretedVariablesForAlgebraicModel = std::function<void(double *pVariables)>;
    using ComputeInterpretedVariablesForDifferentialModel = std::function<void(double pVoi, double *pStates, double *pRates, double *pVariables)>;

    CellmlFileRuntime() = delete;
    ~CellmlFileRuntime() override;

    CellmlFileRuntime(const CellmlFileRuntime &pOther) = delete;
    CellmlFileRuntime(CellmlFileRuntime &&pOther) noexcept = delete;

    CellmlFileRuntime &operator=(const CellmlFileRuntime &pRhs) = delete;
    CellmlFileRuntime &operator=(CellmlFileRuntime &&pRhs) noexcept = delete;

    static CellmlFileRuntimePtr create(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver,
                                       bool pCompiled);

    InitialiseCompiledVariablesForAlgebraicModel initialiseCompiledVariablesForAlgebraicModel() const;
    InitialiseCompiledVariablesForDifferentialModel initialiseCompiledVariablesForDifferentialModel() const;
    ComputeCompiledComputedConstants computeCompiledComputedConstants() const;
    ComputeCompiledRates computeCompiledRates() const;
    ComputeCompiledVariablesForAlgebraicModel computeCompiledVariablesForAlgebraicModel() const;
    ComputeCompiledVariablesForDifferentialModel computeCompiledVariablesForDifferentialModel() const;

    InitialiseInterpretedVariablesForAlgebraicModel initialiseInterpretedVariablesForAlgebraicModel() const;
    InitialiseInterpretedVariablesForDifferentialModel initialiseInterpretedVariablesForDifferentialModel() const;
    ComputeInterpretedComputedConstants computeInterpretedComputedConstants() const;
    ComputeInterpretedRates computeInterpretedRates() const;
    ComputeInterpretedVariablesForAlgebraicModel computeInterpretedVariablesForAlgebraicModel() const;
    ComputeInterpretedVariablesForDifferentialModel computeInterpretedVariablesForDifferentialModel() const;

private:
    class Impl;

    explicit CellmlFileRuntime(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver, bool pCompiled);

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR
