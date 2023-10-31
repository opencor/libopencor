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
#include "utils.h"

namespace libOpenCOR {

// Properties information.

const std::string SolverFourthOrderRungeKutta::Impl::ID = "KISAO:0000032"; // NOLINT
const std::string SolverFourthOrderRungeKutta::Impl::NAME = "Fourth-order Runge-Kutta"; // NOLINT

const std::string SolverFourthOrderRungeKutta::Impl::STEP_ID = "KISAO:0000483"; // NOLINT
const std::string SolverFourthOrderRungeKutta::Impl::STEP_NAME = "Step"; // NOLINT

// Solver.

SolverPtr SolverFourthOrderRungeKutta::Impl::create()
{
    return std::shared_ptr<SolverFourthOrderRungeKutta> {new SolverFourthOrderRungeKutta {}};
}

SolverPropertyPtrVector SolverFourthOrderRungeKutta::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, STEP_ID, STEP_NAME,
                                     {},
                                     toString(STEP_DEFAULT_VALUE),
                                     true),
    };
}

StringVector SolverFourthOrderRungeKutta::Impl::hiddenProperties(const StringStringMap &pProperties)
{
    (void)pProperties;

    return {};
}

SolverFourthOrderRungeKutta::Impl::Impl()
    : SolverOde::Impl()
{
    mProperties[STEP_ID] = toString(STEP_DEFAULT_VALUE);
}

SolverFourthOrderRungeKutta::Impl::~Impl()
{
    delete[] mK1;
    delete[] mK2;
    delete[] mK3;
    delete[] mYk;
}

StringStringMap SolverFourthOrderRungeKutta::Impl::propertiesId() const
{
    static const StringStringMap PROPERTIES_ID = {
        {STEP_NAME, STEP_ID},
    };

    return PROPERTIES_ID;
}

bool SolverFourthOrderRungeKutta::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                                   double *pVariables, ComputeRates pComputeRates)
{
    removeAllIssues();

    // Retrieve the solver's properties.

    bool ok = true;

    mStep = toDouble(mProperties[STEP_ID], ok);

    if (!ok || (mStep <= 0.0)) {
        addError(R"(The "Step" property has an invalid value (")" + mProperties[STEP_ID] + R"("). It must be a floating point number greater than zero.)");

        return false;
    }

    // Create our various arrays.

    mK1 = new double[pSize] {};
    mK2 = new double[pSize] {};
    mK3 = new double[pSize] {};
    mYk = new double[pSize] {};

    // Initialise the ODE solver itself.

    return SolverOde::Impl::initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverFourthOrderRungeKutta::Impl::solve(double &pVoi, double pVoiEnd) const
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

    while (!libOpenCOR::fuzzyCompare(pVoi, pVoiEnd)) {
        // Check that the step is correct.

        if (pVoi + realStep > pVoiEnd) {
            realStep = pVoiEnd - pVoi;
            realHalfStep = HALF * realStep;
            realOneSixthStep = ONE_SIXTH * realStep;
        }

        // Compute f(t_n, Y_n).

        mComputeRates(pVoi, mStates, mRates, mVariables);

        // Compute k1 and Y_n + h / 2 * k1.

        for (size_t i = 0; i < mSize; ++i) {
            mK1[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realHalfStep * mK1[i]; // NOLINT
        }

        // Compute f(t_n + h / 2, Y_n + h / 2 * k1).

        mComputeRates(pVoi + realHalfStep, mYk, mRates, mVariables);

        // Compute k2 and Y_n + h / 2 * k2.

        for (size_t i = 0; i < mSize; ++i) {
            mK2[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realHalfStep * mK2[i]; // NOLINT
        }

        // Compute f(t_n + h / 2, Y_n + h / 2 * k2).

        mComputeRates(pVoi + realHalfStep, mYk, mRates, mVariables);

        // Compute k3 and Y_n + h * k3.

        for (size_t i = 0; i < mSize; ++i) {
            mK3[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realStep * mK3[i]; // NOLINT
        }

        // Compute f(t_n + h, Y_n + h * k3).

        mComputeRates(pVoi + realStep, mYk, mRates, mVariables);

        // Compute Y_n+1.

        for (size_t i = 0; i < mSize; ++i) {
            mStates[i] += realOneSixthStep * (mK1[i] + TWO * mK2[i] + TWO * mK3[i] + mRates[i]); // NOLINT
        }

        // Update the variable of integration.

        pVoi = libOpenCOR::fuzzyCompare(realStep, mStep) ?
                   voiStart + static_cast<double>(++voiCounter) * mStep :
                   pVoiEnd;
    }

    return true;
}

SolverFourthOrderRungeKutta::SolverFourthOrderRungeKutta()
    : SolverOde(new Impl())
{
}

SolverFourthOrderRungeKutta::~SolverFourthOrderRungeKutta()
{
    delete pimpl();
}

SolverFourthOrderRungeKutta::Impl *SolverFourthOrderRungeKutta::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverFourthOrderRungeKutta::Impl *SolverFourthOrderRungeKutta::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

SolverInfoPtr SolverFourthOrderRungeKutta::info() const
{
    return Solver::solversInfo()[Solver::Impl::SolversIndex[SolverFourthOrderRungeKutta::Impl::ID]];
}

Solver::Type SolverFourthOrderRungeKutta::type() const
{
    return Type::ODE;
}

std::string SolverFourthOrderRungeKutta::id() const
{
    return Impl::ID;
}

std::string SolverFourthOrderRungeKutta::name() const
{
    return Impl::NAME;
}

bool SolverFourthOrderRungeKutta::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                             double *pVariables, ComputeRates pComputeRates)
{
    return pimpl()->initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverFourthOrderRungeKutta::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
