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

#include "solversecondorderrungekutta_p.h"

namespace libOpenCOR {

// Solver.

SolverSecondOrderRungeKutta::Impl::Impl()
    : SolverOdeFixedStep::Impl("KISAO:0000381", "Second-order Runge-Kutta")
{
}

SolverPtr SolverSecondOrderRungeKutta::Impl::duplicate()
{
    return SolverOdeFixedStep::Impl::duplicate(SolverSecondOrderRungeKutta::create());
}

bool SolverSecondOrderRungeKutta::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                                   double *pConstants, double *pComputedConstants, double *pAlgebraic,
                                                   const CellmlFileRuntimePtr &pRuntime)
{
    removeAllIssues();

    // Initialise the ODE solver itself.

    if (!SolverOdeFixedStep::Impl::initialise(pVoi, pSize, pStates, pRates,
                                              pConstants, pComputedConstants, pAlgebraic,
                                              pRuntime)) {
        return false;
    }

    // Create our various arrays.

    mYkDoubles.resize(pSize, NAN);

    mYk = mYkDoubles.data();

    return true;
}

bool SolverSecondOrderRungeKutta::Impl::solve(double &pVoi, double pVoiEnd)
{
    // We compute the following:
    //   k1 = f(t_n, Y_n)
    //   k2 = f(t_n + h / 2, Y_n + h / 2 * k1)
    //   Y_n+1 = Y_n + h * k2
    // Note that k1 and k2 don't need to be tracked since they are used only once.

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

        // Compute Y_n + h / 2 * k1.

        for (size_t i = 0; i < mSize; ++i) {
            mYk[i] = mStates[i] + realHalfStep * mRates[i]; // NOLINT
        }

        // Compute f(t_n + h / 2, Y_n + h / 2 * k1).

        computeRates(pVoi + realHalfStep, mYk, mRates, mConstants, mComputedConstants, mAlgebraic);

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

SolverSecondOrderRungeKutta::SolverSecondOrderRungeKutta()
    : SolverOdeFixedStep(new Impl {})
{
}

SolverSecondOrderRungeKutta::~SolverSecondOrderRungeKutta()
{
    delete pimpl();
}

SolverSecondOrderRungeKutta::Impl *SolverSecondOrderRungeKutta::pimpl()
{
    return static_cast<Impl *>(SolverOdeFixedStep::pimpl());
}

/*---GRY---
const SolverSecondOrderRungeKutta::Impl *SolverSecondOrderRungeKutta::pimpl() const
{
    return static_cast<const Impl *>(SolverOdeFixedStep::pimpl());
}
*/

SolverSecondOrderRungeKuttaPtr SolverSecondOrderRungeKutta::create()
{
    return SolverSecondOrderRungeKuttaPtr {new SolverSecondOrderRungeKutta {}};
}

} // namespace libOpenCOR
