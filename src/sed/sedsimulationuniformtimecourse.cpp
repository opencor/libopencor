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

const SedSimulationUniformTimeCourse::Impl *SedSimulationUniformTimeCourse::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}

SedSimulationUniformTimeCoursePtr SedSimulationUniformTimeCourse::create(const SedDocumentPtr &pDocument)
{
    return SedSimulationUniformTimeCoursePtr {new SedSimulationUniformTimeCourse {pDocument}};
}

double SedSimulationUniformTimeCourse::initialTime() const
{
    return pimpl()->mInitialTime;
}

void SedSimulationUniformTimeCourse::setInitialTime(double pInitialTime)
{
    pimpl()->mInitialTime = pInitialTime;
}

double SedSimulationUniformTimeCourse::outputStartTime() const
{
    return pimpl()->mOutputStartTime;
}

void SedSimulationUniformTimeCourse::setOutputStartTime(double pOutputStartTime)
{
    pimpl()->mOutputStartTime = pOutputStartTime;
}

double SedSimulationUniformTimeCourse::outputEndTime() const
{
    return pimpl()->mOutputEndTime;
}

void SedSimulationUniformTimeCourse::setOutputEndTime(double pOutputEndTime)
{
    pimpl()->mOutputEndTime = pOutputEndTime;
}

int SedSimulationUniformTimeCourse::numberOfSteps() const
{
    return pimpl()->mNumberOfSteps;
}

void SedSimulationUniformTimeCourse::setNumberOfSteps(int pNumberOfSteps)
{
    pimpl()->mNumberOfSteps = pNumberOfSteps;
}

} // namespace libOpenCOR
