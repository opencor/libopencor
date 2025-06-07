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

#include "sedrepeatedtask_p.h"

namespace libOpenCOR {

/*---GRY---
SedRepeatedTask::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedAbstractTask::Impl(pDocument)
{
}
*/

/*---GRY---
bool SedRepeatedTask::Impl::isValid()
{
    return true;
}
*/

/*---GRY---
void SedRepeatedTask::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("repeatedTask"));

    SedAbstractTask::Impl::serialise(node);

    xmlAddChild(pNode, node);
}
*/

/*---GRY---
SedRepeatedTask::SedRepeatedTask(const SedDocumentPtr &pDocument)
    : SedAbstractTask(new Impl(pDocument))
{
}
*/

/*---GRY---
SedRepeatedTask::~SedRepeatedTask()
{
    delete pimpl();
}
*/

/*---GRY---
SedRepeatedTask::Impl *SedRepeatedTask::pimpl()
{
    return reinterpret_cast<SedRepeatedTask::Impl *>(SedAbstractTask::pimpl());
}
*/

/*---GRY---
const SedRepeatedTask::Impl *SedRepeatedTask::pimpl() const
{
    return static_cast<const Impl *>(SedAbstractTask::pimpl());
}
*/

/*---GRY---
SedRepeatedTaskPtr SedRepeatedTask::create(const SedDocumentPtr &pDocument)
{
    return SedRepeatedTaskPtr {new SedRepeatedTask {pDocument}};
}
*/

} // namespace libOpenCOR
