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

#include "cellmlfileruntime.h"

#include "libopencor/solverode.h"

namespace libOpenCOR {

class SolverOde::Impl: public Solver::Impl
{
public:
    size_t mSize {0};

    double *mStates {nullptr};
    double *mRates {nullptr};
    double *mConstants {nullptr};
    double *mComputedConstants {nullptr};
    double *mAlgebraic {nullptr};

    CellmlFileRuntimePtr mRuntime;

    CellmlFileRuntime::ComputeRates mComputeRates {nullptr};

    explicit Impl(const std::string &pId, const std::string &pName);

    virtual bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                            double *pConstants, double *pComputedConstants, double *pAlgebraicVariables,
                            const CellmlFileRuntimePtr &pRuntime) = 0;
    virtual bool reinitialise(double pVoi);

    // Solve our model from pVoi to pVoiEnd.
    // Note: our rates are up to date when we get called (i.e. they are those at pVoi), but they may not be when we
    //       return (see SedInstanceTask::Impl::run()).

    virtual bool solve(double &pVoi, double pVoiEnd) = 0;

    void computeRates(double pVoi, double *pStates, double *pRates,
                      double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const;
};

} // namespace libOpenCOR
