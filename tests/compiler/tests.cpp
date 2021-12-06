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

#include "gtest/gtest.h"

#include "compiler.h"

class CompilerTest: public testing::Test
{
protected:
    void SetUp() override
    {
        mCompiler = new libOpenCOR::Compiler();
    }

    void TearDown() override
    {
        delete mCompiler;
    }

    libOpenCOR::Compiler *mCompiler = nullptr;
};

TEST_F(CompilerTest, basic)
{
    // To compiler an empty string is nonsensical, but still allowed.

    EXPECT_TRUE(mCompiler->compile(""));
    EXPECT_EQ(nullptr, mCompiler->function(""));

    // Add "void" to our string.

    EXPECT_FALSE(mCompiler->compile("void"));

    // Add an identifier to our string.

    EXPECT_FALSE(mCompiler->compile("void function"));

    // Add a "(" to our string.

    EXPECT_FALSE(mCompiler->compile("void function("));

    // Add a ")" to our string.

    EXPECT_FALSE(mCompiler->compile("void function()"));

    // Add a "{" to our string.

    EXPECT_FALSE(mCompiler->compile("void function() {"));

    // Add a "}" to our string, making it a valid void function.

    EXPECT_TRUE(mCompiler->compile("void function() {}"));
    EXPECT_NE(nullptr, mCompiler->function("function"));

    // Make the function a double function.

    EXPECT_FALSE(mCompiler->compile("double function() {}"));

    // Add "return" to our string.

    EXPECT_FALSE(mCompiler->compile("double function() { return"));

    // Add "3.0" to our string.

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0"));

    // Add ";" to our string.

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0;"));

    // Add a "}" to our string, making it a valid double function.

    EXPECT_TRUE(mCompiler->compile("double function() { return 3.0; }"));
    EXPECT_NE(nullptr, mCompiler->function("function"));

    // Use an invalid function name.

    EXPECT_FALSE(mCompiler->compile("double .function() { return 3.0; }"));

    // Return an invalid statement.

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0*/a; }"));
}
