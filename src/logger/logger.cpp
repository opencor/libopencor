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

IssuePtrs Logger::Impl::issues() const
{
    return mIssues;
}

IssuePtr Logger::Impl::issue(size_t pIndex) const
{
    if (pIndex >= mIssues.size()) {
        return {};
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

IssuePtrs Logger::Impl::errors() const
{
    return mErrors;
}

IssuePtr Logger::Impl::error(size_t pIndex) const
{
    if (pIndex >= mErrors.size()) {
        return {};
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

IssuePtrs Logger::Impl::warnings() const
{
    return mWarnings;
}

IssuePtr Logger::Impl::warning(size_t pIndex) const
{
    if (pIndex >= mWarnings.size()) {
        return {};
    }

    return mWarnings[pIndex];
}

void Logger::Impl::addIssues(const LoggerPtr &pLogger)
{
    for (const auto &issue : pLogger->issues()) {
        addIssue(issue->type(), issue->description());
    }
}

void Logger::Impl::addIssues(const libcellml::LoggerPtr &pLogger)
{
    for (size_t i {0}; i < pLogger->issueCount(); ++i) {
        auto issue {pLogger->issue(i)};

        addIssue((issue->level() == libcellml::Issue::Level::ERROR) ? Issue::Type::ERROR :
                                                                      Issue::Type::WARNING,
                 issue->description());
    }
}

void Logger::Impl::addIssue(Issue::Type pType, const std::string &pDescription)
{
    auto issue {IssuePtr {new Issue {pType, pDescription}}};
    mIssues.push_back(issue);

    if (pType == Issue::Type::ERROR) {
        mErrors.push_back(issue);
    } else {
        mWarnings.push_back(issue);
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

IssuePtrs Logger::issues() const
{
    return mPimpl->issues();
}

IssuePtr Logger::issue(size_t pIndex) const
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

IssuePtrs Logger::errors() const
{
    return mPimpl->errors();
}

IssuePtr Logger::error(size_t pIndex) const
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

IssuePtrs Logger::warnings() const
{
    return mPimpl->warnings();
}

IssuePtr Logger::warning(size_t pIndex) const
{
    return mPimpl->warning(pIndex);
}

} // namespace libOpenCOR
