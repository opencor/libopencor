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

#include "file_p.h"
#include "seddocument_p.h"
#include "sedmodel_p.h"
#include "sedsimulation_p.h"
#include "solvernla_p.h"
#include "solverode_p.h"

namespace libOpenCOR {

static constexpr auto ID_PREFIX = "simulation";

SedSimulation::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedBase::Impl(pDocument->pimpl()->uniqueId(ID_PREFIX))
{
}

bool SedSimulation::Impl::isValid(const SedModelPtr &pModel)
{
    auto modelType = pModel->pimpl()->mFile->pimpl()->mCellmlFile->type();

    if ((modelType == libcellml::AnalyserModel::Type::ODE) && (mOdeSolver == nullptr)) {
        addError(std::string("Simulation '").append(mId).append("' is to be used with model '").append(pModel->pimpl()->mId).append("' which requires an ODE solver but none is provided."));
    } else if ((modelType == libcellml::AnalyserModel::Type::NLA) && (mNlaSolver == nullptr)) {
        addError(std::string("Simulation '").append(mId).append("' is to be used with model '").append(pModel->pimpl()->mId).append("' which requires an NLA solver but none is provided."));
    } else if (modelType == libcellml::AnalyserModel::Type::DAE) {
        if (mOdeSolver == nullptr) {
            addError(std::string("Simulation '").append(mId).append("' is to be used with model '").append(pModel->pimpl()->mId).append("' which requires an ODE solver but none is provided."));
        }

        if (mNlaSolver == nullptr) {
            addError(std::string("Simulation '").append(mId).append("' is to be used with model '").append(pModel->pimpl()->mId).append("' which requires an NLA solver but none is provided."));
        }
    }

    return !hasErrors();
}

SolverOdePtr SedSimulation::Impl::odeSolver() const
{
    return mOdeSolver;
}

void SedSimulation::Impl::setOdeSolver(const SolverOdePtr &pOdeSolver)
{
    mOdeSolver = pOdeSolver;
}

SolverNlaPtr SedSimulation::Impl::nlaSolver() const
{
    return mNlaSolver;
}

void SedSimulation::Impl::setNlaSolver(const SolverNlaPtr &pNlaSolver)
{
    mNlaSolver = pNlaSolver;
}

void SedSimulation::Impl::serialise(xmlNodePtr pNode) const
{
    SedBase::Impl::serialise(pNode);

    if ((mOdeSolver != nullptr) || (mNlaSolver != nullptr)) {
        if (mOdeSolver != nullptr) {
            mOdeSolver->pimpl()->serialise(pNode);
        }

        if (mNlaSolver != nullptr) {
            mNlaSolver->pimpl()->serialise(pNode, mOdeSolver != nullptr);
        }
    }
}

SedSimulation::SedSimulation(Impl *pPimpl)
    : SedBase(pPimpl)
{
}

SedSimulation::Impl *SedSimulation::pimpl()
{
    return static_cast<Impl *>(SedBase::pimpl());
}

const SedSimulation::Impl *SedSimulation::pimpl() const
{
    return static_cast<const Impl *>(SedBase::pimpl());
}

SolverOdePtr SedSimulation::odeSolver() const
{
    return pimpl()->odeSolver();
}

void SedSimulation::setOdeSolver(const SolverOdePtr &pOdeSolver)
{
    pimpl()->setOdeSolver(pOdeSolver);
}

SolverNlaPtr SedSimulation::nlaSolver() const
{
    return pimpl()->nlaSolver();
}

void SedSimulation::setNlaSolver(const SolverNlaPtr &pNlaSolver)
{
    pimpl()->setNlaSolver(pNlaSolver);
}

} // namespace libOpenCOR
