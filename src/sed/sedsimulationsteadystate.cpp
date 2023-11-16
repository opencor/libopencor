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
#include "sedsimulationsteadystate_p.h"
#include "utils.h"

namespace libOpenCOR {

SedSimulationSteadyState::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

void SedSimulationSteadyState::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, constXmlCharPtr("steadyState"));

    SedSimulation::Impl::serialise(node);

    xmlAddChild(pNode, node);
}

SedSimulationSteadyState::SedSimulationSteadyState(const SedDocumentPtr &pDocument)
    : SedSimulation(new Impl(pDocument))
{
}

SedSimulationSteadyState::~SedSimulationSteadyState()
{
    delete pimpl();
}

SedSimulationSteadyState::Impl *SedSimulationSteadyState::pimpl()
{
    return reinterpret_cast<SedSimulationSteadyState::Impl *>(SedSimulation::pimpl());
}

/*---GRY---
const SedSimulationSteadyState::Impl *SedSimulationSteadyState::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}
*/

SedSimulationSteadyStatePtr SedSimulationSteadyState::create(const SedDocumentPtr &pDocument)
{
    return SedSimulationSteadyStatePtr {new SedSimulationSteadyState {pDocument}};
}

} // namespace libOpenCOR
