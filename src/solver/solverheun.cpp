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
#include "utils.h"

namespace libOpenCOR {

// Properties information.

const std::string SolverHeun::Impl::NAME = "Heun"; // NOLINT
const std::string SolverHeun::Impl::KISAO_ID = "KISAO:0000301"; // NOLINT

const std::string SolverHeun::Impl::STEP_NAME = "Step"; // NOLINT
const std::string SolverHeun::Impl::STEP_KISAO_ID = "KISAO:0000483"; // NOLINT

// Solver.

SolverPtr SolverHeun::Impl::create()
{
    return std::shared_ptr<SolverHeun> {new SolverHeun {}};
}

std::vector<SolverPropertyPtr> SolverHeun::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, STEP_NAME, STEP_KISAO_ID,
                                     {},
                                     std::to_string(STEP_DEFAULT_VALUE),
                                     true),
    };
}

SolverHeun::Impl::Impl()
    : SolverOde::Impl()
{
    mIsValid = true;

    mProperties[STEP_KISAO_ID] = std::to_string(STEP_DEFAULT_VALUE);
}

SolverHeun::Impl::~Impl()
{
    delete[] mK;
    delete[] mYk;
}

std::map<std::string, std::string> SolverHeun::Impl::propertiesKisaoId() const
{
    static const std::map<std::string, std::string> PROPERTIES_KISAO_ID = {
        {STEP_NAME, STEP_KISAO_ID},
    };

    return PROPERTIES_KISAO_ID;
}

bool SolverHeun::Impl::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                  ComputeRates pComputeRates)
{
    removeAllIssues();

    // Retrieve the solver's properties.

    bool ok = true;
    auto step = stringToDouble(mProperties[STEP_KISAO_ID], ok);

    if (ok && (step > 0.0)) {
        mStep = step;
    } else {
        addError(R"(The "Step" property has an invalid value (")" + mProperties[STEP_KISAO_ID] + R"("). It must be a floating point number greater than zero.)");

        return false;
    }

    // Create our various arrays.

    mK = new double[pSize] {};
    mYk = new double[pSize] {};

    // Initialise the ODE solver itself.

    return SolverOde::Impl::initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverHeun::Impl::solve(double &pVoi, double pVoiEnd) const
{
    // We compute the following:
    //   k = f(t_n, Y_n)
    //   Y_n+1 = Y_n + h / 2 * ( f(t_n, Y_n) + f(t_n + h, Y_n + h * k) )

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

        // Compute k and Y_n + h * k.

        for (size_t i = 0; i < mSize; ++i) {
            mK[i] = mRates[i]; // NOLINT
            mYk[i] = mStates[i] + realStep * mK[i]; // NOLINT
        }

        // Compute f(t_n + h, Y_n + h * k).

        mComputeRates(pVoi + realStep, mYk, mRates, mVariables);

        // Compute Y_n+1.

        for (size_t i = 0; i < mSize; ++i) {
            mStates[i] += realHalfStep * (mK[i] + mRates[i]); // NOLINT
        }

        // Update the variable of integration.

        pVoi = libOpenCOR::fuzzyCompare(realStep, mStep) ?
                   voiStart + static_cast<double>(++voiCounter) * mStep :
                   pVoiEnd;
    }

    return true;
}

SolverHeun::SolverHeun()
    : SolverOde(new Impl())
{
}

SolverHeun::~SolverHeun()
{
    delete pimpl();
}

SolverHeun::Impl *SolverHeun::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverHeun::Impl *SolverHeun::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

bool SolverHeun::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                            ComputeRates pComputeRates)
{
    return pimpl()->initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverHeun::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
