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

static constexpr auto ID_PREFIX {"simulation"};

SedSimulation::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedBase::Impl(pDocument->pimpl()->uniqueId(ID_PREFIX))
{
}

bool SedSimulation::Impl::isValid(const SedModelPtr &pModel)
{
    auto modelType {pModel->pimpl()->mFile->pimpl()->mCellmlFile->type()};
    const auto &modelId = pModel->pimpl()->mId;

    auto addMissingSolverError = [&](const char *pSolverType) {
        std::string error;

        error.reserve(mId.size() + modelId.size() + 96); // NOLINT

        error += "Simulation '";
        error += mId;
        error += "' is to be used with model '";
        error += modelId;
        error += "' which requires an ";
        error += pSolverType;
        error += " solver but none is provided.";

        addError(error);
    };

    if ((modelType == libcellml::AnalyserModel::Type::ODE) && (mOdeSolver == nullptr)) {
        addMissingSolverError("ODE");
    } else if ((modelType == libcellml::AnalyserModel::Type::NLA) && (mNlaSolver == nullptr)) {
        addMissingSolverError("NLA");
    } else if (modelType == libcellml::AnalyserModel::Type::DAE) {
        if (mOdeSolver == nullptr) {
            addMissingSolverError("ODE");
        }

        if (mNlaSolver == nullptr) {
            addMissingSolverError("NLA");
        }
    }

    return !hasErrors();
}

const SolverOdePtr &SedSimulation::Impl::odeSolver() const
{
    return mOdeSolver;
}

void SedSimulation::Impl::setOdeSolver(const SolverOdePtr &pOdeSolver)
{
    mOdeSolver = pOdeSolver;
}

const SolverNlaPtr &SedSimulation::Impl::nlaSolver() const
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

const SolverOdePtr &SedSimulation::odeSolver() const
{
    return pimpl()->odeSolver();
}

void SedSimulation::setOdeSolver(const SolverOdePtr &pOdeSolver)
{
    pimpl()->setOdeSolver(pOdeSolver);
}

const SolverNlaPtr &SedSimulation::nlaSolver() const
{
    return pimpl()->nlaSolver();
}

void SedSimulation::setNlaSolver(const SolverNlaPtr &pNlaSolver)
{
    pimpl()->setNlaSolver(pNlaSolver);
}

} // namespace libOpenCOR
