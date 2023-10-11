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

#include "gtest/gtest.h"

#include "tests/utils.h"

TEST(CoverageUtilsTest, toString)
{
    EXPECT_EQ(libOpenCOR::toString(false), "False");
}

TEST(CoverageUtilsTest, toDouble)
{
    bool ok = true;

    libOpenCOR::toDouble("0.123 0.456", ok);

    EXPECT_FALSE(ok);
}

TEST(CoverageUtilsTest, toInt)
{
    bool ok = true;

    libOpenCOR::toInt("123 456", ok);

    EXPECT_FALSE(ok);
}

TEST(CoverageUtilsTest, toSizeT)
{
    bool ok = true;

    libOpenCOR::toSizeT("abc", ok);

    EXPECT_FALSE(ok);

    libOpenCOR::toSizeT("123", ok);

    EXPECT_TRUE(ok);

    libOpenCOR::toSizeT("123 456", ok);

    EXPECT_FALSE(ok);
}
