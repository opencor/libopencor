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

Issue::Impl::Impl(std::string pDescription, Type pType)
    : mType(pType)
    , mDescription(std::move(pDescription))
{
}

Issue::Issue(const std::string &pDescription, Type pType)
    : mPimpl(new Impl {pDescription, pType})
{
}

Issue::~Issue()
{
    delete mPimpl;
}

Issue::Type Issue::type() const
{
    return mPimpl->mType;
}

std::string Issue::typeAsString() const
{
    switch (mPimpl->mType) {
    case Type::ERROR:
        return "Error";
    case Type::WARNING:
        return "Warning";
    default: // Type::MESSAGE.
        return "Message";
    }
}

std::string Issue::description() const
{
    return mPimpl->mDescription;
}

} // namespace libOpenCOR
