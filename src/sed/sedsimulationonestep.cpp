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

#include "seddocument_p.h"
#include "sedsimulationonestep_p.h"
#include "utils.h"

namespace libOpenCOR {

SedSimulationOneStep::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

void SedSimulationOneStep::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("oneStep"));

    SedSimulation::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("step"), toConstXmlCharPtr(toString(mStep)));

    xmlAddChild(pNode, node);
}

SedSimulationOneStep::SedSimulationOneStep(const SedDocumentPtr &pDocument)
    : SedSimulation(new Impl(pDocument))
{
}

SedSimulationOneStep::~SedSimulationOneStep()
{
    delete pimpl();
}

SedSimulationOneStep::Impl *SedSimulationOneStep::pimpl()
{
    return reinterpret_cast<SedSimulationOneStep::Impl *>(SedSimulation::pimpl());
}

/*---GRY---
const SedSimulationOneStep::Impl *SedSimulationOneStep::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}
*/

SedSimulationOneStepPtr SedSimulationOneStep::create(const SedDocumentPtr &pDocument)
{
    return SedSimulationOneStepPtr {new SedSimulationOneStep {pDocument}};
}

} // namespace libOpenCOR
