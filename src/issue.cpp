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

#include "libopencor/issue.h"

#include "issue_p.h"

namespace libOpenCOR {

IssuePtr Issue::Impl::create()
{
    return std::shared_ptr<Issue> {new Issue {}};
}

void Issue::Impl::setType(Issue::Type pType)
{
    mType = pType;
}

void Issue::Impl::setDescription(const std::string &pDescription)
{
    mDescription = pDescription;
}

Issue::Issue()
    : mPimpl(new Impl())
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
