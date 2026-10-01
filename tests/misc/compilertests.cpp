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

#include "cellmlfile.h"
#include "compiler.h"
#include "irgenerator.h"
#include "utils.h"

#include "tests/utils.h"

#include <libopencor>

#include <array>
#include <bit>
#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace {

class CompilerTest: public testing::Test
{
protected:
    libOpenCOR::CompilerPtr mCompiler;

    void SetUp() override
    {
        mCompiler = libOpenCOR::Compiler::create();
    }
};

} // namespace

TEST_F(CompilerTest, basic)
{
    // Add "void" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_01 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected identifier or '(':\n    1 | void\n      |     ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("void"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_01);

    // Add an identifier to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_02 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Only functions and typedefs can be declared at the top level:\n    1 | void function\n      |      ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("void function"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_02);

    // Add a "(" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_03 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected a type specifier:\n    1 | void function(\n      |               ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("void function("));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_03);

    // Add a ")" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_04 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected function body after function declarator:\n    1 | void function()\n      |                ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("void function()"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_04);

    // Add a "{" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_05 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected '}':\n    1 | void function() {\n      |                  ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("void function() {"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_05);

    // Add a "}" to our string, making it a valid void function.

    EXPECT_TRUE(mCompiler->compile("void function() {}"));
    EXPECT_NE(nullptr, mCompiler->function("function"));

    // Make sure that we cannot retrieve a non-existing function.

    EXPECT_EQ(nullptr, mCompiler->function("undefined"));

    // Make the function a double function.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_06 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Non-void function does not return a value:\n    1 | double function() {}\n      |                    ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("double function() {}"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_06);

    // Add "return" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_07 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected expression:\n    1 | double function() { return\n      |                           ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("double function() { return"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_07);

    // Add "3.0" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_08 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected ';' after return statement:\n    1 | double function() { return 3.0\n      |                               ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_08);

    // Add ";" to our string.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_09 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected '}':\n    1 | double function() { return 3.0;\n      |                                ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("double function() { return 3.0;"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_09);

    // Add a "}" to our string, making it a valid double function.

    EXPECT_TRUE(mCompiler->compile("double function() { return 3.0; }"));
    EXPECT_NE(nullptr, mCompiler->function("function"));
    EXPECT_TRUE(libOpenCOR::fuzzyCompare(3.0, reinterpret_cast<double (*)()>(mCompiler->function("function"))()));

    // Use an invalid function name.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_10 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Unexpected character '.':\n    1 | double .function() { return 3.0; }\n      |        ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("double .function() { return 3.0; }"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_10);

    // Return an invalid statement.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_11 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Expected expression:\n    1 | double function() { return 3.0+*-/a; }\n      |                                ^"},
    }};

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

TEST_F(CompilerTest, math)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("misc/math.cellml"))};
    auto cellmlFile {libOpenCOR::CellmlFile::create(file)};
    auto cellmlFileRuntime {cellmlFile->runtime()};

    EXPECT_FALSE(cellmlFileRuntime->hasIssues());
}

namespace {

using UnsignedLong = unsigned long; // NOLINT
using UnsignedLongLong = unsigned long long; // NOLINT
using NullaryFunction = double (*)();
using UnaryFunction = double (*)(double);
using BinaryFunction = double (*)(double, double);
using TernaryFunction = double (*)(double, double, double);

template<typename T>
T function(const libOpenCOR::CompilerPtr &pCompiler, const char *pName)
{
    auto *res {pCompiler->function(pName)};

    EXPECT_NE(nullptr, res) << pName;

    return reinterpret_cast<T>(res); // NOLINT
}

bool sameDouble(double pExpected, double pActual)
{
    // Whether the given doubles are the same, considering that all NaNs are the same.

    return (std::isnan(pExpected) && std::isnan(pActual)) || (std::bit_cast<uint64_t>(pExpected) == std::bit_cast<uint64_t>(pActual));
}

bool closeDouble(double pExpected, double pActual)
{
    // Whether the given doubles are the same or close enough (e.g., because of a floating-point contraction).

    static constexpr double TOLERANCE {1.0e-12};

    return sameDouble(pExpected, pActual)
           || libOpenCOR::fuzzyCompare(pExpected, pActual)
           || (std::fabs(pExpected - pActual) < TOLERANCE);
}

} // namespace

TEST_F(CompilerTest, arithmeticOperators)
{
    ASSERT_TRUE(mCompiler->compile(R"(double add(double a, double b) { return a+b; }
double sub(double a, double b) { return a-b; }
double mul(double a, double b) { return a*b; }
double div(double a, double b) { return a/b; }
double neg(double a) { return -a; }
double precedence(double a, double b, double c) { return a+b*c-a/b-c; }
double associativity(double a, double b, double c) { return a-b-c+a/b/c; }
double parentheses(double a, double b, double c) { return (a+b)*(c-(a-b)); }
double integers() { return 7/2+(7/2.0)-(3-2)*4+(-(1+1)); }
)"));

    static const std::vector<double> VALUES {-2.5, -1.0, -0.0, 0.0, 0.5, 3.0, 1.0e300};

    for (auto a : VALUES) {
        EXPECT_TRUE(sameDouble(-a, function<UnaryFunction>(mCompiler, "neg")(a)));

        for (auto b : VALUES) {
            EXPECT_TRUE(sameDouble(a + b, function<BinaryFunction>(mCompiler, "add")(a, b)));
            EXPECT_TRUE(sameDouble(a - b, function<BinaryFunction>(mCompiler, "sub")(a, b)));
            EXPECT_TRUE(sameDouble(a * b, function<BinaryFunction>(mCompiler, "mul")(a, b)));
            EXPECT_TRUE(sameDouble(a / b, function<BinaryFunction>(mCompiler, "div")(a, b)));

            for (auto c : VALUES) {
                EXPECT_TRUE(closeDouble(a + b * c - a / b - c, function<TernaryFunction>(mCompiler, "precedence")(a, b, c)));
                EXPECT_TRUE(closeDouble(a - b - c + a / b / c, function<TernaryFunction>(mCompiler, "associativity")(a, b, c)));
                EXPECT_TRUE(closeDouble((a + b) * (c - (a - b)), function<TernaryFunction>(mCompiler, "parentheses")(a, b, c)));
            }
        }
    }

    EXPECT_EQ(0.5, function<NullaryFunction>(mCompiler, "integers")()); // I.e. 3 + 3.5 - 4 - 2.
}

TEST_F(CompilerTest, contractions)
{
    // Multiplications followed by an addition or a subtraction are contracted into fmuladd() calls (as Clang does with
    // -ffp-contract=on) unless the result of the multiplication is used for something else.

    ASSERT_TRUE(mCompiler->compile(R"(double f1(double a, double b, double c) { return a*b+c; }
double f2(double a, double b, double c) { return c+a*b; }
double f3(double a, double b, double c) { return a*b-c; }
double f4(double a, double b, double c) { return c-a*b; }
double f5(double a, double b, double c) { return -(a*b)+c; }
double f6(double a, double b, double c) { return c+-(a*b); }
double f7(double a, double b, double c) { return c-(-(a*b)); }
double f8(double a, double b, double c) { return -(a+b)+c; }
double f9(double a, double b, double c) { double x; return (x = a*b)+c; }
double f10(double a, double b, double c) { double x; return (x = -(a*b))+c; }
double f11(double a, double b, double c) { double x; return -(x = a*b)+c; }
double f12(double a, double b, double c) { return a+b+c; }
)"));

    static const std::vector<std::pair<const char *, double (*)(double, double, double)>> EXPECTED {
        {"f1", [](double a, double b, double c) {
             return std::fma(a, b, c);
         }},
        {"f2", [](double a, double b, double c) {
             return std::fma(a, b, c);
         }},
        {"f3", [](double a, double b, double c) {
             return std::fma(a, b, -c);
         }},
        {"f4", [](double a, double b, double c) {
             return std::fma(-a, b, c);
         }},
        {"f5", [](double a, double b, double c) {
             return std::fma(-a, b, c);
         }},
        {"f6", [](double a, double b, double c) {
             return std::fma(-a, b, c);
         }},
        {"f7", [](double a, double b, double c) {
             return std::fma(a, b, c);
         }},
        {"f8", [](double a, double b, double c) {
             return -(a + b) + c;
         }},
        {"f9", [](double a, double b, double c) {
             const volatile double x {a * b};
             return x + c;
         }},
        {"f10", [](double a, double b, double c) {
             const volatile double x {-(a * b)};
             return x + c;
         }},
        {"f11", [](double a, double b, double c) {
             const volatile double x {a * b};
             return -x + c;
         }},
        {"f12", [](double a, double b, double c) {
             return a + b + c;
         }},
    };
    static constexpr double A {0.1};
    static constexpr double B {10.0};
    static constexpr double C {-1.0};

    for (const auto &[name, expected] : EXPECTED) {
        EXPECT_TRUE(sameDouble(expected(A, B, C), function<TernaryFunction>(mCompiler, name)(A, B, C))) << name;
    }
}

TEST_F(CompilerTest, relationalAndLogicalOperators)
{
    ASSERT_TRUE(mCompiler->compile(R"(double eq(double a, double b) { return a == b; }
double ne(double a, double b) { return a != b; }
double lt(double a, double b) { return a < b; }
double le(double a, double b) { return a <= b; }
double gt(double a, double b) { return a > b; }
double ge(double a, double b) { return a >= b; }
double land(double a, double b) { return a && b; }
double lor(double a, double b) { return a || b; }
double lnot(double a) { return !a; }
double xor(double x, double y) { return (x != 0.0) ^ (y != 0.0); }
double combination(double a, double b) { return (a < b) + (a > b) - (a == b) * 3 / 1 + (!(a < b) && (b || a)); }
)"));

    static const std::vector<double> VALUES {-1.0, -0.0, 0.0, 2.0, std::numeric_limits<double>::quiet_NaN(),
                                             std::numeric_limits<double>::infinity()};

    for (auto a : VALUES) {
        EXPECT_EQ(static_cast<double>(a == 0.0), function<UnaryFunction>(mCompiler, "lnot")(a));

        for (auto b : VALUES) {
            // Note: we use std::equal_to and std::not_equal_to to compare doubles exactly.

            auto isEqual {std::equal_to<>()(a, b)};

            EXPECT_EQ(static_cast<double>(isEqual), function<BinaryFunction>(mCompiler, "eq")(a, b));
            EXPECT_EQ(static_cast<double>(std::not_equal_to<>()(a, b)), function<BinaryFunction>(mCompiler, "ne")(a, b));
            EXPECT_EQ(static_cast<double>(a < b), function<BinaryFunction>(mCompiler, "lt")(a, b));
            EXPECT_EQ(static_cast<double>(a <= b), function<BinaryFunction>(mCompiler, "le")(a, b));
            EXPECT_EQ(static_cast<double>(a > b), function<BinaryFunction>(mCompiler, "gt")(a, b));
            EXPECT_EQ(static_cast<double>(a >= b), function<BinaryFunction>(mCompiler, "ge")(a, b));
            EXPECT_EQ(static_cast<double>((a != 0.0) && (b != 0.0)), function<BinaryFunction>(mCompiler, "land")(a, b));
            EXPECT_EQ(static_cast<double>((a != 0.0) || (b != 0.0)), function<BinaryFunction>(mCompiler, "lor")(a, b));
            EXPECT_EQ(static_cast<double>((a != 0.0) != (b != 0.0)), function<BinaryFunction>(mCompiler, "xor")(a, b));

            auto combination {static_cast<int>(a < b)
                              + static_cast<int>(a > b)
                              - (static_cast<int>(isEqual) * 3 / 1)
                              + static_cast<int>((!(a < b)) && ((b != 0.0) || (a != 0.0)))};

            EXPECT_EQ(static_cast<double>(combination), function<BinaryFunction>(mCompiler, "combination")(a, b));
        }
    }
}

TEST_F(CompilerTest, conditionalOperator)
{
    ASSERT_TRUE(mCompiler->compile(R"(double constants(double a) { return (a > 0.0)?1.0:-2.0/4.0; }
double variables(double a, double b) { return (a > b)?a:b; }
double nested(double a, double b) { return (a > b)?(a > 0.0)?a:-a:(b > 0.0)?b:(a < b)?-b:NAN; }
double logicalConditions(double a, double b) { return (a && b)?a:(a || b)?b:(!a)?-a:(a ? b : a)?-b:a+b; }
double integers(double a) { return (a > 0.0)?1:2; }
void set(double *x, double v) { x[0] = v; }
double voidArms(double a) { double x; (a > 0.0)?set(&x, 1.0):set(&x, 2.0); return x; }
typedef struct { double x; } S;
double pointers(S *s, S *t) { return ((s ? s : t)->x) + (!s ? 1.0 : 0.0); }
)"));

    static const std::vector<double> VALUES {-1.0, 0.0, 2.0, std::numeric_limits<double>::quiet_NaN()};

    for (auto a : VALUES) {
        EXPECT_EQ((a > 0.0) ? 1.0 : -0.5, function<UnaryFunction>(mCompiler, "constants")(a));
        EXPECT_EQ((a > 0.0) ? 1.0 : 2.0, function<UnaryFunction>(mCompiler, "integers")(a));
        EXPECT_EQ((a > 0.0) ? 1.0 : 2.0, function<UnaryFunction>(mCompiler, "voidArms")(a));

        // Note: we use a volatile variable for -a since MSVC would otherwise turn (a > 0.0) ? a : -a into fabs(a), which
        //       is wrong for a = 0.0 (i.e. we would get 0.0 rather than -0.0).

        const volatile double minusA {-a};

        for (auto b : VALUES) {
            EXPECT_TRUE(sameDouble((a > b) ? a : b, function<BinaryFunction>(mCompiler, "variables")(a, b)));
            EXPECT_TRUE(sameDouble((a > b) ? ((a > 0.0) ? a : minusA) : ((b > 0.0) ? b : ((a < b) ? -b : std::numeric_limits<double>::quiet_NaN())),
                                   function<BinaryFunction>(mCompiler, "nested")(a, b)));

            auto isTrue = [](double pValue) {
                return pValue != 0.0;
            };

            EXPECT_TRUE(sameDouble((isTrue(a) && isTrue(b)) ? a : ((isTrue(a) || isTrue(b)) ? b : ((!isTrue(a)) ? -a : (isTrue(isTrue(a) ? b : a) ? -b : a + b))),
                                   function<BinaryFunction>(mCompiler, "logicalConditions")(a, b)));
        }
    }

    struct S
    {
        double x;
    };

    static constexpr double X {3.0};
    static constexpr double Y {5.0};

    S s {X};
    S t {Y};
    auto pointers {function<double (*)(S *, S *)>(mCompiler, "pointers")};

    EXPECT_EQ(X, pointers(&s, &t));
    EXPECT_EQ(Y + 1.0, pointers(nullptr, &t));
}

TEST_F(CompilerTest, integers)
{
    ASSERT_TRUE(mCompiler->compile(R"(typedef unsigned long uintptr_t;
typedef unsigned long long uint64;
double unsignedLong(double a, double b) { uintptr_t x = a; uintptr_t y = b; return (x / y) + (x - y) * (x < y) + (x ^ y) + (y >= x) + 2 * x; }
double unsignedLongLong(double a) { uint64 x = a; uintptr_t y = 3; return (x ^ y) + (y ^ x) + (x ^ x) + (x == y) + 4294967296 + (-x > 0); }
double signedInt(double a, double b) { return -(a < b) + ((a < b) - (a > b)) / 2; }
uintptr_t address(double *p) { return (uintptr_t) p; }
double *pointer(void *p) { return (double *) p; }
double fromVoid(void *p) { return ((double *) p)[1]; }
)"));

    auto unsignedLong {function<BinaryFunction>(mCompiler, "unsignedLong")};
    auto unsignedLongLong {function<UnaryFunction>(mCompiler, "unsignedLongLong")};
    auto signedInt {function<BinaryFunction>(mCompiler, "signedInt")};

    for (auto a : {1.0, 7.9, 12.0}) {
        for (auto b : {1.0, 3.0, 20.0}) {
            auto x {static_cast<UnsignedLong>(a)};
            auto y {static_cast<UnsignedLong>(b)};
            auto expectedUnsignedLong {(x / y) + ((x - y) * static_cast<UnsignedLong>(x < y)) + (x ^ y)
                                       + static_cast<UnsignedLong>(y >= x) + (2 * x)};
            auto expectedSignedInt {-static_cast<int>(a < b) + ((static_cast<int>(a < b) - static_cast<int>(a > b)) / 2)};

            EXPECT_EQ(static_cast<double>(expectedUnsignedLong), unsignedLong(a, b));
            EXPECT_EQ(static_cast<double>(expectedSignedInt), signedInt(a, b));
        }

        auto x {static_cast<UnsignedLongLong>(a)};

        EXPECT_EQ(static_cast<double>((x ^ 3U) + (3U ^ x) + (x ^ x) + static_cast<UnsignedLongLong>(x == 3U) + 4294967296ULL
                                      + static_cast<UnsignedLongLong>((0ULL - x) > 0)),
                  unsignedLongLong(a));
    }

    std::array<double, 2> values {1.0, 2.0}; // NOLINT

    EXPECT_EQ(reinterpret_cast<uintptr_t>(values.data()), function<uintptr_t (*)(double *)>(mCompiler, "address")(values.data())); // NOLINT
    EXPECT_EQ(values.data(), function<double *(*)(void *)>(mCompiler, "pointer")(values.data()));
    EXPECT_EQ(2.0, function<double (*)(void *)>(mCompiler, "fromVoid")(values.data()));
}

TEST_F(CompilerTest, numbers)
{
    ASSERT_TRUE(mCompiler->compile(R"(double f1() { return 123; }
double f2() { return .5; }
double f3() { return 5.; }
double f4() { return 1e3; }
double f5() { return 1.0E-3; }
double f6() { return 123.456789e99; }
double f7() { return 2.71828182845905; }
double f8() { return 1.0e+2; }
double f9() { return INFINITY; }
double f10() { return -INFINITY; }
double f11() { return NAN; }
)"));

    static const std::vector<std::pair<const char *, double>> EXPECTED {
        {"f1", 123.0},
        {"f2", .5},
        {"f3", 5.},
        {"f4", 1e3},
        {"f5", 1.0E-3},
        {"f6", 123.456789e99},
        {"f7", 2.71828182845905}, // NOLINT
        {"f8", 1.0e+2},
        {"f9", std::numeric_limits<double>::infinity()},
        {"f10", -std::numeric_limits<double>::infinity()},
        {"f11", std::numeric_limits<double>::quiet_NaN()},
    };

    for (const auto &[name, expected] : EXPECTED) {
        EXPECT_TRUE(sameDouble(expected, function<NullaryFunction>(mCompiler, name)())) << name;
    }
}

TEST_F(CompilerTest, mathematicalFunctions)
{
    ASSERT_TRUE(mCompiler->compile(R"(extern double sin(double);
double pow_(double a, double b) { return pow(a, b); }
double sqrt_(double a) { return sqrt(a); }
double fabs_(double a) { return fabs(a); }
double exp_(double a) { return exp(a); }
double log_(double a) { return log(a); }
double log10_(double a) { return log10(a); }
double ceil_(double a) { return ceil(a); }
double floor_(double a) { return floor(a); }
double fmin_(double a, double b) { return fmin(a, b); }
double fmax_(double a, double b) { return fmax(a, b); }
double fmod_(double a, double b) { return fmod(a, b); }
double sin_(double a) { return sin(a); }
double cos_(double a) { return cos(a); }
double tan_(double a) { return tan(a); }
double sinh_(double a) { return sinh(a); }
double cosh_(double a) { return cosh(a); }
double tanh_(double a) { return tanh(a); }
double asin_(double a) { return asin(a); }
double acos_(double a) { return acos(a); }
double atan_(double a) { return atan(a); }
double asinh_(double a) { return asinh(a); }
double acosh_(double a) { return acosh(a); }
double atanh_(double a) { return atanh(a); }
double pointer(double a) { double (*f)(double) = cos; return f(a) + (1 ? f : sin)(a); }
double integer() { return sqrt(4); }
)"));

    static const std::vector<std::pair<const char *, double (*)(double)>> ONE_PARAMETER_FUNCTIONS {
        {"sqrt_", [](double a) {
             return std::sqrt(a);
         }},
        {"fabs_", [](double a) {
             return std::fabs(a);
         }},
        {"exp_", [](double a) {
             return std::exp(a);
         }},
        {"log_", [](double a) {
             return std::log(a);
         }},
        {"log10_", [](double a) {
             return std::log10(a);
         }},
        {"ceil_", [](double a) {
             return std::ceil(a);
         }},
        {"floor_", [](double a) {
             return std::floor(a);
         }},
        {"sin_", [](double a) {
             return std::sin(a);
         }},
        {"cos_", [](double a) {
             return std::cos(a);
         }},
        {"tan_", [](double a) {
             return std::tan(a);
         }},
        {"sinh_", [](double a) {
             return std::sinh(a);
         }},
        {"cosh_", [](double a) {
             return std::cosh(a);
         }},
        {"tanh_", [](double a) {
             return std::tanh(a);
         }},
        {"asin_", [](double a) {
             return std::asin(a);
         }},
        {"acos_", [](double a) {
             return std::acos(a);
         }},
        {"atan_", [](double a) {
             return std::atan(a);
         }},
        {"asinh_", [](double a) {
             return std::asinh(a);
         }},
        {"acosh_", [](double a) {
             return std::acosh(a);
         }},
        {"atanh_", [](double a) {
             return std::atanh(a);
         }},
        {"pointer", [](double a) {
             return 2.0 * std::cos(a); // NOLINT
         }},
    };
    static const std::vector<std::pair<const char *, double (*)(double, double)>> TWO_PARAMETER_FUNCTIONS {
        {"pow_", [](double a, double b) {
             return std::pow(a, b);
         }},
        {"fmin_", [](double a, double b) {
             return std::fmin(a, b);
         }},
        {"fmax_", [](double a, double b) {
             return std::fmax(a, b);
         }},
        {"fmod_", [](double a, double b) {
             return std::fmod(a, b);
         }},
    };

    for (auto a : {-0.5, 0.25, 1.5, 3.0}) {
        for (const auto &[name, expected] : ONE_PARAMETER_FUNCTIONS) {
            EXPECT_TRUE(closeDouble(expected(a), function<UnaryFunction>(mCompiler, name)(a)))
                << name << "(" << a << ")";
        }

        for (auto b : {-2.0, 0.5, 2.0}) {
            for (const auto &[name, expected] : TWO_PARAMETER_FUNCTIONS) {
                EXPECT_TRUE(closeDouble(expected(a, b), function<BinaryFunction>(mCompiler, name)(a, b)))
                    << name << "(" << a << ", " << b << ")";
            }
        }
    }

    EXPECT_EQ(2.0, function<NullaryFunction>(mCompiler, "integer")());
}

TEST_F(CompilerTest, variablesAndStatements)
{
    ASSERT_TRUE(mCompiler->compile(R"(typedef struct {
    double voi;
    double *states;
    unsigned long count;
} Info;
double counter() { static double count; count = count+1.0; return count; }
double structure(double voi, double *states) { Info info = { voi, states, 0 }; Info *p = &info; void *data = p; return ((Info *) data)->voi+((Info *) data)->states[1]+p->count; }
double arrays(double a) { double u[3]; static double v[2]; u[0] = a; u[1] = 2*a; u[2] = 0.0; v[1] = u[1]; return u[0]+u[1]+u[2]+v[0]+v[1]; }
double assignments(double a) { double x; double y; x = y = a; return x+y; }
double afterReturn(double a) { return a; a = 2.0*a; }
void voidAfterReturn(double *a) { a[0] = 1.0; return; a[0] = 2.0; }
double declarations(double a) { double x = a; double *p = &x; double *q = p; return q[0]+x; }
__attribute__((export_name("exported"))) double exportName() { return 1.0; }
)"));

    auto counter {function<NullaryFunction>(mCompiler, "counter")};

    EXPECT_EQ(1.0, counter());
    EXPECT_EQ(2.0, counter());
    EXPECT_EQ(3.0, counter());

    std::array<double, 2> states {3.0, 5.0}; // NOLINT
    double value {0.0};

    EXPECT_EQ(7.0, function<double (*)(double, double *)>(mCompiler, "structure")(2.0, states.data()));
    EXPECT_EQ(5.0, function<UnaryFunction>(mCompiler, "arrays")(1.0));
    EXPECT_EQ(6.0, function<UnaryFunction>(mCompiler, "assignments")(3.0));
    EXPECT_EQ(3.0, function<UnaryFunction>(mCompiler, "afterReturn")(3.0));

    function<void (*)(double *)>(mCompiler, "voidAfterReturn")(&value);

    EXPECT_EQ(1.0, value);
    EXPECT_EQ(6.0, function<UnaryFunction>(mCompiler, "declarations")(3.0));
    EXPECT_EQ(1.0, function<NullaryFunction>(mCompiler, "exportName")());
}

namespace {

libOpenCOR::ExpectedIssues expectedIssues(const std::string &pCode, const std::string &pMessage, size_t pColumn)
{
    // The issues that we expect when compiling the given (one-line) code fails with the given message at the given
    // column.

    return {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, pMessage + ":\n    1 | " + pCode + "\n      | " + std::string(pColumn - 1, ' ') + "^"},
    }};
}

} // namespace

TEST_F(CompilerTest, errors)
{
    struct Error
    {
        std::string code;
        std::string message;
        size_t column;
    };

    static const std::vector<Error> ERRORS {
        // Tokens.

        {"double f() { return 1e; }", "Exponent has no digits", 22},
        {"double f() { return 1e+; }", "Exponent has no digits", 22},
        {"double f() { return 1.0f; }", "Invalid suffix on numeric constant", 24},
        {"double f() { return 3.0 % 2.0; }", "Unexpected character '%'", 25},
        {"__attribute__((export_name(\"f))) double f() { return 1.0; }", "Missing terminating '\"' character", 28},

        // Declaration specifiers.

        {"static double f() { return 1.0; }", "Unsupported storage class 'static'", 1},
        {"void f() { extern double x; }", "Unsupported storage class 'extern'", 12},
        {"void f() { typedef double T; }", "Unsupported storage class 'typedef'", 12},
        {"void f(static double x) {}", "Unsupported storage class 'static'", 8},
        {"__attribute__((used)) double f() { return 1.0; }", "Unsupported attribute (only __attribute__((export_name(\"<name>\"))) is supported)", 1},
        {"__attribute__((export_name(f))) double f() { return 1.0; }", "Unsupported attribute (only __attribute__((export_name(\"<name>\"))) is supported)", 1},
        {"void f(x) {}", "Expected a type specifier", 8},

        // Structures.

        {"typedef struct S { double x; } T;", "Expected '{' after 'struct'", 15},
        {"typedef struct { static double x; } S;", "Unsupported storage class 'static'", 18},
        {"typedef struct { double; } S;", "Expected identifier or '('", 24},
        {"typedef struct { void x; } S;", "Field has invalid type 'void'", 23},
        {"typedef struct { double x } S;", "Expected ';' at end of declaration list", 26},
        {"typedef struct { double x; double x; } S;", "Duplicate member 'x'", 35},

        // Declarators.

        {"typedef double return;", "Expected identifier or '('", 16},
        {"void f() { double restrict x; }", "Expected identifier or '('", 19},
        {"void f(double restrict x) {}", "Expected ')'", 14},
        {"typedef double (T)(double);", "Expected '*'", 17},
        {"typedef double (*T(double);", "Expected ')'", 19},
        {"void f() { double x[0]; }", "Expected a positive integer constant", 21},
        {"void f() { double x[1.5]; }", "Expected a positive integer constant", 21},
        {"void f() { double x[n]; }", "Expected a positive integer constant", 21},
        {"void f() { double x[99999999999999999999]; }", "Expected a positive integer constant", 21},
        {"void f() { double x[2; }", "Expected ']'", 22},
        {"typedef double T(double)(double);", "Invalid declarator for 'T'", 16},
        {"typedef double T(double)[2];", "Invalid declarator for 'T'", 16},
        {"typedef double T[2](double);", "Invalid declarator for 'T'", 16},
        {"void f(double x {}", "Expected ')'", 16},
        {"void f(double x[0]) {}", "Expected a positive integer constant", 17},
        {"void f(void x) {}", "Parameter has invalid type 'void'", 13},
        {"void f(double, void) {}", "Parameter has invalid type 'void'", 20},
        {"void f() { void x; }", "Variable has invalid type 'void'", 17},
        {"void f() { double g(double); }", "Variable has invalid type 'double (double)'", 19},
        {"void f() { double g(void); }", "Variable has invalid type 'double ()'", 19},
        {"void f() { double g(double, double); }", "Variable has invalid type 'double (double, double)'", 19},

        // Top-level declarations.

        {"typedef double T", "Expected ';' after top level declarator", 17},
        {"typedef double T; typedef double T;", "Redefinition of 'T'", 34},
        {"typedef double f; double f() { return 1.0; }", "Conflicting types for 'f'", 26},
        {"double f(double); double f(double, double);", "Conflicting types for 'f'", 26},
        {"double sin(double, double);", "Conflicting types for 'sin'", 8},
        {"double f() { return 1.0; } double f() { return 2.0; }", "Redefinition of 'f'", 35},
        {"double sin(double x) { return x; }", "Redefinition of 'sin'", 8},
        {"double f(double) { return 1.0; }", "Parameter name omitted", 16},
        {"void f(double x, double x) {}", "Redefinition of 'x'", 25},

        // Statements.

        {"void f(double x) { x }", "Expected ';' after expression", 21},
        {"void f(double x) { x = ; }", "Expected expression", 24},
        {"double f() { return; }", "Non-void function 'f' should return a value", 14},
        {"void f() { return 1.0; }", "Void function 'f' should not return a value", 12},
        {"double *f(double x) { return x; }", "Cannot convert 'double' to 'double *'", 30},
        {"void f() { double x; double x; }", "Redefinition of 'x'", 29},
        {"void f() { double 1.0; }", "Expected identifier or '('", 19},
        {"void f() { static double x = 1.0; }", "Static variables cannot be initialised", 26},
        {"void f() { double x[2] = { 1.0, 2.0 }; }", "Invalid initialiser list for type 'double[2]'", 19},
        {"typedef struct { double a; double b; } S; void f() { S s = { 1.0 }; }", "Expected ','", 65},
        {"typedef struct { double a; } S; void f() { S s = { 1.0, 2.0 }; }", "Expected '}'", 55},
        {"typedef struct { double a; } S; void f() { S s = { ; }; }", "Expected expression", 52},
        {"typedef struct { double *a; } S; void f() { S s = { 1.0 }; }", "Cannot convert 'double' to 'double *'", 53},
        {"void f() { double x = ; }", "Expected expression", 23},
        {"void f() { double *x = 1.0; }", "Cannot convert 'double' to 'double *'", 24},
        {"void f(double *p) { unsigned long *q = p; }", "Cannot convert 'double *' to 'unsigned long *'", 40},
        {"void f() { double x }", "Expected ';' at end of declaration", 20},

        // Assignments.

        {"void f(double x) { 1.0 = x; }", "Expression is not assignable", 24},
        {"void f() { double a[2]; a = 1.0; }", "Expression is not assignable", 27},
        {"void f(double x) { double *p; p = x; }", "Cannot convert 'double' to 'double *'", 35},
        {"void f(double x) { double y; y = x = ; }", "Expected expression", 38},

        // Conditional operator.

        {"double f(double x) { return x ? 1.0 : ; }", "Expected expression", 39},
        {"double f(double x) { return x ? : 1.0; }", "Expected expression", 33},
        {"double f(double x) { return x ? 1.0 ; }", "Expected ':'", 36},
        {"typedef struct { double a; } S; double f() { S s; return s ? 1.0 : 2.0; }", "Used type 'S' where arithmetic or pointer type is required", 60},
        {"void g() {} double f() { return g() ? 1.0 : 2.0; }", "Used type 'void' where arithmetic or pointer type is required", 37},
        {"double f(double *p, double x) { return x ? p : x; }", "Incompatible operand types ('double *' and 'double')", 42},
        {"double f(double *p, double x) { return x ? x : p; }", "Incompatible operand types ('double' and 'double *')", 42},
        {"double f(double *p, double x) { return x ? x : x ? p : x; }", "Incompatible operand types ('double *' and 'double')", 50},

        // Binary operators.

        {"double f(double *p) { return p + 1.0; }", "Invalid operands to binary expression ('double *' and 'double')", 32},
        {"double f(double *p) { return p * 2.0 + 1.0; }", "Invalid operands to binary expression ('double *' and 'double')", 32},
        {"double f(double *p) { return 1.0 + 2.0 * p; }", "Invalid operands to binary expression ('double' and 'double *')", 40},
        {"double f(double x) { return 1 ^ x; }", "Invalid operands to binary expression ('int' and 'double')", 31},
        {"double f(double x) { return x ^ 1; }", "Invalid operands to binary expression ('double' and 'int')", 31},
        {"typedef struct { double a; } S; double f() { S s; return s && 1.0; }", "Invalid operands to binary expression ('S' and 'double')", 60},
        {"typedef struct { double a; } S; double f() { S s; return 1.0 || s; }", "Invalid operands to binary expression ('double' and 'S')", 62},
        {"double f(double x) { return x + ; }", "Expected expression", 33},
        {"double f(double x) { return x + x * ; }", "Expected expression", 37},

        // Unary operators and casts.

        {"double f(double *p) { return -p; }", "Invalid argument type 'double *' to unary expression", 30},
        {"typedef struct { double a; } S; double f() { S s; return !s; }", "Invalid argument type 'S' to unary expression", 58},
        {"double *f(double x) { return &(x + 1.0); }", "Cannot take the address of an rvalue of type 'double'", 30},
        {"double f() { return -; }", "Expected expression", 22},
        {"double f(double x) { return (double y) x; }", "Expected ')'", 37},
        {"double f(double x) { return (double ; }", "Expected ')'", 36},
        {"double f(double x) { return (static double) x; }", "Expected expression", 30},
        {"double f(double x) { return (double (x) x; }", "Expected '*'", 38},
        {"double f(double x) { return (double) ; }", "Expected expression", 38},
        {"double f(double x) { return (double *) x; }", "Invalid cast from 'double' to 'double *'", 29},
        {"double f(double *p) { return (double) p; }", "Invalid cast from 'double *' to 'double'", 30},
        {"double f(double x) { return (void) x; }", "Invalid cast from 'double' to 'void'", 29},
        {"typedef struct { double a; } S; double f(double x) { return (S) x; }", "Invalid cast from 'double' to 'S'", 61},
        {"double f(unsigned long x) { return (double *) x; }", "Invalid cast from 'unsigned long' to 'double *'", 36},
        {"double f(double x) { return (unsigned) x; }", "Expected a type specifier", 30},

        // Subscripts, calls, and members.

        {"double f(double x) { return x[1]; }", "Subscripted value is not an array or a pointer", 30},
        {"double f(double (*g)(double)) { return g[0]; }", "Subscripted value is not an array or a pointer", 41},
        {"double f(void *p) { return p[0]; }", "Subscripted value is not an array or a pointer", 29},
        {"double f(double *p) { return p[1.0]; }", "Array subscript is not an integer", 32},
        {"double f(double *p) { return p[1; }", "Expected ']'", 33},
        {"double f(double *p) { return p[]; }", "Expected expression", 32},
        {"double f(double x) { return x(1.0); }", "Called object type 'double' is not a function or function pointer", 30},
        {"double f(double *p) { return p(1.0); }", "Called object type 'double *' is not a function or function pointer", 31},
        {"double f(double x) { return f(x, x); }", "Expected 1 argument(s), got 2", 30},
        {"double f(double x) { return f(x; }", "Expected ')'", 32},
        {"double f(double x) { return f(; }", "Expected expression", 31},
        {"double f(double *p) { return f(1.0); }", "Cannot convert 'double' to 'double *'", 32},
        {"double f(double x) { return x->a; }", "Member reference type 'double' is not a pointer to a structure", 30},
        {"double f(double *p) { return p->a; }", "Member reference type 'double *' is not a pointer to a structure", 31},
        {"typedef struct { double a; } S; double f(S *s) { return s->b; }", "No member named 'b'", 60},
        {"typedef struct { double a; } S; double f(S *s) { return s->1; }", "No member named '1'", 60},

        // Primary expressions.

        {"double f(double x) { return (x; }", "Expected ')'", 31},
        {"double f() { return 99999999999999999999; }", "Invalid integer constant", 21},
        {"double f() { return 9223372036854775808; }", "Invalid integer constant", 21},
        {"double f() { return 09; }", "Invalid integer constant", 21},
        {"double f() { return y; }", "Use of undeclared identifier 'y'", 21},
        {"typedef double T; double f() { return T; }", "Unexpected type name 'T': expected expression", 39},
        {"double f() { return double; }", "Expected expression", 21},
        {"double f() { return \"x\"; }", "Expected expression", 21},
        {"double f(double *p) { return p; }", "Cannot convert 'double *' to 'double'", 30},
        {"double f(double x) { return f; }", "Cannot convert 'double (double) *' to 'double'", 29},
        {"double g(double a, double b) { return g; }", "Cannot convert 'double (double, double) *' to 'double'", 39},
        {"double h() { return h; }", "Cannot convert 'double () *' to 'double'", 21},
    };

    for (const auto &error : ERRORS) {
        EXPECT_FALSE(mCompiler->compile(error.code)) << error.code;
        EXPECT_EQ_ISSUES(mCompiler, expectedIssues(error.code, error.message, error.column));
    }

    // Errors on a line other than the first one.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_01 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Exponent has no digits:\n    3 |     return 1.0e;\n      |               ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("double f()\n{\n    return 1.0e;\n}"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_01);

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES_02 {{
        {libOpenCOR::Issue::Type::ERROR, "The given code could not be compiled."},
        {libOpenCOR::Issue::Type::ERROR, "Missing terminating '\"' character:\n    1 | __attribute__((export_name(\"f\n      |                            ^"},
    }};

    EXPECT_FALSE(mCompiler->compile("__attribute__((export_name(\"f\n\"))) double f() { return 1.0; }"));
    EXPECT_EQ_ISSUES(mCompiler, EXPECTED_ISSUES_02);

    // Numbers at the very end of the code.

    static const std::vector<Error> NUMBERS {
        {"1", "Expected a type specifier", 1},
        {"1.", "Expected a type specifier", 1},
        {"1.5", "Expected a type specifier", 1},
        {"1e", "Exponent has no digits", 2},
        {"1e5", "Expected a type specifier", 1},
    };

    for (const auto &number : NUMBERS) {
        EXPECT_FALSE(mCompiler->compile(number.code)) << number.code;
        EXPECT_EQ_ISSUES(mCompiler, expectedIssues(number.code, number.message, number.column));
    }
}

TEST_F(CompilerTest, nestingDepth)
{
    // Expressions can be nested (e.g., using parentheses, unary operators, or casts) up to a certain depth, beyond
    // which an error is reported (rather than risking a stack overflow).

    static constexpr size_t MAX_DEPTH {255};

    auto parentheses = [](size_t pDepth) {
        return "double f(double x) { return " + std::string(pDepth, '(') + "x" + std::string(pDepth, ')') + "; }";
    };
    auto minuses = [](size_t pDepth) {
        return "double f(double x) { return " + std::string(pDepth, '-') + "x; }";
    };
    auto casts = [](size_t pDepth) {
        std::string res {"double f(double x) { return "};

        for (size_t i {0}; i < pDepth; ++i) {
            res += "(double) ";
        }

        return res + "x; }";
    };
    static const std::string PREFIX {"double f(double x) { return "};

    ASSERT_TRUE(mCompiler->compile(parentheses(MAX_DEPTH)));
    EXPECT_EQ(3.0, function<UnaryFunction>(mCompiler, "f")(3.0));

    EXPECT_FALSE(mCompiler->compile(parentheses(MAX_DEPTH + 1)));
    EXPECT_EQ_ISSUES(mCompiler, expectedIssues(parentheses(MAX_DEPTH + 1), "Expression is too deeply nested", PREFIX.size() + MAX_DEPTH + 2));

    ASSERT_TRUE(mCompiler->compile(minuses(MAX_DEPTH)));
    EXPECT_EQ(-3.0, function<UnaryFunction>(mCompiler, "f")(3.0));

    EXPECT_FALSE(mCompiler->compile(minuses(MAX_DEPTH + 1)));
    EXPECT_EQ_ISSUES(mCompiler, expectedIssues(minuses(MAX_DEPTH + 1), "Expression is too deeply nested", PREFIX.size() + MAX_DEPTH + 1));

    ASSERT_TRUE(mCompiler->compile(casts(MAX_DEPTH)));
    EXPECT_EQ(3.0, function<UnaryFunction>(mCompiler, "f")(3.0));

    EXPECT_FALSE(mCompiler->compile(casts(MAX_DEPTH + 1)));
    EXPECT_EQ_ISSUES(mCompiler, expectedIssues(casts(MAX_DEPTH + 1), "Expression is too deeply nested", PREFIX.size() + (MAX_DEPTH * std::string("(double) ").size()) + 1));
}

TEST_F(CompilerTest, expressionHeight)
{
    // Expressions can be high (e.g., a sum with many terms or a piecewise expression with many pieces) up to a certain
    // height, beyond which an error is reported (rather than risking a stack overflow).
    // Note: the height of a sum with N terms is N + 1 (since loading the value of x counts as one level) and that of a
    //       piecewise expression with N pieces is N + 3.

    static constexpr size_t MAX_HEIGHT {1024};
    static constexpr size_t MAX_TERMS {MAX_HEIGHT - 1};
    static constexpr size_t MAX_PIECES {MAX_HEIGHT - 3};

    auto sum = [](size_t pTerms) {
        std::string res {"double f(double x) { return x"};

        for (size_t i {1}; i < pTerms; ++i) {
            res += " + x";
        }

        return res + "; }";
    };
    auto piecewise = [](size_t pPieces) {
        std::string res {"double f(double x) { return "};

        for (size_t i {0}; i < pPieces; ++i) {
            res += "(x < " + std::to_string(i) + ".0) ? " + std::to_string(i) + ".0 : ";
        }

        return res + "x; }";
    };
    auto column = [](const std::string &pCode, char pCharacter, size_t pOccurrence) {
        size_t res {0};

        for (size_t i {0}; i < pOccurrence; ++i) {
            res = pCode.find(pCharacter, res) + 1;
        }

        return res;
    };

    ASSERT_TRUE(mCompiler->compile(sum(MAX_TERMS)));
    EXPECT_EQ(static_cast<double>(MAX_TERMS), function<UnaryFunction>(mCompiler, "f")(1.0));

    auto code {sum(MAX_TERMS + 1)};

    EXPECT_FALSE(mCompiler->compile(code));
    EXPECT_EQ_ISSUES(mCompiler, expectedIssues(code, "Expression is too deeply nested", column(code, '+', MAX_TERMS)));

    ASSERT_TRUE(mCompiler->compile(piecewise(MAX_PIECES)));
    EXPECT_EQ(5.0, function<UnaryFunction>(mCompiler, "f")(4.5));
    EXPECT_EQ(2000.0, function<UnaryFunction>(mCompiler, "f")(2000.0));

    code = piecewise(MAX_PIECES + 1);

    EXPECT_FALSE(mCompiler->compile(code));
    EXPECT_EQ_ISSUES(mCompiler, expectedIssues(code, "Expression is too deeply nested", column(code, '?', 1)));

    // Much higher expressions, which used to result in a stack overflow.

    EXPECT_FALSE(mCompiler->compile(sum(100000)));
    EXPECT_FALSE(mCompiler->compile(piecewise(100000)));
}

TEST_F(CompilerTest, constantExpressions)
{
    // Constant expressions are evaluated (as Clang does) unless their evaluation has undefined behaviour and conditions
    // that are constant only result in the relevant code being generated.

    ASSERT_TRUE(mCompiler->compile(R"(double integerComparisons(double a, double b) { return ((1 == 1) ? a : b) + ((1 != 1) ? a : b) + ((1 < 2) ? a : b) + ((2 <= 1) ? a : b) + ((1 > 2) ? a : b) + ((2 >= 2) ? a : b); }
double unsignedComparisons(double a, double b) { return (((unsigned long) 1 < (unsigned long) -1) ? a : b) + (((unsigned long) 2 >= 3) ? a : b); }
double doubleComparisons(double a, double b) { return ((1.0 == 1.0) ? a : b) + ((1.0 != 1.0) ? a : b) + ((1.0 < 2.0) ? a : b) + ((2.0 <= 1.0) ? a : b) + ((1.0 > 2.0) ? a : b) + ((2.0 >= 2.0) ? a : b); }
double doubleOperations(double a, double b) { return ((1.0 + 2.0 * 3.0 - 4.0 / 2.0 > 4.5) ? a : b) + ((0.0 / 0.0 != 0.0) ? a : b) + ((1.0 / 0.0 > 0.0) ? a : b); }
double integerOperations(double a, double b) { return (((3 - 2) * 4 / 2 + 1 == 3) ? a : b) + ((6 ^ 3) ? a : b) + (((unsigned long) 7 / 2 - 1 + 2 * (unsigned long) 3 == 8) ? a : b); }
double casts(double a, double b) { return (((double) 3 > 2.5) ? a : b) + (((double) (unsigned long) -1 > 0.0) ? a : b) + (((int) 2.9 == 2) ? a : b) + (((unsigned long) 3.7 == 3) ? a : b) + (((unsigned long) -1 > 0) ? a : b) + (((int) (unsigned long) 5 == 5) ? a : b) + (((int) (unsigned long long) 4294967296 == 0) ? a : b) + (((double) 1 ? 1 : 0) ? a : b) + (((int) 1) ? a : b); }
double unaryOperators(double a, double b) { return ((!0) ? a : b) + ((!1.0) ? a : b) + ((-(1.0) < 0.0) ? a : b) + ((-(1) < 0) ? a : b) + ((-(unsigned long) 1 > 0) ? a : b); }
double logicalOperators(double a, double b) { return ((1 && 0) ? a : b) + ((0 || 1) ? a : b) + ((1 && 2.0) ? a : b) + ((0.0 || 0) ? a : b) + ((1 && a) ? a : b) + ((a && 1) ? a : b) + ((0 || a) ? a : b) + ((a || 0) ? a : b) + ((0 && a) ? a : b) + ((1 || a) ? a : b) + ((a && 0) ? a : b) + ((a || 1) ? a : b) + ((a && (0 && b)) ? a : b) + ((a || (1 || b)) ? a : b); }
double logicalValues(double a) { return (1 && a) + (0 && a) + (1 || a) + (0 || a) + (a && 1) + (a || 0); }
double conditionalOperators(double a, double b) { return (((1 ? 2 : 3) > 1) ? a : b) + ((0 ? 2 : 3) ? a : b) + ((1 ? 0.0 : a) ? a : b) + ((0 ? a : 2.0) ? a : b) + (1 ? a : b) + (0 ? a : b) + (1.0 ? a : b); }
double undefinedOperations(double a, double b) { return ((2147483647 + 1) ? a : b) + ((-2147483647 - 2) ? a : b) + ((65536 * 65536) ? a : b) + ((1 / 0) ? a : b) + (((-2147483647 - 1) / -1) ? a : b) + ((-(-2147483647 - 1)) ? a : b) + (((int) 1e10) ? a : b) + (((int) -1e10) ? a : b) + (((unsigned long) -1.0) ? a : b) + (((unsigned long) 1e20) ? a : b) + (((int) (0.0 / 0.0)) ? a : b) + (a ? 1.0 : 1 / 0); }
)"));

    static constexpr double A {1.0};
    static constexpr double B {10.0};

    EXPECT_EQ((3 * A) + (3 * B), function<BinaryFunction>(mCompiler, "integerComparisons")(A, B));
    EXPECT_EQ(A + B, function<BinaryFunction>(mCompiler, "unsignedComparisons")(A, B));
    EXPECT_EQ((3 * A) + (3 * B), function<BinaryFunction>(mCompiler, "doubleComparisons")(A, B));
    EXPECT_EQ(3 * A, function<BinaryFunction>(mCompiler, "doubleOperations")(A, B));
    EXPECT_EQ(3 * A, function<BinaryFunction>(mCompiler, "integerOperations")(A, B));
    EXPECT_EQ(9 * A, function<BinaryFunction>(mCompiler, "casts")(A, B));
    EXPECT_EQ((4 * A) + B, function<BinaryFunction>(mCompiler, "unaryOperators")(A, B));
    EXPECT_EQ((9 * A) + (5 * B), function<BinaryFunction>(mCompiler, "logicalOperators")(A, B));
    EXPECT_EQ(9 * B, function<BinaryFunction>(mCompiler, "logicalOperators")(0.0, B));
    EXPECT_EQ(5.0, function<UnaryFunction>(mCompiler, "logicalValues")(A));
    EXPECT_EQ(1.0, function<UnaryFunction>(mCompiler, "logicalValues")(0.0));
    EXPECT_EQ((5 * A) + (2 * B), function<BinaryFunction>(mCompiler, "conditionalOperators")(A, B));

    // Note: we don't call undefinedOperations() since its behaviour is undefined.

    EXPECT_NE(nullptr, mCompiler->function("undefinedOperations"));
}

TEST_F(CompilerTest, typesAndFunctions)
{
    ASSERT_TRUE(mCompiler->compile(R"(typedef unsigned long size_t;
typedef unsigned long long uint64;
int integer(int i, int j) { int k = i * j - i / j; return -k + (i < j) + (i <= j) + (i > j) + (i >= j) + (i == j) + (i != j) + (k ^ i); }
double integerConversions(double a, int i, size_t n) { int j = a; size_t m = i; uint64 p = n; return j + m + p + (int) a + (size_t) a + (uint64) i + (double) i + (int) n; }
double voidParameters(void) { return 1.0; }
double arrayParameters(double a[2], double b[3]) { return a[1] + b[0]; }
double functionParameters(double f(double), double (*g)(double), double x) { return f(x) + g(x); }
double unprototypedFunctionPointer(double (*f)(), double x) { return f(x, x); }
double declaredThenDefined(double);
double declaredThenDefined(double x) { return 2.0 * x; }
double callDeclaredThenDefined(double x) { return declaredThenDefined(x); }
double *identity(double *p) { return p; }
double indexCallResult(double *p) { return identity(p)[1] + (p ? p : identity(p))[0]; }
double localArray(int i, double x) { double a[3]; a[0] = x; a[1] = 2.0 * x; a[2] = 3.0 * x; return a[i] + a[2]; }
double pointerToPointer(double **p, size_t *n, int *i) { return p[0][1] + n[0] + i[1]; }
double functionPointerSelect(double x) { return (x > 0.0 ? cos : sin)(x); }
double identityCast(double x) { return (double) x + (int) (int) x; }
double unaryCondition(double x) { return (-x) ? 1.0 : 2.0; }
double integerCondition(int i, size_t n) { return (i ? 1.0 : 2.0) + (n ? 3.0 : 4.0) + (!i ? 5.0 : 6.0); }
double assignmentCondition(double a, double b) { int i; return ((i = (a < b)) ? 1.0 : 3.0) + 10 * i; }
double mixedConditional(double x, int i) { return (x > 0.0) ? i : x; }
double addressOfFunction(double x) { double (*f)(double) = &cos; return f(x); }
)"));

    for (auto i : {-7, -1, 2, 5}) {
        for (auto j : {-3, 1, 4}) {
            auto k {(i * j) - (i / j)};

            EXPECT_EQ(-k + static_cast<int>(i < j) + static_cast<int>(i <= j) + static_cast<int>(i > j) + static_cast<int>(i >= j)
                          + static_cast<int>(i == j) + static_cast<int>(i != j) + (k ^ i), // NOLINT
                      function<int (*)(int, int)>(mCompiler, "integer")(i, j));
        }

        EXPECT_EQ(((i != 0) ? 1.0 : 2.0) + 3.0 + ((i == 0) ? 5.0 : 6.0),
                  function<double (*)(int, UnsignedLong)>(mCompiler, "integerCondition")(i, 1));
        EXPECT_EQ(static_cast<double>(i), function<double (*)(double, int)>(mCompiler, "mixedConditional")(1.0, i));
        EXPECT_EQ(-1.0, function<double (*)(double, int)>(mCompiler, "mixedConditional")(-1.0, i));
    }

    EXPECT_EQ(2.0 + 4.0 + 5.0, function<double (*)(int, UnsignedLong)>(mCompiler, "integerCondition")(0, 0));

    for (auto a : {0.5, 3.7}) {
        for (auto i : {0, 3}) {
            for (const UnsignedLong n : {1UL, 12UL}) {
                // Note: we convert our values explicitly, as C implicitly does.

                auto j {static_cast<int>(a)};
                auto m {static_cast<UnsignedLong>(i)};
                auto p {static_cast<UnsignedLongLong>(n)};
                auto expected {static_cast<UnsignedLong>(j) + m + p + static_cast<UnsignedLongLong>(static_cast<int>(a))
                               + static_cast<UnsignedLong>(a) + static_cast<UnsignedLongLong>(i)};

                EXPECT_EQ(static_cast<double>(expected) + static_cast<double>(i) + static_cast<int>(n),
                          function<double (*)(double, int, UnsignedLong)>(mCompiler, "integerConversions")(a, i, n));
            }
        }
    }

    std::array<double, 3> values {3.0, 5.0, 7.0}; // NOLINT
    std::array<double *, 1> pointers {values.data()};
    std::array<UnsignedLong, 1> sizes {11}; // NOLINT
    std::array<int, 2> integers {13, 17}; // NOLINT

    EXPECT_EQ(1.0, function<NullaryFunction>(mCompiler, "voidParameters")());
    EXPECT_EQ(5.0 + 3.0, function<double (*)(double *, double *)>(mCompiler, "arrayParameters")(values.data(), values.data()));
    EXPECT_EQ(2.0 * 3.0, function<UnaryFunction>(mCompiler, "callDeclaredThenDefined")(3.0));
    EXPECT_EQ(5.0 + 3.0, function<double (*)(double *)>(mCompiler, "indexCallResult")(values.data()));
    EXPECT_EQ(5.0 + 11.0 + 17.0, function<double (*)(double **, UnsignedLong *, int *)>(mCompiler, "pointerToPointer")(pointers.data(), sizes.data(), integers.data()));

    for (auto i : {0, 1, 2}) {
        EXPECT_EQ(((i + 1) * 2.0) + (3 * 2.0), function<double (*)(int, double)>(mCompiler, "localArray")(i, 2.0));
    }

    auto twice = [](double pX) {
        return 2.0 * pX; // NOLINT
    };
    auto thrice = [](double pX) {
        return 3.0 * pX; // NOLINT
    };
    auto add = [](double pX, double pY) {
        return pX + pY;
    };

    EXPECT_EQ(5.0 * 7.0, function<double (*)(double (*)(double), double (*)(double), double)>(mCompiler, "functionParameters")(twice, thrice, 7.0));
    EXPECT_EQ(2.0 * 7.0, function<double (*)(double (*)(double, double), double)>(mCompiler, "unprototypedFunctionPointer")(add, 7.0));

    for (auto x : {-2.5, 0.0, 1.5}) {
        EXPECT_TRUE(closeDouble((x > 0.0) ? std::cos(x) : std::sin(x), function<UnaryFunction>(mCompiler, "functionPointerSelect")(x)));
        EXPECT_TRUE(closeDouble(std::cos(x), function<UnaryFunction>(mCompiler, "addressOfFunction")(x)));
        EXPECT_EQ(x + std::trunc(x), function<UnaryFunction>(mCompiler, "identityCast")(x));
        EXPECT_EQ((x != 0.0) ? 1.0 : 2.0, function<UnaryFunction>(mCompiler, "unaryCondition")(x));
        EXPECT_EQ((x < 1.0) ? 11.0 : 3.0, function<BinaryFunction>(mCompiler, "assignmentCondition")(x, 1.0));
    }
}

TEST_F(CompilerTest, structures)
{
    ASSERT_TRUE(mCompiler->compile(R"(typedef struct { int a; double b; } Padded;
typedef struct { double a; int b; } TrailingPadding;
typedef struct { double a[2]; double b; } WithArray;
typedef struct { double a; double *b; } Named;
typedef Named Alias;
double padded(double x) { Padded p = { 1, x }; TrailingPadding q = { x, 2 }; Padded *pp = &p; TrailingPadding *qq = &q; return pp->a + pp->b + qq->a + qq->b; }
double withArray(WithArray *w) { return w->a[1] + w->b; }
double alias(Alias *a) { return a->a + a->b[0]; }
)"));

    struct WithArray
    {
        std::array<double, 2> a;
        double b;
    };

    struct Named
    {
        double a;
        double *b;
    };

    WithArray withArray {{3.0, 5.0}, 7.0}; // NOLINT
    double value {11.0}; // NOLINT
    Named named {13.0, &value}; // NOLINT

    EXPECT_EQ(1.0 + 2.0 + 2.0 + 2.0, function<UnaryFunction>(mCompiler, "padded")(2.0));
    EXPECT_EQ(5.0 + 7.0, function<double (*)(WithArray *)>(mCompiler, "withArray")(&withArray));
    EXPECT_EQ(13.0 + 11.0, function<double (*)(Named *)>(mCompiler, "alias")(&named));
}

TEST_F(CompilerTest, restrictQualifiers)
{
    // As Clang does, only flag a restrict-qualified pointer parameter of a function definition as not aliasing any
    // other pointer, i.e. neither a restrict-qualified pointer parameter of a function declaration, a pointer to a
    // restrict-qualified pointer, nor a restrict-qualified local variable.

    static const std::string CODE {R"(double scale(double * restrict, double);
double sum(double * restrict a, double * restrict * b, double *restrict c, double *d) { double * restrict e = d; a[0] = b[0][0] + c[0] + e[0]; return scale(a, 2.0); }
double scale(double *a, double x) { return x * a[0]; }
)"};

    ASSERT_TRUE(mCompiler->compile(CODE));

    std::array<double, 4> values {0.0, 3.0, 5.0, 7.0}; // NOLINT
    std::array<double *, 1> pointers {&values[1]};

    EXPECT_EQ(2.0 * (3.0 + 5.0 + 7.0), function<double (*)(double *, double **, double *, double *)>(mCompiler, "sum")(values.data(), pointers.data(), &values[2], &values[3]));
    EXPECT_EQ(3.0 + 5.0 + 7.0, values[0]);

    const libOpenCOR::IrGeneratorTarget target;
    std::string error;
    auto ir {libOpenCOR::generateIrText(CODE + "double declared(double * restrict a);\ndouble callDeclared(double *a) { return declared(a); }\n", target, error)};

    EXPECT_EQ("", error);
    EXPECT_NE(std::string::npos, ir.find("define double @sum(ptr noalias noundef %0, ptr noundef %1, ptr noalias noundef %2, ptr noundef %3)"));
    EXPECT_NE(std::string::npos, ir.find("define double @scale(ptr noundef %0, double noundef %1)"));
    EXPECT_NE(std::string::npos, ir.find("declare double @declared(ptr noundef)"));
}

TEST_F(CompilerTest, targets)
{
    // Our IR generator doesn't have any platform-specific code path, but some of the LLVM IR that it generates depends
    // on the target, so check it for a target other than the one on which we are running, i.e. WebAssembly (where long
    // is 32 bits, where large arrays are further aligned, where functions without a prototype are flagged as such, and
    // where our functions are hidden).

    static const std::string CODE {R"(typedef unsigned long size_t;
extern size_t count();
double f(size_t n) { double small[1]; double large[2]; small[0] = 1.0; large[1] = 2.0; return ((unsigned long long) n ? small[0] : large[1]) + count(); }
)"};

    libOpenCOR::IrGeneratorTarget target;

    target.triple = "wasm32-unknown-emscripten";
    target.dataLayout = "e-m:e-p:32:32-p10:8:8-p20:8:8-i64:64-i128:128-n32:64-S128-ni:1:10:20";
    target.longBits = 32; // NOLINT
    target.largeArrayMinBits = 128; // NOLINT
    target.largeArrayAlignment = 128; // NOLINT
    target.unprototypedDeclarationAttributes = {{"no-prototype", ""}};
    target.definitionVisibility = 1; // llvm::GlobalValue::HiddenVisibility.

    std::string error;
    auto ir {libOpenCOR::generateIrText(CODE, target, error)};

    EXPECT_EQ("", error);
    EXPECT_NE(std::string::npos, ir.find("target triple = \"wasm32-unknown-emscripten\""));
    EXPECT_NE(std::string::npos, ir.find("define hidden double @f(i32 noundef %0)"));
    EXPECT_NE(std::string::npos, ir.find("alloca [1 x double], align 8\n"));
    EXPECT_NE(std::string::npos, ir.find("alloca [2 x double], align 16\n"));
    EXPECT_NE(std::string::npos, ir.find("zext i32 %7 to i64\n  %9 = icmp ne i64 %8, 0\n"));
    EXPECT_NE(std::string::npos, ir.find("declare i32 @count(...) #2\n"));
    EXPECT_NE(std::string::npos, ir.find("attributes #2 = { \"no-prototype\" }\n"));

    // Some code that cannot be compiled.

    EXPECT_EQ("", libOpenCOR::generateIrText("void", target, error));
    EXPECT_EQ("Expected identifier or '(':\n    1 | void\n      |     ^", error);
}
