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

#include "solverodeunknown_p.h"

namespace libOpenCOR {

SolverOdePtr SolverOdeUnknown::Impl::create()
{
#ifdef COVERAGE_ENABLED
    auto res = std::shared_ptr<SolverOdeUnknown> {new SolverOdeUnknown {}};

    res->pimpl()->propertiesKisaoId(); // Just for code coverage.

    return res;
#else
    return std::shared_ptr<SolverOdeUnknown> {new SolverOdeUnknown {}};
#endif
}

std::map<std::string, std::string> SolverOdeUnknown::Impl::propertiesKisaoId() const
{
    return {};
}

bool SolverOdeUnknown::Impl::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                        ComputeRates pComputeRates)
{
    (void)pSize;
    (void)pStates;
    (void)pRates;
    (void)pVariables;
    (void)pComputeRates;

    return false;
}

void SolverOdeUnknown::Impl::solve(double &pVoi, double pVoiEnd) const
{
    (void)pVoi;
    (void)pVoiEnd;
}

SolverOdeUnknown::SolverOdeUnknown()
    : SolverOde(new Impl())
{
}

SolverOdeUnknown::~SolverOdeUnknown()
{
    delete pimpl();
}

SolverOdeUnknown::Impl *SolverOdeUnknown::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverOdeUnknown::Impl *SolverOdeUnknown::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

bool SolverOdeUnknown::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                  ComputeRates pComputeRates)
{
    return pimpl()->initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

void SolverOdeUnknown::solve(double &pVoi, double pVoiEnd) const
{
    pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
