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

const std::string SolverForwardEuler::Impl::Name = "Forward Euler"; // NOLINT
const std::string SolverForwardEuler::Impl::KisaoId = "KISAO:0000030"; // NOLINT

const std::string SolverForwardEuler::Impl::StepName = "Step"; // NOLINT
const std::string SolverForwardEuler::Impl::StepKisaoId = "KISAO:0000483"; // NOLINT

SolverOdePtr SolverForwardEuler::Impl::create()
{
    return std::shared_ptr<SolverForwardEuler> {new SolverForwardEuler {}};
}

std::vector<SolverPropertyPtr> SolverForwardEuler::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, StepName, StepKisaoId,
                                     {},
                                     std::to_string(StepDefaultValue),
                                     true),
    };
}

SolverForwardEuler::Impl::Impl()
    : SolverOde::Impl()
{
    mIsValid = true;

    mProperties[StepKisaoId] = std::to_string(StepDefaultValue);
}

std::map<std::string, std::string> SolverForwardEuler::Impl::propertiesKisaoId() const
{
    static const std::map<std::string, std::string> PropertiesKisaoId = {
        {StepName, StepKisaoId},
    };

    return PropertiesKisaoId;
}

bool SolverForwardEuler::Impl::initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                                          ComputeRates pComputeRates)
{
    removeAllIssues();

    bool ok = true;
    auto step = stringToDouble(mProperties[StepKisaoId], &ok);

    if (ok && (step > 0.0)) {
        mStep = step;
    } else {
        addError(R"(The "Step" property has an invalid value (")" + mProperties[StepKisaoId] + R"("). It must be a floating point number greater than zero.)");

        return false;
    }

    return SolverOde::Impl::initialise(pSize, pStates, pRates, pVariables, pComputeRates);
}

void SolverForwardEuler::Impl::solve(double &pVoi, double pVoiEnd) const
{
    // Y_n+1 = Y_n + h * f(t_n, Y_n).

    const double voiStart = pVoi;
    size_t voiCounter = 0;
    double realStep = mStep;

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
                   voiStart + static_cast<double>(++voiCounter) *realStep :
                   pVoi = pVoiEnd;
    }
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

void SolverForwardEuler::solve(double &pVoi, double pVoiEnd) const
{
    pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR
