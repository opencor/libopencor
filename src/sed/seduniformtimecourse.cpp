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
#include "seduniformtimecourse_p.h"
#include "utils.h"

namespace libOpenCOR {

SedUniformTimeCourse::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

void SedUniformTimeCourse::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("uniformTimeCourse"));

    SedSimulation::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("initialTime"), toConstXmlCharPtr(toString(mInitialTime)));
    xmlNewProp(node, toConstXmlCharPtr("outputStartTime"), toConstXmlCharPtr(toString(mOutputStartTime)));
    xmlNewProp(node, toConstXmlCharPtr("outputEndTime"), toConstXmlCharPtr(toString(mOutputEndTime)));
    xmlNewProp(node, toConstXmlCharPtr("numberOfSteps"), toConstXmlCharPtr(toString(mNumberOfSteps)));

    xmlAddChild(pNode, node);
}

SedUniformTimeCourse::SedUniformTimeCourse(const SedDocumentPtr &pDocument)
    : SedSimulation(new Impl(pDocument))
{
}

SedUniformTimeCourse::~SedUniformTimeCourse()
{
    delete pimpl();
}

SedUniformTimeCourse::Impl *SedUniformTimeCourse::pimpl()
{
    return reinterpret_cast<SedUniformTimeCourse::Impl *>(SedSimulation::pimpl());
}

const SedUniformTimeCourse::Impl *SedUniformTimeCourse::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}

SedUniformTimeCoursePtr SedUniformTimeCourse::create(const SedDocumentPtr &pDocument)
{
    return SedUniformTimeCoursePtr {new SedUniformTimeCourse {pDocument}};
}

double SedUniformTimeCourse::initialTime() const
{
    return pimpl()->mInitialTime;
}

void SedUniformTimeCourse::setInitialTime(double pInitialTime)
{
    pimpl()->mInitialTime = pInitialTime;
}

double SedUniformTimeCourse::outputStartTime() const
{
    return pimpl()->mOutputStartTime;
}

void SedUniformTimeCourse::setOutputStartTime(double pOutputStartTime)
{
    pimpl()->mOutputStartTime = pOutputStartTime;
}

double SedUniformTimeCourse::outputEndTime() const
{
    return pimpl()->mOutputEndTime;
}

void SedUniformTimeCourse::setOutputEndTime(double pOutputEndTime)
{
    pimpl()->mOutputEndTime = pOutputEndTime;
}

int SedUniformTimeCourse::numberOfSteps() const
{
    return pimpl()->mNumberOfSteps;
}

void SedUniformTimeCourse::setNumberOfSteps(int pNumberOfSteps)
{
    pimpl()->mNumberOfSteps = pNumberOfSteps;
}

} // namespace libOpenCOR
