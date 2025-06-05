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

SedChangeAttribute::Impl::Impl(const std::string &pTarget, const std::string &pNewValue)
    : SedChange::Impl(pTarget)
    , mNewValue(pNewValue)
{
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
    xmlNewProp(pNode, toConstXmlCharPtr("newValue"), toConstXmlCharPtr(mNewValue));
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

SedChangeAttributePtr SedChangeAttribute::create(const std::string &pTarget, const std::string &pNewValue)
{
    return SedChangeAttributePtr {new SedChangeAttribute {pTarget, pNewValue}};
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
