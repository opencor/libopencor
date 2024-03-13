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
#include "sedtask_p.h"
#include "utils.h"

namespace libOpenCOR {

SedTask::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedAbstractTask::Impl(pDocument)
{
}

void SedTask::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("task"));

    SedAbstractTask::Impl::serialise(node);

    xmlAddChild(pNode, node);
}

SedTask::SedTask(const SedDocumentPtr &pDocument)
    : SedAbstractTask(new Impl(pDocument))
{
}

SedTask::~SedTask()
{
    delete pimpl();
}

SedTask::Impl *SedTask::pimpl()
{
    return reinterpret_cast<SedTask::Impl *>(SedAbstractTask::pimpl());
}

/*---GRY---
const SedTask::Impl *SedTask::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedAbstractTask::pimpl());
}
*/

SedTaskPtr SedTask::create(const SedDocumentPtr &pDocument)
{
    return SedTaskPtr {new SedTask {pDocument}};
}

} // namespace libOpenCOR
