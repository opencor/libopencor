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

#include "sedbase_p.h"

#include "utils.h"

namespace libOpenCOR {

SedBase::Impl::Impl(const std::string &pId)
    : mId(pId)
{
}

std::string SedBase::Impl::id() const
{
    return mId;
}

void SedBase::Impl::setId(const std::string &pId)
{
    mId = pId;
}

void SedBase::Impl::serialise(xmlNodePtr pNode) const
{
    xmlNewProp(pNode, toConstXmlCharPtr("id"), toConstXmlCharPtr(mId));
}

SedBase::SedBase(Impl *pPimpl)
    : Logger(pPimpl)
{
}

SedBase::Impl *SedBase::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl);
}

const SedBase::Impl *SedBase::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

std::string SedBase::id() const
{
    return pimpl()->id();
}

void SedBase::setId(const std::string &pId)
{
    pimpl()->setId(pId);
}

} // namespace libOpenCOR
