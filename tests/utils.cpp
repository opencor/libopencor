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

#include "../extern/modp_b64/modp_b64.h"

#include <libopencor>
#include <regex>

namespace libOpenCOR {

void printIssues(const LoggerPtr &pLogger)
{
    printf("---[ISSUES]---[BEGIN]\n"); // NOLINT

    for (auto &issue : pLogger->issues()) {
        const auto *type = (issue->type() == Issue::Type::ERROR) ?
                               "ERROR" :
                               "WARNING";

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
        } else {
            EXPECT_EQ(issues[i]->typeAsString(), "Warning");
        }
    }
}

void expectEqualValues(const SedInstanceTaskPtr &pInstanceTask, size_t pIndex,
                       const libOpenCOR::Doubles &pStateValues, const libOpenCOR::Doubles &pStateAbsTols,
                       const libOpenCOR::Doubles &pRateValues, const libOpenCOR::Doubles &pRateAbsTols,
                       const libOpenCOR::Doubles &pConstantValues, const libOpenCOR::Doubles &pConstantAbsTols,
                       const libOpenCOR::Doubles &pComputedConstantValues, const libOpenCOR::Doubles &pComputedConstantAbsTols,
                       const libOpenCOR::Doubles &pAlgebraicValues, const libOpenCOR::Doubles &pAlgebraicAbsTols)
{
    for (size_t i = 0; i < pInstanceTask->stateCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->state(i)[pIndex], pStateValues[i], pStateAbsTols[i]);
    }

    for (size_t i = 0; i < pInstanceTask->rateCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->rate(i)[pIndex], pRateValues[i], pRateAbsTols[i]);
    }

    for (size_t i = 0; i < pInstanceTask->constantCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->constant(i)[pIndex], pConstantValues[i], pConstantAbsTols[i]);
    }

    for (size_t i = 0; i < pInstanceTask->computedConstantCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->computedConstant(i)[pIndex], pComputedConstantValues[i], pComputedConstantAbsTols[i]);
    }

    for (size_t i = 0; i < pInstanceTask->algebraicCount(); ++i) {
        EXPECT_NEAR(pInstanceTask->algebraic(i)[pIndex], pAlgebraicValues[i], pAlgebraicAbsTols[i]);
    }
}

std::string resourcePath(const std::string &pResourceRelativePath)
{
    return std::string(RESOURCE_LOCATION) + "/" + pResourceRelativePath;
}

std::string textFileContents(const std::string &pFileName)
{
    static const auto CRLF_REGEX = std::regex("\\r\\n");

    auto res = libOpenCOR::toString(libOpenCOR::fileContents(pFileName));

    // To retrieve a file contents as bytes will, on Windows, result in LF characters being converted to CR+LF, so
    // convert them back since we expect LF.

    return regex_replace(res, CRLF_REGEX, "\n");
}

UnsignedChars charArrayToUnsignedChars(const char *pContents)
{
    return {pContents, pContents + strlen(pContents)}; // NOLINT
}

UnsignedChars base64Decode(const char *pContents)
{
    char *buffer = new char[modp_b64_decode_len(strlen(pContents))];
    const size_t length = modp_b64_decode(buffer, pContents, strlen(pContents));
    UnsignedChars res(buffer, buffer + length); // NOLINT

    delete[] buffer;

    return res;
}

} // namespace libOpenCOR
