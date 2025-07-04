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
#include "sedmodel_p.h"
#include "sedsimulation_p.h"
#include "sedtask_p.h"

#include "utils.h"

namespace libOpenCOR {

SedTask::Impl::Impl(const SedDocumentPtr &pDocument, const SedModelPtr &pModel,
                    const SedSimulationPtr &pSimulation)
    : SedAbstractTask::Impl(pDocument)
    , mModel(pModel)
    , mSimulation(pSimulation)
{
}

bool SedTask::Impl::isValid()
{
    // Make sure that we have both a model and a simulation.

    if (mModel == nullptr) {
        addError(std::string("Task '").append(mId).append("' requires a model."));
    }

    if (mSimulation == nullptr) {
        addError(std::string("Task '").append(mId).append("' requires a simulation."));
    }

    if (hasIssues()) {
        return false;
    }

    // Make sure that the model is of CellML type.

    if (mModel->pimpl()->mFile->type() != File::Type::CELLML_FILE) {
        addError(std::string("Task '").append(mId).append("' requires a model of CellML type."));

        return false;
    }

    // Make sure that the model is valid.

    if (!mModel->pimpl()->isValid()) {
        addIssues(mModel);
    }

    // Make sure that the simulation is valid for the model.

    if (!mSimulation->pimpl()->isValid(mModel)) {
        addIssues(mSimulation);
    }

    return !hasIssues();
}

SedModelPtr SedTask::Impl::model() const
{
    return mModel;
}

void SedTask::Impl::setModel(const SedModelPtr &pModel)
{
    mModel = pModel;
}

SedSimulationPtr SedTask::Impl::simulation() const
{
    return mSimulation;
}

void SedTask::Impl::setSimulation(const SedSimulationPtr &pSimulation)
{
    mSimulation = pSimulation;
}

void SedTask::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("task"));

    SedAbstractTask::Impl::serialise(node);

    if (mModel != nullptr) {
        xmlNewProp(node, toConstXmlCharPtr("modelReference"), toConstXmlCharPtr(mModel->pimpl()->mId));
    }

    if (mSimulation != nullptr) {
        xmlNewProp(node, toConstXmlCharPtr("simulationReference"), toConstXmlCharPtr(mSimulation->pimpl()->mId));
    }

    xmlAddChild(pNode, node);
}

SedTask::SedTask(const SedDocumentPtr &pDocument, const SedModelPtr &pModel, const SedSimulationPtr &pSimulation)
    : SedAbstractTask(new Impl(pDocument, pModel, pSimulation))
{
}

SedTask::~SedTask()
{
    delete pimpl();
}

SedTask::Impl *SedTask::pimpl()
{
    return reinterpret_cast<SedTask::Impl *>(SedAbstractTask::pimpl());
}

const SedTask::Impl *SedTask::pimpl() const
{
    return static_cast<const Impl *>(SedAbstractTask::pimpl());
}

SedTaskPtr SedTask::create(const SedDocumentPtr &pDocument, const SedModelPtr &pModel,
                           const SedSimulationPtr &pSimulation)
{
    return SedTaskPtr {new SedTask {pDocument, pModel, pSimulation}};
}

SedModelPtr SedTask::model() const
{
    return pimpl()->model();
}

void SedTask::setModel(const SedModelPtr &pModel)
{
    pimpl()->setModel(pModel);
}

SedSimulationPtr SedTask::simulation() const
{
    return pimpl()->simulation();
}

void SedTask::setSimulation(const SedSimulationPtr &pSimulation)
{
    pimpl()->setSimulation(pSimulation);
}

} // namespace libOpenCOR
