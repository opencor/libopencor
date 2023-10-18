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

#include "compiler.h"
#include "utils.h"

#include "tests/utils.h"

class CompilerTest: public testing::Test
{
public:
    libOpenCOR::CompilerPtr mCompiler;

    void SetUp() override
    {
        mCompiler = libOpenCOR::Compiler::create();
    }
};

TEST_F(CompilerTest, basic)
{
    // Cannot retrieve a function if nothing has been compiled.

    EXPECT_EQ(nullptr, mCompiler->function(""));

    // To compile an empty string is nonsensical, but still possible.

    EXPECT_TRUE(mCompiler->compile(""));
    EXPECT_EQ(nullptr, mCompiler->function(""));

    // Add "void" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_01 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected identifier or '(':\nvoid\n    ^"},
    };

    EXPECT_FALSE(mCompiler->compile("void"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_01);

    // Add an identifier to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_02 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Variable has incomplete type 'void':\nvoid function\n     ^"},
        {libOpenCOR::Issue::Type::ERROR, "Expected ';' after top level declarator:\nvoid function\n             ^\n             ;"},
    };

    EXPECT_FALSE(mCompiler->compile("void function"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_02);

    // Add a "(" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_03 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected parameter declarator:\nvoid function(\n              ^"},
        {libOpenCOR::Issue::Type::ERROR, "Expected ')' to match this '(':\nvoid function(\n             ^"},
        {libOpenCOR::Issue::Type::ERROR, "Expected function body after function declarator:\nvoid function(\n              ^"},
    };

    EXPECT_FALSE(mCompiler->compile("void function("));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_03);

    // Add a ")" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_04 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected function body after function declarator:\nvoid function()\n               ^"},
    };

    EXPECT_FALSE(mCompiler->compile("void function()"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_04);

    // Add a "{" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_05 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected '}':\nvoid function() {\n                 ^\nto match this '{':\nvoid function() {\n                ^"},
    };

    EXPECT_FALSE(mCompiler->compile("void function() {"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_05);

    // Add a "}" to our string, making it a valid void function.

    EXPECT_TRUE(mCompiler->compile("void function() {}"));
    EXPECT_NE(nullptr, mCompiler->function("function"));

    // Make sure that we cannot retrieve a non-existing function.

    EXPECT_EQ(nullptr, mCompiler->function("undefined"));

    // Make the function a double function.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_06 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Non-void function does not return a value:\ndouble function() {}\n                   ^"},
    };

    EXPECT_FALSE(mCompiler->compile("double function() {}"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_06);

    // Add "return" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_07 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected expression:\ndouble function() { return\n                          ^"},
        {libOpenCOR::Issue::Type::ERROR, "Expected '}' to match this '{':\ndouble function() { return\n                  ^"},
    };

    EXPECT_FALSE(mCompiler->compile("double function() { return"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_07);

    // Add "3.0" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_08 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected ';' after return statement:\ndouble function() { return 3.0\n                              ^\n                              ;"},
        {libOpenCOR::Issue::Type::ERROR, "Expected '}' to match this '{':\ndouble function() { return 3.0\n                  ^"},
    };

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_08);

    // Add ";" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_09 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected '}':\ndouble function() { return 3.0;\n                               ^\nto match this '{':\ndouble function() { return 3.0;\n                  ^"},
    };

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0;"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_09);

    // Add a "}" to our string, making it a valid double function.

    EXPECT_TRUE(mCompiler->compile("double function() { return 3.0; }"));
    EXPECT_NE(nullptr, mCompiler->function("function"));
    EXPECT_TRUE(libOpenCOR::fuzzyCompare(3.0, reinterpret_cast<double (*)()>(mCompiler->function("function"))()));

    // Use an invalid function name.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_10 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected identifier or '(':\ndouble .function() { return 3.0; }\n       ^"},
    };

    EXPECT_FALSE(mCompiler->compile("double .function() { return 3.0; }"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_10);

    // Return an invalid statement.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_11 = {
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected expression:\ndouble function() { return 3.0+*-/a; }\n                                 ^"},
        {libOpenCOR::Issue::Type::ERROR, "Use of undeclared identifier 'a':\ndouble function() { return 3.0+*-/a; }\n                                  ^"},
    };

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0+*-/a; }"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_11);
}

TEST_F(CompilerTest, severalFunctions)
{
    EXPECT_TRUE(mCompiler->compile("double function1() { return 3.0; }"
                                   "double function2() { return 5.0; }"
                                   "double function3() { return 7.0; }"));
    EXPECT_NE(nullptr, mCompiler->function("function1"));
    EXPECT_NE(nullptr, mCompiler->function("function2"));
    EXPECT_NE(nullptr, mCompiler->function("function3"));
    EXPECT_TRUE(libOpenCOR::fuzzyCompare(3.0, reinterpret_cast<double (*)()>(mCompiler->function("function1"))()));
    EXPECT_TRUE(libOpenCOR::fuzzyCompare(5.0, reinterpret_cast<double (*)()>(mCompiler->function("function2"))()));
    EXPECT_TRUE(libOpenCOR::fuzzyCompare(7.0, reinterpret_cast<double (*)()>(mCompiler->function("function3"))()));
}

TEST_F(CompilerTest, severalParameters)
{
    EXPECT_TRUE(mCompiler->compile("double function(double a, double b, double c) { return a*b*c; }"));
    EXPECT_NE(nullptr, mCompiler->function("function"));
    EXPECT_TRUE(libOpenCOR::fuzzyCompare(105.0, reinterpret_cast<double (*)(double, double, double)>(mCompiler->function("function"))(3.0, 5.0, 7.0)));
}
