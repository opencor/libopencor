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
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return !mIssues.empty();
}

size_t Logger::Impl::issueCount() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return mIssues.size();
}

IssuePtrs Logger::Impl::issues() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return mIssues;
}

IssuePtr Logger::Impl::issue(size_t pIndex) const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    if (pIndex >= mIssues.size()) {
        return nullptr;
    }

    return mIssues[pIndex];
}

bool Logger::Impl::hasErrors() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return !mErrors.empty();
}

size_t Logger::Impl::errorCount() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return mErrors.size();
}

IssuePtrs Logger::Impl::errors() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return mErrors;
}

IssuePtr Logger::Impl::error(size_t pIndex) const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    if (pIndex >= mErrors.size()) {
        return nullptr;
    }

    return mErrors[pIndex];
}

bool Logger::Impl::hasWarnings() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return !mWarnings.empty();
}

size_t Logger::Impl::warningCount() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return mWarnings.size();
}

IssuePtrs Logger::Impl::warnings() const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    return mWarnings;
}

IssuePtr Logger::Impl::warning(size_t pIndex) const
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    if (pIndex >= mWarnings.size()) {
        return nullptr;
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
    const auto issueCount = pLogger->issueCount();

    for (size_t i {0}; i < issueCount; ++i) {
        auto issue {pLogger->issue(i)};

        if (issue != nullptr) {
            addIssue((issue->level() == libcellml::Issue::Level::ERROR) ? Issue::Type::ERROR : Issue::Type::WARNING,
                     issue->description(), pContext);
        }
    }
}

void Logger::Impl::addIssue(Issue::Type pType, const std::string &pDescription, const std::string &pContext)
{
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

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
    const std::scoped_lock<std::recursive_mutex> lock(mMutex);

    mIssues.clear();

    mErrors.clear();
    mWarnings.clear();
}

Logger::Logger(std::unique_ptr<Impl> pPimpl)
    : mPimpl(std::move(pPimpl))
{
}

Logger::~Logger() = default;

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
