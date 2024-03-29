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

#include "solverforwardeuler_p.h"

namespace libOpenCOR {

// Solver.

bool SolverForwardEuler::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                          double *pVariables, ComputeRates pComputeRates)
{
    removeAllIssues();

    // Initialise the ODE solver itself.

    return SolverOdeFixedStep::Impl::initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverForwardEuler::Impl::solve(double &pVoi, double pVoiEnd) const
{
    // We compute the following:
    //   Y_n+1 = Y_n + h * f(t_n, Y_n)

    const auto voiStart = pVoi;
    size_t voiCounter = 0;
    auto realStep = mStep;

    while (!fuzzyCompare(pVoi, pVoiEnd)) {
        // Check that the step is correct.

        if (pVoi + realStep > pVoiEnd) {
            realStep = pVoiEnd - pVoi;
        }

        // Compute f(t_n, Y_n).

        mComputeRates(pVoi, mStates, mRates, mVariables);

        // Compute Y_n+1.

        for (size_t i = 0; i < mSize; ++i) {
            mStates[i] += realStep * mRates[i]; // NOLINT
        }

        // Update the variable of integration.

        pVoi = fuzzyCompare(realStep, mStep) ?
                   voiStart + static_cast<double>(++voiCounter) * mStep :
                   pVoiEnd;
    }

    return true;
}

SolverForwardEuler::SolverForwardEuler()
    : SolverOdeFixedStep(new Impl {})
{
}

SolverForwardEuler::~SolverForwardEuler()
{
    delete pimpl();
}

SolverForwardEuler::Impl *SolverForwardEuler::pimpl()
{
    return static_cast<Impl *>(SolverOdeFixedStep::pimpl());
}

const SolverForwardEuler::Impl *SolverForwardEuler::pimpl() const
{
    return static_cast<const Impl *>(SolverOdeFixedStep::pimpl());
}

SolverForwardEulerPtr SolverForwardEuler::create()
{
    return SolverForwardEulerPtr {new SolverForwardEuler {}};
}

std::string SolverForwardEuler::id() const
{
    return "KISAO:0000030";
}

std::string SolverForwardEuler::name() const
{
    return "Forward Euler";
}

bool SolverForwardEuler::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                                    ComputeRates pComputeRates)
{
    return pimpl()->initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverForwardEuler::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
