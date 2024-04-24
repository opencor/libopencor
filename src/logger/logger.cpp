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

bool Logger::Impl::hasErrors() const
{
    return !mErrors.empty();
}

bool Logger::Impl::hasWarnings() const
{
    return !mWarnings.empty();
}

bool Logger::Impl::hasMessages() const
{
    return !mMessages.empty();
}

void Logger::Impl::addIssues(const LoggerPtr &pLogger)
{
    for (const auto &issue : pLogger->issues()) {
        addIssue(issue->description(), issue->type());
    }
}

void Logger::Impl::addIssues(const libcellml::LoggerPtr &pLogger)
{
    for (size_t i = 0; i < pLogger->issueCount(); ++i) {
        auto issue = pLogger->issue(i);

#ifdef CODE_COVERAGE_ENABLED //---GRY--- SHOULD BE REMOVED AT SOME POINT.
        addIssue(issue->description(),
                 (issue->level() == libcellml::Issue::Level::ERROR) ? Issue::Type::ERROR :
                                                                      Issue::Type::WARNING);
#else
        addIssue(issue->description(),
                 (issue->level() == libcellml::Issue::Level::ERROR)   ? Issue::Type::ERROR :
                 (issue->level() == libcellml::Issue::Level::WARNING) ? Issue::Type::WARNING :
                                                                        Issue::Type::MESSAGE);
#endif
    }
}

void Logger::Impl::addIssue(const std::string &pDescription, Issue::Type pType)
{
    auto issue = IssuePtr {new Issue {pDescription, pType}};
    mIssues.push_back(issue);

#ifdef CODE_COVERAGE_ENABLED //---GRY--- SHOULD BE REMOVED AT SOME POINT.
    mErrors.push_back(issue);
#else
    switch (pType) {
    case libOpenCOR::Issue::Type::ERROR:
        mErrors.push_back(issue);

        break;

    case libOpenCOR::Issue::Type::WARNING:
        mWarnings.push_back(issue);

        break;
    default: // libOpenCOR::Issue::Type::MESSAGE:
        mMessages.push_back(issue);

        break;
    }
#endif
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

void Logger::Impl::addMessage(const std::string &pDescription)
{
    addIssue(pDescription, Issue::Type::MESSAGE);
}

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
    return pimpl()->hasIssues();
}

IssuePtrVector Logger::issues() const
{
    return pimpl()->mIssues;
}

bool Logger::hasErrors() const
{
    return pimpl()->hasErrors();
}

IssuePtrVector Logger::errors() const
{
    return pimpl()->mErrors;
}

bool Logger::hasWarnings() const
{
    return pimpl()->hasWarnings();
}

IssuePtrVector Logger::warnings() const
{
    return pimpl()->mWarnings;
}

bool Logger::hasMessages() const
{
    return pimpl()->hasMessages();
}

IssuePtrVector Logger::messages() const
{
    return pimpl()->mMessages;
}

} // namespace libOpenCOR
