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

#include "sedchange_p.h"

#include "utils.h"

namespace libOpenCOR {

SedChange::Impl::Impl(const std::string &pTarget)
    : mTarget(pTarget)
{
}

std::string SedChange::Impl::target() const
{
    return mTarget;
}

void SedChange::Impl::serialise(xmlNodePtr pNode) const
{
    if (!mTarget.empty()) {
        xmlNewProp(pNode, toConstXmlCharPtr("target"), toConstXmlCharPtr(mTarget));
    }
}

SedChange::SedChange(Impl *pPimpl)
    : SedBase(pPimpl)
{
}

SedChange::Impl *SedChange::pimpl()
{
    return static_cast<Impl *>(SedBase::pimpl());
}

const SedChange::Impl *SedChange::pimpl() const
{
    return static_cast<const Impl *>(SedBase::pimpl());
}

std::string SedChange::target() const
{
    return pimpl()->target();
}

} // namespace libOpenCOR
