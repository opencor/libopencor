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

#include "solvercvode_p.h"
#include "utils.h"

namespace libOpenCOR {

const std::string SolverCvode::Impl::NAME = "CVODE"; // NOLINT
const std::string SolverCvode::Impl::KISAO_ID = "KISAO:0000019"; // NOLINT

const std::string SolverCvode::Impl::STEP_NAME = "Step"; // NOLINT
const std::string SolverCvode::Impl::STEP_KISAO_ID = "KISAO:0000483"; // NOLINT

SolverPtr SolverCvode::Impl::create()
{
    return std::shared_ptr<SolverCvode> {new SolverCvode {}};
}

std::vector<SolverPropertyPtr> SolverCvode::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, STEP_NAME, STEP_KISAO_ID,
                                     {},
                                     std::to_string(STEP_DEFAULT_VALUE),
                                     true),
    };
}

SolverCvode::Impl::Impl()
    : SolverOde::Impl()
{
    mIsValid = true;

    mProperties[STEP_KISAO_ID] = std::to_string(STEP_DEFAULT_VALUE);
}

std::map<std::string, std::string> SolverCvode::Impl::propertiesKisaoId() const
{
    static const std::map<std::string, std::string> PROPERTIES_KISAO_ID = {
        {STEP_NAME, STEP_KISAO_ID},
    };

    return PROPERTIES_KISAO_ID;
}

bool SolverCvode::Impl::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
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

    // Initialise the ODE solver itself.

    return SolverOde::Impl::initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverCvode::Impl::solve(double &pVoi, double pVoiEnd) const
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

SolverCvode::SolverCvode()
    : SolverOde(new Impl())
{
}

SolverCvode::~SolverCvode()
{
    delete pimpl();
}

SolverCvode::Impl *SolverCvode::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverCvode::Impl *SolverCvode::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

bool SolverCvode::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                             ComputeRates pComputeRates)
{
    return pimpl()->initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverCvode::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
