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

#include "sedabstracttask_p.h"

#include "seddocument_p.h"

namespace libOpenCOR {

static constexpr auto ID_PREFIX = "task";

SedAbstractTask::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedBase::Impl(pDocument->pimpl()->uniqueId(ID_PREFIX))
    , mDocument(pDocument)
{
}

void SedAbstractTask::Impl::serialise(xmlNodePtr pNode) const
{
    SedBase::Impl::serialise(pNode);
}

SedAbstractTask::SedAbstractTask(Impl *pPimpl)
    : SedBase(pPimpl)
{
}

SedAbstractTask::Impl *SedAbstractTask::pimpl()
{
    return reinterpret_cast<Impl *>(SedBase::pimpl());
}

const SedAbstractTask::Impl *SedAbstractTask::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedBase::pimpl());
}

} // namespace libOpenCOR
