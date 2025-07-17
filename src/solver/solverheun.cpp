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

#include "solverheun_p.h"

namespace libOpenCOR {

// Solver.

SolverHeun::Impl::Impl()
    : SolverOdeFixedStep::Impl("KISAO:0000301", "Heun")
{
}

SolverPtr SolverHeun::Impl::duplicate()
{
    return SolverOdeFixedStep::Impl::duplicate(SolverHeun::create());
}

bool SolverHeun::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                  double *pConstants, double *pComputedConstants, double *pAlgebraic,
                                  CellmlFileRuntime::ComputeRates pComputeRates)
{
    removeAllIssues();

    // Initialise the ODE solver itself.

    if (!SolverOdeFixedStep::Impl::initialise(pVoi, pSize, pStates, pRates,
                                              pConstants, pComputedConstants, pAlgebraic,
                                              pComputeRates)) {
        return false;
    }

    // Create our various arrays.

    mKDoubles.resize(pSize, NAN);
    mYkDoubles.resize(pSize, NAN);

    mK = mKDoubles.data();
    mYk = mYkDoubles.data();

    return true;
}

bool SolverHeun::Impl::solve(double &pVoi, double pVoiEnd)
{
    // We compute the following:
    //   k = f(t_n, Y_n)
    //   Y_n+1 = Y_n + h / 2 * ( f(t_n, Y_n) + f(t_n + h, Y_n + h * k) )

    static const auto HALF = 0.5;

    const auto voiStart = pVoi;
    size_t voiCounter = 0;
    auto realStep = mStep;
    auto realHalfStep = HALF * realStep;

    while (!fuzzyCompare(pVoi, pVoiEnd)) {
        // Check that the step is correct.

        if (pVoi + realStep > pVoiEnd) {
            realStep = pVoiEnd - pVoi;
            realHalfStep = HALF * realStep;
        }

        // Compute f(t_n, Y_n).

        computeRates(pVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraic);

        // Compute k and Y_n + h * k.

        for (size_t i = 0; i < mSize; ++i) {
            mK[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realStep * mK[i]; // NOLINT
        }

        // Compute f(t_n + h, Y_n + h * k).

        computeRates(pVoi + realStep, mYk, mRates, mConstants, mComputedConstants, mAlgebraic);

        // Compute Y_n+1.

        for (size_t i = 0; i < mSize; ++i) {
            mStates[i] += realHalfStep * (mK[i] + mRates[i]); // NOLINT
        }

        // Update the variable of integration.

        pVoi = fuzzyCompare(realStep, mStep) ?
                   voiStart + static_cast<double>(++voiCounter) * mStep :
                   pVoiEnd;
    }

    return true;
}

SolverHeun::SolverHeun()
    : SolverOdeFixedStep(new Impl {})
{
}

SolverHeun::~SolverHeun()
{
    delete pimpl();
}

SolverHeun::Impl *SolverHeun::pimpl()
{
    return static_cast<Impl *>(SolverOdeFixedStep::pimpl());
}

/*---GRY---
const SolverHeun::Impl *SolverHeun::pimpl() const
{
    return static_cast<const Impl *>(SolverOdeFixedStep::pimpl());
}
*/

SolverHeunPtr SolverHeun::create()
{
    return SolverHeunPtr {new SolverHeun {}};
}

} // namespace libOpenCOR
