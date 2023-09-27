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
#include "utils.h"

namespace libOpenCOR {

// Properties information.

const std::string SolverForwardEuler::Impl::NAME = "Forward Euler"; // NOLINT
const std::string SolverForwardEuler::Impl::KISAO_ID = "KISAO:0000030"; // NOLINT

const std::string SolverForwardEuler::Impl::STEP_NAME = "Step"; // NOLINT
const std::string SolverForwardEuler::Impl::STEP_KISAO_ID = "KISAO:0000483"; // NOLINT

// Solver.

SolverPtr SolverForwardEuler::Impl::create()
{
    return std::shared_ptr<SolverForwardEuler> {new SolverForwardEuler {}};
}

std::vector<SolverPropertyPtr> SolverForwardEuler::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, STEP_NAME, STEP_KISAO_ID,
                                     {},
                                     std::to_string(STEP_DEFAULT_VALUE),
                                     true),
    };
}

SolverForwardEuler::Impl::Impl()
    : SolverOde::Impl()
{
    mIsValid = true;

    mProperties[STEP_KISAO_ID] = std::to_string(STEP_DEFAULT_VALUE);
}

std::map<std::string, std::string> SolverForwardEuler::Impl::propertiesKisaoId() const
{
    static const std::map<std::string, std::string> PROPERTIES_KISAO_ID = {
        {STEP_NAME, STEP_KISAO_ID},
    };

    return PROPERTIES_KISAO_ID;
}

bool SolverForwardEuler::Impl::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                          ComputeRates pComputeRates)
{
    removeAllIssues();

    // Retrieve the solver's properties.

    bool ok = true;
    auto step = toDouble(mProperties[STEP_KISAO_ID], ok);

    if (ok && (step > 0.0)) {
        mStep = step;
    } else {
        addError(R"(The "Step" property has an invalid value (")" + mProperties[STEP_KISAO_ID] + R"("). It must be a floating point number greater than zero.)");

        return false;
    }

    // Initialise the ODE solver itself.

    return SolverOde::Impl::initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverForwardEuler::Impl::solve(double &pVoi, double pVoiEnd) const
{
    // We compute the following:
    //   Y_n+1 = Y_n + h * f(t_n, Y_n)

    const auto voiStart = pVoi;
    size_t voiCounter = 0;
    auto realStep = mStep;

    while (!libOpenCOR::fuzzyCompare(pVoi, pVoiEnd)) {
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

        pVoi = libOpenCOR::fuzzyCompare(realStep, mStep) ?
                   voiStart + static_cast<double>(++voiCounter) * mStep :
                   pVoiEnd;
    }

    return true;
}

SolverForwardEuler::SolverForwardEuler()
    : SolverOde(new Impl())
{
}

SolverForwardEuler::~SolverForwardEuler()
{
    delete pimpl();
}

SolverForwardEuler::Impl *SolverForwardEuler::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverForwardEuler::Impl *SolverForwardEuler::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

bool SolverForwardEuler::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                    ComputeRates pComputeRates)
{
    return pimpl()->initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverForwardEuler::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
