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

SedBase::Impl::~Impl()
{
    // Release our id if it is reserved in our document, if we still have one, so that it can be given to another object
    // (e.g., if we were created for our document, but never added to it).

    if (mHasReservedId) {
        auto document {mDocument.lock()};

        if (document != nullptr) {
            document->pimpl()->mIds.erase(mId);
        }
    }
}

bool SedBase::Impl::isDuplicateId(const std::string &pId) const
{
    // Check whether the given id is used by another object of our document, if we still have one. Our own id is not a
    // duplicate unless it is not reserved in our document (i.e. we have been removed from it) and another object has
    // since been given it.

    auto document {mDocument.lock()};

    return (document != nullptr) && document->pimpl()->mIds.contains(pId) && ((pId != mId) || !mHasReservedId);
}

const std::string &SedBase::Impl::id() const
{
    return mId;
}

bool SedBase::Impl::setId(const std::string &pId)
{
    // Make sure that the id is not already used by another object of our document and if our current id is reserved in
    // our document, if we still have one, then reserve the new id instead.

    if (isDuplicateId(pId)) {
        return false;
    }

    if (mHasReservedId) {
        auto document {mDocument.lock()};

        if (document != nullptr) {
            auto &ids {document->pimpl()->mIds};

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

bool SedBase::isValidId(const std::string &pId)
{
    return isSedmlId(pId);
}

bool SedBase::isDuplicateId(const std::string &pId) const
{
    return pimpl()->isDuplicateId(pId);
}

const std::string &SedBase::id() const
{
    return pimpl()->id();
}

bool SedBase::setId(const std::string &pId)
{
    if (!isValidId(pId)) {
        return false;
    }

    return pimpl()->setId(pId);
}

} // namespace libOpenCOR
