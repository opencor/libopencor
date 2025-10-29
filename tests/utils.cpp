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

#include <cmath>
#include <libopencor>
#include <regex>

namespace libOpenCOR {

void expectEqualIssues(const LoggerPtr &pLogger, const ExpectedIssues &pExpectedIssues)
{
    auto issues {pLogger->issues()};

    EXPECT_EQ(issues.size(), pExpectedIssues.size());

    for (size_t i {0}; i < issues.size(); ++i) {
        EXPECT_EQ(issues[i]->type(), pExpectedIssues[i].type);
        EXPECT_EQ(issues[i]->description(), pExpectedIssues[i].description);

        if (issues[i]->type() == Issue::Type::ERROR) {
            EXPECT_EQ(issues[i]->typeAsString(), "Error");
        } else {
            EXPECT_EQ(issues[i]->typeAsString(), "Warning");
        }
    }
}

namespace {

void expectEqualValue(double pValue, double pExpectedValue, double pAbsTol)
{
    if (std::isinf(pExpectedValue)) {
        EXPECT_TRUE(std::isinf(pValue));
    } else if (std::isnan(pExpectedValue)) {
        EXPECT_TRUE(std::isnan(pValue));
    } else {
        EXPECT_NEAR(pValue, pExpectedValue, pAbsTol);
    }
}

} // namespace

void expectEqualValues(const SedInstanceTaskPtr &pInstanceTask, size_t pIndex,
                       const Doubles &pStateValues, const Doubles &pStateAbsTols,
                       const Doubles &pRateValues, const Doubles &pRateAbsTols,
                       const Doubles &pConstantValues, const Doubles &pConstantAbsTols,
                       const Doubles &pComputedConstantValues, const Doubles &pComputedConstantAbsTols,
                       const Doubles &pAlgebraicValues, const Doubles &pAlgebraicAbsTols)
{
    for (size_t i {0}; i < pInstanceTask->stateCount(); ++i) {
        expectEqualValue(pInstanceTask->state(i)[pIndex], pStateValues[i], pStateAbsTols[i]);
    }

    for (size_t i {0}; i < pInstanceTask->rateCount(); ++i) {
        expectEqualValue(pInstanceTask->rate(i)[pIndex], pRateValues[i], pRateAbsTols[i]);
    }

    for (size_t i {0}; i < pInstanceTask->constantCount(); ++i) {
        expectEqualValue(pInstanceTask->constant(i)[pIndex], pConstantValues[i], pConstantAbsTols[i]);
    }

    for (size_t i {0}; i < pInstanceTask->computedConstantCount(); ++i) {
        expectEqualValue(pInstanceTask->computedConstant(i)[pIndex], pComputedConstantValues[i], pComputedConstantAbsTols[i]);
    }

    for (size_t i {0}; i < pInstanceTask->algebraicCount(); ++i) {
        expectEqualValue(pInstanceTask->algebraic(i)[pIndex], pAlgebraicValues[i], pAlgebraicAbsTols[i]);
    }
}

std::string resourcePath(const std::string &pResourceRelativePath)
{
    return canonicalFileName(std::string(RESOURCE_LOCATION) + "/" + pResourceRelativePath);
}

std::string textFileContents(const std::string &pFileName)
{
    static const auto CRLF_REGEX {std::regex("\\r\\n")};

    auto res {toString(fileContents(pFileName))};

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
    char *buffer {new char[modp_b64_decode_len(strlen(pContents))]};
    const size_t length {modp_b64_decode(buffer, pContents, strlen(pContents))};
    UnsignedChars res(buffer, buffer + length); // NOLINT

    delete[] buffer;

    return res;
}

} // namespace libOpenCOR
