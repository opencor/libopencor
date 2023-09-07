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

#include "tests/utils.h"

#include "gtest/gtest.h"

#include "libopencor/logger.h"

#include <cstring>

namespace libOpenCOR {

void expectEqualIssues(const LoggerPtr &pLogger, const ExpectedIssues &pExpectedIssues)
{
    EXPECT_EQ(pLogger->issueCount(), pExpectedIssues.size());

    for (size_t i = 0; i < pLogger->issueCount(); ++i) {
        EXPECT_EQ(pLogger->issue(i)->type(), pExpectedIssues[i].type);
        EXPECT_EQ(pLogger->issue(i)->description(), pExpectedIssues[i].description);
    }
}

void expectEqualStrings(const std::vector<std::string> &pStrings, const std::vector<std::string> &pExpectedStrings)
{
    EXPECT_EQ(pStrings.size(), pExpectedStrings.size());

    for (size_t i = 0; i < pStrings.size(); ++i) {
        EXPECT_EQ(pStrings[i], pExpectedStrings[i]);
    }
}

std::string resourcePath(const std::string &pResourceRelativePath)
{
    return std::string(RESOURCE_LOCATION) + "/" + pResourceRelativePath;
}

#ifdef BUILDING_USING_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
std::vector<unsigned char> charArrayToVector(const char *pContents)
#ifdef BUILDING_USING_CLANG
#    pragma clang diagnostic pop
#endif
{
    return {pContents, pContents + strlen(pContents)}; // NOLINT
}

} // namespace libOpenCOR
