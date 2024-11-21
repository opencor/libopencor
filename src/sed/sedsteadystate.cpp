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

#include "sedsteadystate_p.h"

#include "utils.h"

namespace libOpenCOR {

SedSteadyState::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

void SedSteadyState::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("steadyState"));

    SedSimulation::Impl::serialise(node);

    xmlAddChild(pNode, node);
}

SedSteadyState::SedSteadyState(const SedDocumentPtr &pDocument)
    : SedSimulation(new Impl(pDocument))
{
}

SedSteadyState::~SedSteadyState()
{
    delete pimpl();
}

SedSteadyState::Impl *SedSteadyState::pimpl()
{
    return reinterpret_cast<SedSteadyState::Impl *>(SedSimulation::pimpl());
}

/*---GRY---
const SedSteadyState::Impl *SedSteadyState::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}
*/

SedSteadyStatePtr SedSteadyState::create(const SedDocumentPtr &pDocument)
{
    return SedSteadyStatePtr {new SedSteadyState {pDocument}};
}

} // namespace libOpenCOR
