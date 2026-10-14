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
#include "seddocument_p.h"

#include "utils.h"

namespace libOpenCOR {

SedBase::Impl::Impl(const std::string &pId)
    : mId(pId)
{
}

SedBase::Impl::Impl(const SedDocumentPtr &pDocument, const std::string &pIdPrefix)
    : mId(pDocument->pimpl()->uniqueId(pIdPrefix))
    , mDocument(pDocument)
    , mHasReservedId(true)
{
}

const std::string &SedBase::Impl::id() const
{
    return mId;
}

bool SedBase::Impl::setId(const std::string &pId)
{
    // Make sure that the id is not already used by another object of our document, if we still have one, and if our
    // current id is reserved in our document (i.e. we have not been removed from it) then reserve the new id instead.

    if (pId == mId) {
        return true;
    }

    auto document {mDocument.lock()};

    if (document != nullptr) {
        auto &ids {document->pimpl()->mIds};

        if (ids.contains(pId)) {
            return false;
        }

        if (mHasReservedId) {
            ids.erase(mId);
            ids.insert(pId);
        }
    }

    mId = pId;

    return true;
}

void SedBase::Impl::serialise(xmlNodePtr pNode) const
{
    xmlNewProp(pNode, toConstXmlCharPtr("id"), toConstXmlCharPtr(mId));
}

SedBase::SedBase(std::unique_ptr<Impl> pPimpl)
    : Logger(std::move(pPimpl))
{
}

SedBase::Impl *SedBase::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl.get());
}

const SedBase::Impl *SedBase::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl.get());
}

const std::string &SedBase::id() const
{
    return pimpl()->id();
}

bool SedBase::isValidId(const std::string &pId)
{
    return isSedmlId(pId);
}

bool SedBase::setId(const std::string &pId)
{
    if (!isValidId(pId)) {
        return false;
    }

    return pimpl()->setId(pId);
}

} // namespace libOpenCOR
