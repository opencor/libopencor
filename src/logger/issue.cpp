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

#include "issue_p.h"

namespace libOpenCOR {

Issue::Impl::Impl(Type pType, const std::string &pDescription, const std::string &pContext)
    : mType(pType)
    , mDescription(pDescription)
    , mContext(pContext)
{
    // Check whether we have some context to add to the description.

    if (!mContext.empty()) {
        mDescriptionWithContext.reserve(mContext.size() + 2 + mDescription.size()); // NOLINT

        mDescriptionWithContext.insert(0, mDescription);

#ifndef CODE_COVERAGE_ENABLED
        if (std::isupper(mDescriptionWithContext[0]) != 0) {
#endif
            mDescriptionWithContext[0] = static_cast<char>(std::tolower(mDescriptionWithContext[0]));
#ifndef CODE_COVERAGE_ENABLED
        }
#endif

        mDescriptionWithContext.insert(0, ": ");
        mDescriptionWithContext.insert(0, mContext);
    }
}

Issue::Type Issue::Impl::type() const
{
    return mType;
}

const std::string &Issue::Impl::typeAsString() const
{
    static const std::string ERROR_STRING {"Error"};
    static const std::string WARNING_STRING {"Warning"};

    return (mType == Type::ERROR) ? ERROR_STRING : WARNING_STRING;
}

const std::string &Issue::Impl::description() const
{
    if (!mContext.empty()) {
        return mDescriptionWithContext;
    }

    return mDescription;
}

Issue::Issue(Type pType, const std::string &pDescription, const std::string &pContext)
    : mPimpl(new Impl {pType, pDescription, pContext})
{
}

Issue::~Issue()
{
    delete mPimpl;
}

Issue::Type Issue::type() const
{
    return mPimpl->type();
}

const std::string &Issue::typeAsString() const
{
    return mPimpl->typeAsString();
}

const std::string &Issue::description() const
{
    return mPimpl->description();
}

} // namespace libOpenCOR
