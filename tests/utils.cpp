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

namespace libOpenCOR {

void expectEqualIssues(const LoggerPtr &pLogger, const ExpectedIssues &pExpectedIssues)
{
    auto issues = pLogger->issues();

    EXPECT_EQ(issues.size(), pExpectedIssues.size());

    for (size_t i = 0; i < issues.size(); ++i) {
        EXPECT_EQ(issues[i]->type(), pExpectedIssues[i].type);
        EXPECT_EQ(issues[i]->description(), pExpectedIssues[i].description);
    }
}

void expectEqualDoubles(double *pDoubles, const Doubles &pExpectedDoubles, const Doubles &pAbsoluteErrors)
{
    for (size_t i = 0; i < pExpectedDoubles.size(); ++i) {
        EXPECT_NEAR(pDoubles[i], pExpectedDoubles[i], pAbsoluteErrors[i]); // NOLINT
    }
}

void expectEqualHiddenProperties(const StringVector &pHiddenProperties, const StringVector &pExpectedHiddenProperties)
{
    EXPECT_EQ(pHiddenProperties.size(), pExpectedHiddenProperties.size());

    for (size_t i = 0; i < pHiddenProperties.size(); ++i) {
        EXPECT_EQ(pHiddenProperties[i], pExpectedHiddenProperties[i]);
    }
}

std::string resourcePath(const std::string &pResourceRelativePath)
{
    return std::string(RESOURCE_LOCATION) + "/" + pResourceRelativePath;
}

UnsignedCharVector charArrayToVector(const char *pContents)
{
    return {pContents, pContents + strlen(pContents)}; // NOLINT
}

} // namespace libOpenCOR
