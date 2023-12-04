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

#include "compiler.h"
#include "logger_p.h"

#include "cellmlfile.h"
#include "cellmlfileruntime.h"

namespace libOpenCOR {

class CellmlFileRuntime::Impl: public Logger::Impl
{
public:
    CompilerPtr mCompiler = nullptr;
    char *mNlaSolverAddress = nullptr;

    InitialiseVariablesForDifferentialModelFunction mInitialiseVariablesForDifferentialModel;
    InitialiseVariablesForAlgebraicModelFunction mInitialiseVariablesForAlgebraicModel = nullptr;
    ComputeComputedConstantsFunction mComputeComputedConstants = nullptr;
    ComputeRatesFunction mComputeRates = nullptr;
    ComputeVariablesForDifferentialModelFunction mComputeVariablesForDifferentialModel = nullptr;
    ComputeVariablesForAlgebraicModelFunction mComputeVariablesForAlgebraicModel = nullptr;

    explicit Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver);
    ~Impl();
};

} // namespace libOpenCOR
