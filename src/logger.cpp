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

#include "libopencor/logger.h"

#include "logger_p.h"

namespace libOpenCOR {

void Logger::Impl::addIssue(const IssuePtr &issue)
{
    mIssues.push_back(issue);
}

void Logger::Impl::removeAllIssues()
{
    mIssues.clear();
}

Logger::Logger()
    : mPimpl(std::make_unique<Impl>())
{
}

size_t Logger::issueCount() const
{
    return mPimpl->mIssues.size();
}

IssuePtr Logger::issue(size_t pIndex) const
{
    if (pIndex < mPimpl->mIssues.size()) {
        return mPimpl->mIssues.at(pIndex);
    }

    return {};
}

} // namespace libOpenCOR
