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

#include "sedanalysis_p.h"

#include "seddocument_p.h"

#include "utils.h"

namespace libOpenCOR {

SedAnalysis::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

/*---GRY---
void SedAnalysis::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("steadyState"));

    SedSimulation::Impl::serialise(node);

    xmlAddChild(pNode, node);
}
*/

SedAnalysis::SedAnalysis(const SedDocumentPtr &pDocument)
    : SedSimulation(new Impl(pDocument))
{
}

SedAnalysis::~SedAnalysis()
{
    delete pimpl();
}

SedAnalysis::Impl *SedAnalysis::pimpl()
{
    return reinterpret_cast<SedAnalysis::Impl *>(SedSimulation::pimpl());
}

/*---GRY---
const SedAnalysis::Impl *SedAnalysis::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}
*/

SedAnalysisPtr SedAnalysis::create(const SedDocumentPtr &pDocument)
{
    return SedAnalysisPtr {new SedAnalysis {pDocument}};
}

} // namespace libOpenCOR
