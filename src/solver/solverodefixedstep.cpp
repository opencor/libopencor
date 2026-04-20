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

#include "solverodefixedstep_p.h"

#include "sedml/SedAlgorithm.h"

namespace libOpenCOR {

// Solver.

SolverOdeFixedStep::Impl::Impl(const std::string &pId, const std::string &pName)
    : SolverOde::Impl(pId, pName)
{
}

void SolverOdeFixedStep::Impl::populate(libsedml::SedAlgorithm *pAlgorithm)
{
    auto addUnknownParameterWarning = [&](const std::string &pKisaoId) {
        std::string warning;

        warning.reserve(pKisaoId.size() + 49); // NOLINT

        warning += "The parameter '";
        warning += pKisaoId;
        warning += "' is not recognised. It will be ignored.";

        addWarning(warning);
    };

    for (unsigned int i {0}; i < pAlgorithm->getNumAlgorithmParameters(); ++i) {
        auto *algorithmParameter {pAlgorithm->getAlgorithmParameter(i)};
        const auto &kisaoId {algorithmParameter->getKisaoID()};
        const auto &value {algorithmParameter->getValue()};

        if (kisaoId == "KISAO:0000483") {
            mStep = toDouble(value);

            if ((mStep <= 0.0) || std::isnan(mStep)) {
                const auto defaultStep {toString(DEFAULT_STEP)};
                std::string warning;

                warning.reserve(kisaoId.size() + value.size() + defaultStep.size() + 85); // NOLINT

                warning += "The step ('";
                warning += kisaoId;
                warning += "') cannot be equal to '";
                warning += value;
                warning += "'. It must be greater or equal to 0. A step of ";
                warning += defaultStep;
                warning += " will be used instead.";

                addWarning(warning);

                mStep = DEFAULT_STEP;
            }
        } else {
            addUnknownParameterWarning(kisaoId);
        }
    }
}

SolverPtr SolverOdeFixedStep::Impl::duplicate(const SolverOdeFixedStepPtr &pSolver) const
{
    auto *solverPimpl {pSolver->pimpl()};

    solverPimpl->mStep = mStep;

    return pSolver;
}

StringStringMap SolverOdeFixedStep::Impl::properties() const
{
    StringStringMap res;

    res["KISAO:0000483"] = toString(mStep);

    return res;
}

bool SolverOdeFixedStep::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                          double *pConstants, double *pComputedConstants, double *pAlgebraicVariables,
                                          const CellmlFileRuntimePtr &pRuntime)
{
    // Initialise the ODE solver itself.

    SolverOde::Impl::initialise(pVoi, pSize, pStates, pRates,
                                pConstants, pComputedConstants, pAlgebraicVariables,
                                pRuntime);

    // Check the solver's properties.

    if (mStep <= 0.0) {
        const auto stepAsString {toString(mStep)};
        std::string error;

        error.reserve(stepAsString.size() + 45); // NOLINT

        error += "The step cannot be equal to ";
        error += stepAsString;
        error += ". It must be greater than 0.";

        addError(error);

        return false;
    }

    return true;
}

double SolverOdeFixedStep::Impl::step() const
{
    return mStep;
}

void SolverOdeFixedStep::Impl::setStep(double pStep)
{
    mStep = pStep;
}

SolverOdeFixedStep::SolverOdeFixedStep(Impl *pPimpl)
    : SolverOde(pPimpl)
{
}

SolverOdeFixedStep::Impl *SolverOdeFixedStep::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverOdeFixedStep::Impl *SolverOdeFixedStep::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

double SolverOdeFixedStep::step() const
{
    return pimpl()->step();
}

void SolverOdeFixedStep::setStep(double pStep)
{
    pimpl()->setStep(pStep);
}

} // namespace libOpenCOR
