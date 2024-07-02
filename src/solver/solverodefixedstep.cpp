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

#include "utils.h"

#include "libsedmlbegin.h"
#include <sedml/SedAlgorithm.h>
#include "libsedmlend.h"

namespace libOpenCOR {

// Solver.

SolverOdeFixedStep::Impl::Impl(const std::string &pId, const std::string &pName)
    : SolverOde::Impl(pId, pName)
{
}

void SolverOdeFixedStep::Impl::populate(libsedml::SedAlgorithm *pAlgorithm)
{
    for (unsigned int i = 0; i < pAlgorithm->getNumAlgorithmParameters(); ++i) {
        auto *algorithmParameter = pAlgorithm->getAlgorithmParameter(i);
        auto kisaoId = algorithmParameter->getKisaoID();
        auto value = algorithmParameter->getValue();

        if (kisaoId == "KISAO:0000483") {
            mStep = toDouble(value);

            if ((mStep <= 0.0) || std::isnan(mStep)) {
                addWarning(std::string("The step ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater or equal to 0. A step of ").append(toString(DEFAULT_STEP)).append(" will be used instead."));

                mStep = DEFAULT_STEP;
            }
        } else {
            addWarning(std::string("The parameter '").append(kisaoId).append("' is not recognised. It will be ignored."));
        }
    }
}

SolverPtr SolverOdeFixedStep::Impl::duplicate(const SolverOdeFixedStepPtr &pSolver) const
{
    auto *solverPimpl = pSolver->pimpl();

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
                                          double *pVariables,
                                          CellmlFileRuntime::ComputeCompiledRates pComputeCompiledRates,
                                          CellmlFileRuntime::ComputeInterpretedRates pComputeInterpretedRates)
{
    // Initialise the ODE solver itself.

    SolverOde::Impl::initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeCompiledRates,
                                pComputeInterpretedRates);

    // Check the solver's properties.

    if (mStep <= 0.0) {
        addError(std::string("The step cannot be equal to ").append(toString(mStep)).append(". It must be greater than 0."));

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
