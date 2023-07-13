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

#include "issue_p.h"
#include "logger_p.h"

namespace libOpenCOR {

/*---GRY---
void Logger::Impl::addIssue(const IssuePtr &pIssue)
{
    addIssue(pIssue->description(), pIssue->type());
}
*/

void Logger::Impl::addIssue(const std::string &pDescription, Issue::Type pType)
{
    auto index = mIssues.size();

    mIssues.push_back(std::shared_ptr<Issue> {new Issue {pDescription, pType}});

    mErrors.push_back(index);
    /*---GRY---
        switch (pType) {
        case libOpenCOR::Issue::Type::ERROR:
            mErrors.push_back(index);

            break;

        case libOpenCOR::Issue::Type::WARNING:
            mWarnings.push_back(index);

            break;
        default: // libOpenCOR::Issue::Type::MESSAGE.
            mMessages.push_back(index);

            break;
        }
    */
}

void Logger::Impl::addError(const std::string &pDescription)
{
    addIssue(pDescription, Issue::Type::ERROR);
}

/*---GRY---
void Logger::Impl::addWarning(const std::string &pDescription)
{
    addIssue(pDescription, Issue::Type::WARNING);
}
*/

/*---GRY---
void Logger::Impl::addMessage(const std::string &pDescription)
{
    addIssue(pDescription, Issue::Type::MESSAGE);
}
*/

void Logger::Impl::removeAllIssues()
{
    mIssues.clear();

    mErrors.clear();
    mWarnings.clear();
    mMessages.clear();
}

Logger::Logger(Impl *pPimpl)
    : mPimpl(pPimpl)
{
}

Logger::Impl *Logger::pimpl()
{
    return mPimpl;
}

const Logger::Impl *Logger::pimpl() const
{
    return mPimpl;
}

bool Logger::hasIssues() const
{
    return !pimpl()->mIssues.empty();
}

size_t Logger::issueCount() const
{
    return pimpl()->mIssues.size();
}

IssuePtr Logger::issue(size_t pIndex) const
{
    if (pIndex < pimpl()->mIssues.size()) {
        return pimpl()->mIssues.at(pIndex);
    }

    return {};
}

bool Logger::hasErrors() const
{
    return !pimpl()->mErrors.empty();
}

size_t Logger::errorCount() const
{
    return pimpl()->mErrors.size();
}

IssuePtr Logger::error(size_t pIndex) const
{
    if (pIndex < pimpl()->mErrors.size()) {
        return pimpl()->mIssues.at(pimpl()->mErrors.at(pIndex));
    }

    return {};
}

/*---GRY---
bool Logger::hasWarnings() const
{
    return !pimpl()->mWarnings.empty();
}
*/

/*---GRY---
size_t Logger::warningCount() const
{
    return pimpl()->mWarnings.size();
}
*/

/*---GRY---
IssuePtr Logger::warning(size_t pIndex) const
{
    if (pIndex < pimpl()->mWarnings.size()) {
        return pimpl()->mIssues.at(pimpl()->mWarnings.at(pIndex));
    }

    return {};
}
*/

/*---GRY---
bool Logger::hasMessages() const
{
    return !pimpl()->mMessages.empty();
}
*/

/*---GRY---
size_t Logger::messageCount() const
{
    return pimpl()->mMessages.size();
}
*/

/*---GRY---
IssuePtr Logger::message(size_t pIndex) const
{
    if (pIndex < pimpl()->mMessages.size()) {
        return pimpl()->mIssues.at(pimpl()->mMessages.at(pIndex));
    }

    return {};
}
*/

} // namespace libOpenCOR
