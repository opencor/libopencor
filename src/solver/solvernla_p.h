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

#include "solver_p.h"
#include "solvernla.h"

namespace libOpenCOR {

class SolverNla::Impl: public Solver::Impl
{
public:
    size_t mSize = 0;

    double *mStates = nullptr;
    double *mRates = nullptr;
    double *mVariables = nullptr;

    ComputeRates mComputeRates = nullptr;

    virtual bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                            ComputeRates pComputeRates) = 0;

    virtual bool solve(double &pVoi, double pVoiEnd) const = 0;
};

} // namespace libOpenCOR
