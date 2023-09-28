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
#include "utils.h"

namespace libOpenCOR {

// Properties information.

const std::string SolverSecondOrderRungeKutta::Impl::ID = "KISAO:0000381"; // NOLINT
const std::string SolverSecondOrderRungeKutta::Impl::NAME = "Second-order Runge-Kutta"; // NOLINT

const std::string SolverSecondOrderRungeKutta::Impl::STEP_ID = "KISAO:0000483"; // NOLINT
const std::string SolverSecondOrderRungeKutta::Impl::STEP_NAME = "Step"; // NOLINT

// Solver.

SolverPtr SolverSecondOrderRungeKutta::Impl::create()
{
    return std::shared_ptr<SolverSecondOrderRungeKutta> {new SolverSecondOrderRungeKutta {}};
}

std::vector<SolverPropertyPtr> SolverSecondOrderRungeKutta::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, STEP_ID, STEP_NAME,
                                     {},
                                     toString(STEP_DEFAULT_VALUE),
                                     true),
    };
}

SolverSecondOrderRungeKutta::Impl::Impl()
    : SolverOde::Impl()
{
    mIsValid = true;

    mProperties[STEP_ID] = toString(STEP_DEFAULT_VALUE);
}

SolverSecondOrderRungeKutta::Impl::~Impl()
{
    delete[] mYk;
}

std::map<std::string, std::string> SolverSecondOrderRungeKutta::Impl::propertiesId() const
{
    static const std::map<std::string, std::string> PROPERTIES_ID = {
        {STEP_NAME, STEP_ID},
    };

    return PROPERTIES_ID;
}

bool SolverSecondOrderRungeKutta::Impl::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                                   ComputeRates pComputeRates)
{
    removeAllIssues();

    // Retrieve the solver's properties.

    bool ok = true;
    auto step = toDouble(mProperties[STEP_ID], ok);

    if (ok && (step > 0.0)) {
        mStep = step;
    } else {
        addError(R"(The "Step" property has an invalid value (")" + mProperties[STEP_ID] + R"("). It must be a floating point number greater than zero.)");

        return false;
    }

    // Create our various arrays.

    mYk = new double[pSize] {};

    // Initialise the ODE solver itself.

    return SolverOde::Impl::initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverSecondOrderRungeKutta::Impl::solve(double &pVoi, double pVoiEnd) const
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

    while (!libOpenCOR::fuzzyCompare(pVoi, pVoiEnd)) {
        // Check that the step is correct.

        if (pVoi + realStep > pVoiEnd) {
            realStep = pVoiEnd - pVoi;
            realHalfStep = HALF * realStep;
        }

        // Compute f(t_n, Y_n).

        mComputeRates(pVoi, mStates, mRates, mVariables);

        // Compute Y_n + h / 2 * k1.

        for (size_t i = 0; i < mSize; ++i) {
            mYk[i] = mStates[i] + realHalfStep * mRates[i]; // NOLINT
        }

        // Compute f(t_n + h / 2, Y_n + h / 2 * k1).

        mComputeRates(pVoi + realHalfStep, mYk, mRates, mVariables);

        // Compute Y_n+1.

        for (size_t i = 0; i < mSize; ++i) {
            mStates[i] += realStep * mRates[i]; // NOLINT
        }

        // Update the variable of integration.

        pVoi = libOpenCOR::fuzzyCompare(realStep, mStep) ?
                   voiStart + static_cast<double>(++voiCounter) * mStep :
                   pVoiEnd;
    }

    return true;
}

SolverSecondOrderRungeKutta::SolverSecondOrderRungeKutta()
    : SolverOde(new Impl())
{
}

SolverSecondOrderRungeKutta::~SolverSecondOrderRungeKutta()
{
    delete pimpl();
}

SolverSecondOrderRungeKutta::Impl *SolverSecondOrderRungeKutta::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverSecondOrderRungeKutta::Impl *SolverSecondOrderRungeKutta::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

bool SolverSecondOrderRungeKutta::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                             ComputeRates pComputeRates)
{
    return pimpl()->initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverSecondOrderRungeKutta::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
