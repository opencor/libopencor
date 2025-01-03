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

#include "solverfourthorderrungekutta_p.h"

namespace libOpenCOR {

// Solver.

SolverFourthOrderRungeKutta::Impl::Impl()
    : SolverOdeFixedStep::Impl("KISAO:0000032", "Fourth-order Runge-Kutta")
{
}

SolverPtr SolverFourthOrderRungeKutta::Impl::duplicate()
{
    return SolverOdeFixedStep::Impl::duplicate(SolverFourthOrderRungeKutta::create());
}

bool SolverFourthOrderRungeKutta::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                                   double *pConstants, double *pComputedConstants, double *pAlgebraic,
                                                   CellmlFileRuntime::ComputeCompiledRates pComputeCompiledRates,
                                                   CellmlFileRuntime::ComputeInterpretedRates pComputeInterpretedRates)
{
    removeAllIssues();

    // Initialise the ODE solver itself.

    if (!SolverOdeFixedStep::Impl::initialise(pVoi, pSize, pStates, pRates,
                                              pConstants, pComputedConstants, pAlgebraic,
                                              pComputeCompiledRates, pComputeInterpretedRates)) {
        return false;
    }

    // Create our various arrays.

    mK1Doubles.resize(pSize, NAN);
    mK2Doubles.resize(pSize, NAN);
    mK3Doubles.resize(pSize, NAN);
    mYkDoubles.resize(pSize, NAN);

    mK1 = mK1Doubles.data();
    mK2 = mK2Doubles.data();
    mK3 = mK3Doubles.data();
    mYk = mYkDoubles.data();

    return true;
}

bool SolverFourthOrderRungeKutta::Impl::solve(double &pVoi, double pVoiEnd)
{
    // We compute the following:
    //   k1 = f(t_n, Y_n)
    //   k2 = f(t_n + h / 2, Y_n + h / 2 * k1)
    //   k3 = f(t_n + h / 2, Y_n + h / 2 * k2)
    //   k4 = f(t_n + h, Y_n + h * k3)
    //   Y_n+1 = Y_n + h / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
    // Note that k4 doesn't need to be tracked since it is used only once.

    static const auto TWO = 2.0;
    static const auto HALF = 0.5;
    static const auto ONE_SIXTH = 1.0 / 6.0;

    const auto voiStart = pVoi;
    size_t voiCounter = 0;
    auto realStep = mStep;
    auto realHalfStep = HALF * realStep;
    auto realOneSixthStep = ONE_SIXTH * realStep;

    while (!fuzzyCompare(pVoi, pVoiEnd)) {
        // Check that the step is correct.

        if (pVoi + realStep > pVoiEnd) {
            realStep = pVoiEnd - pVoi;
            realHalfStep = HALF * realStep;
            realOneSixthStep = ONE_SIXTH * realStep;
        }

        // Compute f(t_n, Y_n).

        computeRates(pVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraic);

        // Compute k1 and Y_n + h / 2 * k1.

        for (size_t i = 0; i < mSize; ++i) {
            mK1[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realHalfStep * mK1[i]; // NOLINT
        }

        // Compute f(t_n + h / 2, Y_n + h / 2 * k1).

        computeRates(pVoi + realHalfStep, mYk, mRates, mConstants, mComputedConstants, mAlgebraic);

        // Compute k2 and Y_n + h / 2 * k2.

        for (size_t i = 0; i < mSize; ++i) {
            mK2[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realHalfStep * mK2[i]; // NOLINT
        }

        // Compute f(t_n + h / 2, Y_n + h / 2 * k2).

        computeRates(pVoi + realHalfStep, mYk, mRates, mConstants, mComputedConstants, mAlgebraic);

        // Compute k3 and Y_n + h * k3.

        for (size_t i = 0; i < mSize; ++i) {
            mK3[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realStep * mK3[i]; // NOLINT
        }

        // Compute f(t_n + h, Y_n + h * k3).

        computeRates(pVoi + realStep, mYk, mRates, mConstants, mComputedConstants, mAlgebraic);

        // Compute Y_n+1.

        for (size_t i = 0; i < mSize; ++i) {
            mStates[i] += realOneSixthStep * (mK1[i] + TWO * mK2[i] + TWO * mK3[i] + mRates[i]); // NOLINT
        }

        // Update the variable of integration.

        pVoi = fuzzyCompare(realStep, mStep) ?
                   voiStart + static_cast<double>(++voiCounter) * mStep :
                   pVoiEnd;
    }

    return true;
}

SolverFourthOrderRungeKutta::SolverFourthOrderRungeKutta()
    : SolverOdeFixedStep(new Impl {})
{
}

SolverFourthOrderRungeKutta::~SolverFourthOrderRungeKutta()
{
    delete pimpl();
}

SolverFourthOrderRungeKutta::Impl *SolverFourthOrderRungeKutta::pimpl()
{
    return static_cast<Impl *>(SolverOdeFixedStep::pimpl());
}

/*---GRY---
const SolverFourthOrderRungeKutta::Impl *SolverFourthOrderRungeKutta::pimpl() const
{
    return static_cast<const Impl *>(SolverOdeFixedStep::pimpl());
}
*/

SolverFourthOrderRungeKuttaPtr SolverFourthOrderRungeKutta::create()
{
    return SolverFourthOrderRungeKuttaPtr {new SolverFourthOrderRungeKutta {}};
}

} // namespace libOpenCOR
