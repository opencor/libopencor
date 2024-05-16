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

#include "utils.h"

#include "tests/utils.h"

#include <libopencor>
#include <regex>

namespace libOpenCOR {

void printIssues(const LoggerPtr &pLogger)
{
    printf("---[ISSUES]---[BEGIN]\n"); // NOLINT

    for (auto &issue : pLogger->issues()) {
        const auto *type = (issue->type() == Issue::Type::ERROR) ?
                               "ERROR" :
                           (issue->type() == Issue::Type::WARNING) ?
                               "WARNING" :
                               "MESSAGE";

        printf("%s: %s\n", type, issue->description().c_str()); // NOLINT
    }

    printf("---[ISSUES]---[END]\n"); // NOLINT
}

void expectEqualIssues(const LoggerPtr &pLogger, const ExpectedIssues &pExpectedIssues)
{
    auto issues = pLogger->issues();

    EXPECT_EQ(issues.size(), pExpectedIssues.size());

    for (size_t i = 0; i < issues.size(); ++i) {
        EXPECT_EQ(issues[i]->type(), pExpectedIssues[i].type);
        EXPECT_EQ(issues[i]->description(), pExpectedIssues[i].description);

        if (issues[i]->type() == Issue::Type::ERROR) {
            EXPECT_EQ(issues[i]->typeAsString(), "Error");
        } else if (issues[i]->type() == Issue::Type::WARNING) {
            EXPECT_EQ(issues[i]->typeAsString(), "Warning");
        } else {
            EXPECT_EQ(issues[i]->typeAsString(), "Message");
        }
    }
}

void expectEqualValues(const SedInstanceTaskPtr &pInstanceTask, size_t pIndex, const libOpenCOR::Doubles &pStateValues,
                       const libOpenCOR::Doubles &pStateAbsTols, const libOpenCOR::Doubles &pRateValues,
                       const libOpenCOR::Doubles &pRateAbsTols, const libOpenCOR::Doubles &pVariableValues,
                       const libOpenCOR::Doubles &pVariableAbsTols)
{
    for (size_t i = 0; i < pInstanceTask->stateCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->state(i)[pIndex], pStateValues[i], pStateAbsTols[i]);
    }

    for (size_t i = 0; i < pInstanceTask->rateCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->rate(i)[pIndex], pRateValues[i], pRateAbsTols[i]);
    }

    for (size_t i = 0; i < pInstanceTask->variableCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->variable(i)[pIndex], pVariableValues[i], pVariableAbsTols[i]);
    }
}

std::string resourcePath(const std::string &pResourceRelativePath)
{
    return std::string(RESOURCE_LOCATION) + "/" + pResourceRelativePath;
}

std::string textFileContents(const std::string &pFileName)
{
    static const std::regex CR_LF("\\r\\n");

    auto res = libOpenCOR::toString(libOpenCOR::fileContents(libOpenCOR::resourcePath(pFileName)));

    // To retrieve a file contents as bytes will, on Windows, result in LF characters being converted to CR+LF, so
    // convert them back since we expect LF.

    return regex_replace(res, CR_LF, "\n");
}

UnsignedChars charArrayToUnsignedChars(const char *pContents)
{
    return {pContents, pContents + strlen(pContents)}; // NOLINT
}

} // namespace libOpenCOR
