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

#include "sedonestep_p.h"

#include "seddocument_p.h"

#include "utils.h"

namespace libOpenCOR {

SedOneStep::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

double SedOneStep::Impl::step() const
{
    return mStep;
}

void SedOneStep::Impl::setStep(double pStep)
{
    mStep = pStep;
}

void SedOneStep::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("oneStep"));

    SedSimulation::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("step"), toConstXmlCharPtr(toString(mStep)));

    xmlAddChild(pNode, node);
}

SedOneStep::SedOneStep(const SedDocumentPtr &pDocument)
    : SedSimulation(new Impl(pDocument))
{
}

SedOneStep::~SedOneStep()
{
    delete pimpl();
}

SedOneStep::Impl *SedOneStep::pimpl()
{
    return reinterpret_cast<SedOneStep::Impl *>(SedSimulation::pimpl());
}

const SedOneStep::Impl *SedOneStep::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedSimulation::pimpl());
}

SedOneStepPtr SedOneStep::create(const SedDocumentPtr &pDocument)
{
    return SedOneStepPtr {new SedOneStep {pDocument}};
}

double SedOneStep::step() const
{
    return pimpl()->step();
}

void SedOneStep::setStep(double pStep)
{
    pimpl()->setStep(pStep);
}

} // namespace libOpenCOR
