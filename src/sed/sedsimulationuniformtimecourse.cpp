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
#include "sedsimulationuniformtimecourse_p.h"
#include "utils.h"

namespace libOpenCOR {

SedSimulationUniformTimeCourse::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

void SedSimulationUniformTimeCourse::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("uniformTimeCourse"));

    SedSimulation::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("initialTime"), toConstXmlCharPtr(toString(mInitialTime)));
    xmlNewProp(node, toConstXmlCharPtr("outputStartTime"), toConstXmlCharPtr(toString(mOutputStartTime)));
    xmlNewProp(node, toConstXmlCharPtr("outputEndTime"), toConstXmlCharPtr(toString(mOutputEndTime)));
    xmlNewProp(node, toConstXmlCharPtr("numberOfSteps"), toConstXmlCharPtr(toString(mNumberOfSteps)));

    xmlAddChild(pNode, node);
}

SedSimulationUniformTimeCourse::SedSimulationUniformTimeCourse(const SedDocumentPtr &pDocument)
    : SedSimulation(new Impl(pDocument))
{
}

SedSimulationUniformTimeCourse::~SedSimulationUniformTimeCourse()
{
    delete pimpl();
}

SedSimulationUniformTimeCourse::Impl *SedSimulationUniformTimeCourse::pimpl()
{
    return reinterpret_cast<SedSimulationUniformTimeCourse::Impl *>(SedSimulation::pimpl());
}

/*---GRY---
const SedSimulationUniformTimeCourse::Impl *SedSimulationUniformTimeCourse::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}
*/

SedSimulationUniformTimeCoursePtr SedSimulationUniformTimeCourse::create(const SedDocumentPtr &pDocument)
{
    return SedSimulationUniformTimeCoursePtr {new SedSimulationUniformTimeCourse {pDocument}};
}

} // namespace libOpenCOR
