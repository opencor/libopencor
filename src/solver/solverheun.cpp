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

const std::string SolverHeun::Impl::ID = "KISAO:0000301"; // NOLINT
const std::string SolverHeun::Impl::NAME = "Heun"; // NOLINT

const std::string SolverHeun::Impl::STEP_ID = "KISAO:0000483"; // NOLINT
const std::string SolverHeun::Impl::STEP_NAME = "Step"; // NOLINT

// Solver.

SolverPtr SolverHeun::Impl::create()
{
    return std::shared_ptr<SolverHeun> {new SolverHeun {}};
}

SolverPropertyPtrVector SolverHeun::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, STEP_ID, STEP_NAME,
                                     {},
                                     toString(STEP_DEFAULT_VALUE),
                                     true),
    };
}

StringVector SolverHeun::Impl::hiddenProperties(const StringStringMap &pProperties)
{
    (void)pProperties;

    return {};
}

SolverHeun::Impl::Impl()
    : SolverOde::Impl()
{
    mProperties[STEP_ID] = toString(STEP_DEFAULT_VALUE);
}

SolverHeun::Impl::~Impl()
{
    delete[] mK;
    delete[] mYk;
}

StringStringMap SolverHeun::Impl::propertiesId() const
{
    static const StringStringMap PROPERTIES_ID = {
        {STEP_NAME, STEP_ID},
    };

    return PROPERTIES_ID;
}

bool SolverHeun::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                                  ComputeRates pComputeRates)
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

    mK = new double[pSize] {};
    mYk = new double[pSize] {};

    // Initialise the ODE solver itself.

    return SolverOde::Impl::initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
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

SolverInfoPtr SolverHeun::info() const
{
    return Solver::solversInfo()[Solver::Impl::SolversIndex[SolverHeun::Impl::ID]];
}

Solver::Type SolverHeun::type() const
{
    return Type::ODE;
}

std::string SolverHeun::id() const
{
    return Impl::ID;
}

std::string SolverHeun::name() const
{
    return Impl::NAME;
}

bool SolverHeun::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                            ComputeRates pComputeRates)
{
    return pimpl()->initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverHeun::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
