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

Issue::Impl::Impl(Type pType, const std::string &pDescription)
    : mType(pType)
    , mDescription(std::move(pDescription))
{
}

Issue::Issue(Type pType, const std::string &pDescription)
    : mPimpl(new Impl {pType, pDescription})
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

std::string Issue::description() const
{
    return mPimpl->mDescription;
}

} // namespace libOpenCOR
