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

SolverForwardEuler::Impl::Impl()
    : SolverOdeFixedStep::Impl("KISAO:0000030", "Forward Euler")
{
}

SolverPtr SolverForwardEuler::Impl::duplicate()
{
    return SolverOdeFixedStep::Impl::duplicate(SolverForwardEuler::create());
}

bool SolverForwardEuler::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                          double *pConstants, double *pComputedConstants, double *pAlgebraic,
                                          CellmlFileRuntime::ComputeRates pComputeRates)
{
    removeAllIssues();

    // Initialise the ODE solver itself.

    return SolverOdeFixedStep::Impl::initialise(pVoi, pSize, pStates, pRates,
                                                pConstants, pComputedConstants, pAlgebraic,
                                                pComputeRates);
}

bool SolverForwardEuler::Impl::solve(double &pVoi, double pVoiEnd)
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

        computeRates(pVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraic);

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

/*---GRY---
const SolverForwardEuler::Impl *SolverForwardEuler::pimpl() const
{
    return static_cast<const Impl *>(SolverOdeFixedStep::pimpl());
}
*/

SolverForwardEulerPtr SolverForwardEuler::create()
{
    return SolverForwardEulerPtr {new SolverForwardEuler {}};
}

} // namespace libOpenCOR
