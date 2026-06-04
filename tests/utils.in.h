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

#pragma once

#define GTEST_DONT_DEFINE_ASSERT_EQ 1
#define GTEST_DONT_DEFINE_ASSERT_NE 1
#define GTEST_DONT_DEFINE_ASSERT_GT 1
#define GTEST_DONT_DEFINE_ASSERT_GE 1
#define GTEST_DONT_DEFINE_ASSERT_LT 1
#define GTEST_DONT_DEFINE_ASSERT_LE 1

#include "gtest/gtest.h"

#include "libopencor/issue.h"
#include "libopencor/types.h"

namespace libOpenCOR {

static constexpr auto RESOURCE_LOCATION {"@RESOURCE_LOCATION@"};

static constexpr auto REMOTE_BASE_PATH {"https://raw.githubusercontent.com/opencor/libopencor/master/tests/res"};
static constexpr auto REMOTE_FILE {"https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"};

typedef struct
{
    Issue::Type type;
    std::string description;
} ExpectedIssue;

using ExpectedIssues = std::vector<ExpectedIssue>;

void expectEqualIssues(const LoggerPtr &pLogger, const ExpectedIssues &pExpectedIssues);

#define EXPECT_EQ_ISSUES(pLogger, pExpectedIssues) \
    SCOPED_TRACE("Issue occurred here."); \
    expectEqualIssues(pLogger, pExpectedIssues)

void expectEqualValues(const SedInstanceTaskPtr &pInstanceTask, size_t pIndex,
                       const Doubles &pStateValues, const Doubles &pStateAbsTols,
                       const Doubles &pRateValues, const Doubles &pRateAbsTols,
                       const Doubles &pConstantValues, const Doubles &pConstantAbsTols,
                       const Doubles &pComputedConstantValues, const Doubles &pComputedConstantAbsTols,
                       const Doubles &pAlgebraicValues, const Doubles &pAlgebraicAbsTols);

#define EXPECT_EQ_VALUES(pInstanceTask, pIndex, pStateValues, pStateAbsTols, pRateValues, pRateAbsTols, pConstantValues, pConstantAbsTols, pComputedConstantValues, pComputedConstantAbsTols, pAlgebraicValues, pAlgebraicAbsTols) \
    SCOPED_TRACE("Issue occurred here."); \
    expectEqualValues(pInstanceTask, pIndex, pStateValues, pStateAbsTols, pRateValues, pRateAbsTols, pConstantValues, pConstantAbsTols, pComputedConstantValues, pComputedConstantAbsTols, pAlgebraicValues, pAlgebraicAbsTols)

std::string resourcePath(const std::string &pResourceRelativePath = {});

std::string textFileContents(const std::string &pFileName);
UnsignedChars charArrayToUnsignedChars(const char *pContents);

} // namespace libOpenCOR
