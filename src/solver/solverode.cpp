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

#include "solverode_p.h"

namespace libOpenCOR {

SolverOde::Impl::Impl(const std::string &pId, const std::string &pName)
    : Solver::Impl(pId, pName)
{
}

bool SolverOde::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                                 ComputeRates pComputeRates)
{
    (void)pVoi;

    mSize = pSize;

    mStates = pStates;
    mRates = pRates;
    mVariables = pVariables;

    mComputeRates = pComputeRates;

    return true;
}

bool SolverOde::Impl::reinitialise(double pVoi)
{
    (void)pVoi;

    return true;
}

SolverOde::SolverOde(Impl *pPimpl)
    : Solver(pPimpl)
{
}

SolverOde::Impl *SolverOde::pimpl()
{
    return static_cast<Impl *>(Solver::pimpl());
}

const SolverOde::Impl *SolverOde::pimpl() const
{
    return static_cast<const Impl *>(Solver::pimpl());
}

Solver::Type SolverOde::type() const
{
    return Type::ODE;
}

bool SolverOde::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                           ComputeRates pComputeRates)
{
    return pimpl()->initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverOde::reinitialise(double pVoi)
{
    return pimpl()->reinitialise(pVoi);
}

bool SolverOde::solve(double &pVoi, double pVoiEnd)
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
