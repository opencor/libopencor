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
        auto type = (issue->type() == Issue::Type::ERROR) ?
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
    }
}

void expectEqualDoubles(double *pDoubles, const Doubles &pExpectedDoubles, const Doubles &pAbsoluteErrors)
{
    for (size_t i = 0; i < pExpectedDoubles.size(); ++i) {
        EXPECT_NEAR(pDoubles[i], pExpectedDoubles[i], pAbsoluteErrors[i]); // NOLINT
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

UnsignedCharVector charArrayToVector(const char *pContents)
{
    return {pContents, pContents + strlen(pContents)}; // NOLINT
}

} // namespace libOpenCOR
