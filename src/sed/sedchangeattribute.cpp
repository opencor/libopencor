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

#include "sedchangeattribute_p.h"

#include "utils.h"

namespace libOpenCOR {

SedChangeAttribute::Impl::Impl(const std::string &pComponent, const std::string &pVariable,
                               const std::string &pNewValue)
    : mComponent(pComponent)
    , mVariable(pVariable)
    , mNewValue(pNewValue)
{
    updateTarget();
}

void SedChangeAttribute::Impl::setTarget(const std::string &pTarget)
{
    mTarget = pTarget;
}

void SedChangeAttribute::Impl::updateTarget()
{
    setTarget("/cellml:model/cellml:component[@name='" + mComponent + "']/cellml:variable[@name='" + mVariable + "']");
}

std::string SedChangeAttribute::Impl::component() const
{
    return mComponent;
}

void SedChangeAttribute::Impl::setComponent(const std::string &pComponent)
{
    mComponent = pComponent;

    updateTarget();
}

std::string SedChangeAttribute::Impl::variable() const
{
    return mVariable;
}

void SedChangeAttribute::Impl::setVariable(const std::string &pVariable)
{
    mVariable = pVariable;

    updateTarget();
}

std::string SedChangeAttribute::Impl::newValue() const
{
    return mNewValue;
}

void SedChangeAttribute::Impl::setNewValue(const std::string &pNewValue)
{
    mNewValue = pNewValue;
}

void SedChangeAttribute::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("changeAttribute"));

    SedChange::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("newValue"), toConstXmlCharPtr(mNewValue));

    xmlAddChild(pNode, node);
}

SedChangeAttribute::SedChangeAttribute(const std::string &pComponent, const std::string &pVariable,
                                       const std::string &pNewValue)
    : SedChange(new Impl(pComponent, pVariable, pNewValue))
{
}

SedChangeAttribute::~SedChangeAttribute()
{
    delete pimpl();
}

SedChangeAttribute::Impl *SedChangeAttribute::pimpl()
{
    return static_cast<Impl *>(SedChange::pimpl());
}

const SedChangeAttribute::Impl *SedChangeAttribute::pimpl() const
{
    return static_cast<const Impl *>(SedChange::pimpl());
}

SedChangeAttributePtr SedChangeAttribute::create(const std::string &pComponent, const std::string &pVariable,
                                                 const std::string &pNewValue)
{
    return SedChangeAttributePtr(new SedChangeAttribute(pComponent, pVariable, pNewValue));
}

std::string SedChangeAttribute::component() const
{
    return pimpl()->component();
}

void SedChangeAttribute::setComponent(const std::string &pComponent)
{
    pimpl()->setComponent(pComponent);
}

std::string SedChangeAttribute::variable() const
{
    return pimpl()->variable();
}

void SedChangeAttribute::setVariable(const std::string &pVariable)
{
    pimpl()->setVariable(pVariable);
}

std::string SedChangeAttribute::newValue() const
{
    return pimpl()->newValue();
}

void SedChangeAttribute::setNewValue(const std::string &pNewValue)
{
    pimpl()->setNewValue(pNewValue);
}

} // namespace libOpenCOR
