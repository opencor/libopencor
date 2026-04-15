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
#include "logger_p.h"

namespace libOpenCOR {

bool Logger::Impl::hasIssues() const
{
    return !mIssues.empty();
}

size_t Logger::Impl::issueCount() const
{
    return mIssues.size();
}

const IssuePtrs &Logger::Impl::issues() const
{
    return mIssues;
}

const IssuePtr &Logger::Impl::issue(size_t pIndex) const
{
    static const IssuePtr NO_ISSUE_PTR;

    if (pIndex >= mIssues.size()) {
        return NO_ISSUE_PTR;
    }

    return mIssues[pIndex];
}

bool Logger::Impl::hasErrors() const
{
    return !mErrors.empty();
}

size_t Logger::Impl::errorCount() const
{
    return mErrors.size();
}

const IssuePtrs &Logger::Impl::errors() const
{
    return mErrors;
}

const IssuePtr &Logger::Impl::error(size_t pIndex) const
{
    static const IssuePtr NO_ISSUE_PTR;

    if (pIndex >= mErrors.size()) {
        return NO_ISSUE_PTR;
    }

    return mErrors[pIndex];
}

bool Logger::Impl::hasWarnings() const
{
    return !mWarnings.empty();
}

size_t Logger::Impl::warningCount() const
{
    return mWarnings.size();
}

const IssuePtrs &Logger::Impl::warnings() const
{
    return mWarnings;
}

const IssuePtr &Logger::Impl::warning(size_t pIndex) const
{
    static const IssuePtr NO_ISSUE_PTR;

    if (pIndex >= mWarnings.size()) {
        return NO_ISSUE_PTR;
    }

    return mWarnings[pIndex];
}

void Logger::Impl::addIssues(const LoggerPtr &pLogger, const std::string &pContext)
{
    for (const auto &issue : pLogger->issues()) {
        addIssue(issue->type(), issue->mPimpl->mDescription,
                 issue->mPimpl->mContext.empty() ? pContext : pContext + " | " + issue->mPimpl->mContext);
    }
}

void Logger::Impl::addIssues(const libcellml::LoggerPtr &pLogger, const std::string &pContext)
{
    for (size_t i {0}; i < pLogger->issueCount(); ++i) {
        auto issue {pLogger->issue(i)};

        addIssue((issue->level() == libcellml::Issue::Level::ERROR) ? Issue::Type::ERROR :
                                                                      Issue::Type::WARNING,
                 issue->description(), pContext);
    }
}

void Logger::Impl::addIssue(Issue::Type pType, const std::string &pDescription, const std::string &pContext)
{
    auto issue {IssuePtr {new Issue {pType, pDescription, pContext}}};
    mIssues.push_back(issue);

    if (pType == Issue::Type::ERROR) {
        mErrors.push_back(std::move(issue));
    } else {
        mWarnings.push_back(std::move(issue));
    }
}

void Logger::Impl::addError(const std::string &pDescription)
{
    addIssue(Issue::Type::ERROR, pDescription);
}

void Logger::Impl::addWarning(const std::string &pDescription)
{
    addIssue(Issue::Type::WARNING, pDescription);
}

void Logger::Impl::removeAllIssues()
{
    mIssues.clear();

    mErrors.clear();
    mWarnings.clear();
}

Logger::Logger(Impl *pPimpl)
    : mPimpl(pPimpl)
{
}

bool Logger::hasIssues() const
{
    return mPimpl->hasIssues();
}

size_t Logger::issueCount() const
{
    return mPimpl->issueCount();
}

const IssuePtrs &Logger::issues() const
{
    return mPimpl->issues();
}

const IssuePtr &Logger::issue(size_t pIndex) const
{
    return mPimpl->issue(pIndex);
}

bool Logger::hasErrors() const
{
    return mPimpl->hasErrors();
}

size_t Logger::errorCount() const
{
    return mPimpl->errorCount();
}

const IssuePtrs &Logger::errors() const
{
    return mPimpl->errors();
}

const IssuePtr &Logger::error(size_t pIndex) const
{
    return mPimpl->error(pIndex);
}

bool Logger::hasWarnings() const
{
    return mPimpl->hasWarnings();
}

size_t Logger::warningCount() const
{
    return mPimpl->warningCount();
}

const IssuePtrs &Logger::warnings() const
{
    return mPimpl->warnings();
}

const IssuePtr &Logger::warning(size_t pIndex) const
{
    return mPimpl->warning(pIndex);
}

} // namespace libOpenCOR
