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

#include "irgenerator.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <deque>
#include <format>
#include <limits>
#include <map>
#include <tuple>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace libOpenCOR {

namespace {

// Our IR generator is a small compiler for the subset of C that libCellML's generator produces with our generator
// profile, i.e. functions that take and return doubles and pointers (possibly restrict-qualified), local and static
// variables, arrays of doubles, a structure (for our NLA systems), arithmetic, relational and logical operators, the
// conditional operator, calls, casts, as well as __attribute__((export_name("..."))) (for our WASM version). The IR it
// generates mirrors what Clang generates for that subset (with the -cc1 arguments that would normally be used), so that
// LLVM optimises our code in the same way (e.g., the same intrinsics for our mathematical functions and the same
// floating-point contractions).

// Tokens.

enum class TokenKind : uint8_t
{
    END,
    IDENTIFIER,
    NUMBER,
    STRING,
    PUNCTUATOR
};

struct Token
{
    TokenKind kind {TokenKind::END};
    std::string_view text;
    size_t line {1};
    size_t column {1};
};

// Types.
// Note: types are interned, so two types are the same if and only if they are the same object.

struct Type;

using TypePtr = const Type *;

struct Field
{
    std::string_view name;
    TypePtr type {nullptr};
};

struct Type
{
    enum class Kind : uint8_t
    {
        VOID,
        INTEGER,
        DOUBLE,
        POINTER,
        ARRAY,
        STRUCT,
        FUNCTION
    };

    Kind kind {Kind::VOID};
    unsigned int bits {0}; // INTEGER.
    bool isSigned {false}; // INTEGER.
    TypePtr element {nullptr}; // POINTER (pointee), ARRAY (element), and FUNCTION (return type).
    uint64_t count {0}; // ARRAY.
    std::vector<TypePtr> params; // FUNCTION.
    std::vector<Field> fields; // STRUCT.
    std::string name; // INTEGER and STRUCT.
};

class Types
{
public:
    explicit Types(unsigned int pLongBits)
        : mVoid(newType(Type::Kind::VOID))
        , mDouble(newType(Type::Kind::DOUBLE))
        , mInt(newInteger("int", 32, true)) // NOLINT
        , mLongLong(newInteger("long long", 64, true)) // NOLINT
        , mUnsignedLong(newInteger("unsigned long", pLongBits, false))
        , mUnsignedLongLong(newInteger("unsigned long long", 64, false)) // NOLINT
    {
    }

    [[nodiscard]] TypePtr voidType() const
    {
        return mVoid;
    }

    [[nodiscard]] TypePtr doubleType() const
    {
        return mDouble;
    }

    [[nodiscard]] TypePtr intType() const
    {
        return mInt;
    }

    [[nodiscard]] TypePtr longLongType() const
    {
        return mLongLong;
    }

    [[nodiscard]] TypePtr unsignedLongType() const
    {
        return mUnsignedLong;
    }

    [[nodiscard]] TypePtr unsignedLongLongType() const
    {
        return mUnsignedLongLong;
    }

    TypePtr pointerTo(TypePtr pType)
    {
        auto &res {mPointers[pType]};

        if (res == nullptr) {
            auto *type {newType(Type::Kind::POINTER)};

            type->element = pType;

            res = type;
        }

        return res;
    }

    TypePtr arrayOf(TypePtr pType, uint64_t pCount)
    {
        auto &res {mArrays[{pType, pCount}]};

        if (res == nullptr) {
            auto *type {newType(Type::Kind::ARRAY)};

            type->element = pType;
            type->count = pCount;

            res = type;
        }

        return res;
    }

    TypePtr functionOf(TypePtr pReturnType, const std::vector<TypePtr> &pParams)
    {
        auto &res {mFunctions[{pReturnType, pParams}]};

        if (res == nullptr) {
            auto *type {newType(Type::Kind::FUNCTION)};

            type->element = pReturnType;
            type->params = pParams;

            res = type;
        }

        return res;
    }

    Type *newStruct()
    {
        return newType(Type::Kind::STRUCT);
    }

private:
    std::deque<Type> mTypes;
    std::map<TypePtr, TypePtr> mPointers;
    std::map<std::pair<TypePtr, uint64_t>, TypePtr> mArrays;
    std::map<std::pair<TypePtr, std::vector<TypePtr>>, TypePtr> mFunctions;

    TypePtr mVoid;
    TypePtr mDouble;
    TypePtr mInt;
    TypePtr mLongLong;
    TypePtr mUnsignedLong;
    TypePtr mUnsignedLongLong;

    Type *newType(Type::Kind pKind)
    {
        auto &res {mTypes.emplace_back()};

        res.kind = pKind;

        return &res;
    }

    Type *newInteger(const char *pName, unsigned int pBits, bool pIsSigned)
    {
        auto *res {newType(Type::Kind::INTEGER)};

        res->name = pName;
        res->bits = pBits;
        res->isSigned = pIsSigned;

        return res;
    }
};

// Note: types, declarators, expressions, and the code that we generate for them are naturally recursive, which is fine
//       since our expressions cannot be nested too deeply nor be too high (see MAX_NESTING_DEPTH and
//       MAX_EXPRESSION_HEIGHT).
// NOLINTBEGIN(misc-no-recursion)

std::string typeName(TypePtr pType)
{
    switch (pType->kind) {
    case Type::Kind::VOID:
        return "void";
    case Type::Kind::INTEGER:
    case Type::Kind::STRUCT:
        return pType->name;
    case Type::Kind::DOUBLE:
        return "double";
    case Type::Kind::POINTER:
        return typeName(pType->element) + " *";
    case Type::Kind::ARRAY:
        return typeName(pType->element) + "[" + std::to_string(pType->count) + "]";
    default: { // Type::Kind::FUNCTION.
        std::string res {typeName(pType->element) + " ("};

        for (size_t i {0}; i < pType->params.size(); ++i) {
            res += ((i == 0) ? "" : ", ") + typeName(pType->params[i]);
        }

        return res + ")";
    }
    }
}

bool isArithmetic(TypePtr pType)
{
    return (pType->kind == Type::Kind::INTEGER) || (pType->kind == Type::Kind::DOUBLE);
}

bool isScalar(TypePtr pType)
{
    return isArithmetic(pType) || (pType->kind == Type::Kind::POINTER);
}

bool isObject(TypePtr pType)
{
    // Whether the given type is that of an object that we can create, i.e. a variable, a field, or a parameter.

    if (pType->kind == Type::Kind::ARRAY) {
        return isObject(pType->element);
    }

    return (pType->kind != Type::Kind::VOID) && (pType->kind != Type::Kind::FUNCTION);
}

// Our mathematical functions and how Clang lowers them when math errno is disabled (i.e. to an intrinsic) to a frem
// instruction (for fmod()) or to a call to the C standard library function.

struct Builtin
{
    std::string_view name;
    unsigned int paramCount;
    llvm::Intrinsic::ID intrinsic;
};

constexpr auto FMOD {llvm::Intrinsic::num_intrinsics};

constexpr std::array BUILTINS {
    Builtin {"pow", 2, llvm::Intrinsic::pow},
    Builtin {"sqrt", 1, llvm::Intrinsic::sqrt},
    Builtin {"fabs", 1, llvm::Intrinsic::fabs},
    Builtin {"exp", 1, llvm::Intrinsic::exp},
    Builtin {"log", 1, llvm::Intrinsic::log},
    Builtin {"log10", 1, llvm::Intrinsic::log10},
    Builtin {"ceil", 1, llvm::Intrinsic::ceil},
    Builtin {"floor", 1, llvm::Intrinsic::floor},
    Builtin {"fmin", 2, llvm::Intrinsic::minnum},
    Builtin {"fmax", 2, llvm::Intrinsic::maxnum},
    Builtin {"fmod", 2, FMOD},
    Builtin {"sin", 1, llvm::Intrinsic::sin},
    Builtin {"cos", 1, llvm::Intrinsic::cos},
    Builtin {"tan", 1, llvm::Intrinsic::tan},
    Builtin {"sinh", 1, llvm::Intrinsic::sinh},
    Builtin {"cosh", 1, llvm::Intrinsic::cosh},
    Builtin {"tanh", 1, llvm::Intrinsic::tanh},
    Builtin {"asin", 1, llvm::Intrinsic::asin},
    Builtin {"acos", 1, llvm::Intrinsic::acos},
    Builtin {"atan", 1, llvm::Intrinsic::atan},
    Builtin {"asinh", 1, llvm::Intrinsic::not_intrinsic},
    Builtin {"acosh", 1, llvm::Intrinsic::not_intrinsic},
    Builtin {"atanh", 1, llvm::Intrinsic::not_intrinsic},
};

// Symbols.

struct Symbol
{
    enum class Kind : uint8_t
    {
        VARIABLE,
        FUNCTION,
        TYPEDEF
    };

    Kind kind {Kind::VARIABLE};
    std::string_view name;
    TypePtr type {nullptr};
    bool isStatic {false}; // Static local variable.
    bool isRestrict {false}; // Restrict-qualified pointer parameter.
    bool isDefined {false}; // Function that has been defined.
    const Builtin *builtin {nullptr}; // Mathematical function.
    std::string_view exportName;
    llvm::Value *value {nullptr}; // Alloca or global variable for a variable, and function for a function.
};

// Abstract syntax tree.

struct Expr;

using ExprPtr = std::unique_ptr<Expr>;

struct Expr
{
    enum class Kind : uint8_t
    {
        NUMBER,
        VARIABLE,
        FUNCTION,
        CAST,
        UNARY,
        BINARY,
        LOGICAL,
        CONDITIONAL,
        CALL,
        INDEX,
        MEMBER,
        ADDRESS,
        ASSIGNMENT
    };

    enum class Cast : uint8_t
    {
        NONE,
        LOAD,
        DECAY,
        INTEGER_TO_DOUBLE,
        DOUBLE_TO_INTEGER,
        INTEGER_TO_INTEGER,
        POINTER_TO_POINTER,
        POINTER_TO_INTEGER
    };

    Kind kind {Kind::NUMBER};
    TypePtr type {nullptr};
    Token token;
    bool isLvalue {false};
    double doubleValue {0.0};
    uint64_t integerValue {0};
    Symbol *symbol {nullptr};
    size_t field {0};
    Cast cast {Cast::LOAD};
    std::vector<ExprPtr> operands;
    size_t height {1}; // The height of the expression's tree (see MAX_EXPRESSION_HEIGHT).
};

struct Stmt
{
    enum class Kind : uint8_t
    {
        EXPRESSION,
        DECLARATION,
        RETURN
    };

    Kind kind {Kind::EXPRESSION};
    ExprPtr expr; // EXPRESSION, RETURN (if any), and DECLARATION (single initialiser, if any).
    Symbol *variable {nullptr}; // DECLARATION.
    std::vector<ExprPtr> initialisers; // DECLARATION (initialiser list, if any).
};

using StmtPtr = std::unique_ptr<Stmt>;

struct FunctionDefinition
{
    Symbol *function {nullptr};
    std::vector<Symbol *> params;
    std::vector<StmtPtr> body;
};

// Parser (which also checks the semantics of the code).

enum class Storage : uint8_t
{
    NONE,
    TYPEDEF,
    EXTERN,
    STATIC
};

enum class Context : uint8_t
{
    TOP_LEVEL,
    BLOCK,
    OTHER
};

struct DeclSpec
{
    Storage storage {Storage::NONE};
    std::string_view exportName;
    TypePtr type {nullptr};
};

struct Param
{
    std::string_view name;
    Token token;
    TypePtr type {nullptr};
    bool isRestrict {false};
};

struct Declarator
{
    std::string_view name;
    Token token;
    TypePtr type {nullptr};
    bool isRestrict {false}; // Whether the declared pointer (not what it points to) is restrict-qualified.
    std::vector<Param> params; // The parameters of the function that is being declared, if any.
};

constexpr std::array KEYWORDS {
    std::string_view {"__attribute__"},
    std::string_view {"double"},
    std::string_view {"extern"},
    std::string_view {"int"},
    std::string_view {"long"},
    std::string_view {"restrict"},
    std::string_view {"return"},
    std::string_view {"static"},
    std::string_view {"struct"},
    std::string_view {"typedef"},
    std::string_view {"unsigned"},
    std::string_view {"void"},
};

constexpr std::array STORAGE_CLASSES {
    std::string_view {"typedef"},
    std::string_view {"extern"},
    std::string_view {"static"},
};

constexpr std::array TYPE_KEYWORDS {
    std::string_view {"double"},
    std::string_view {"int"},
    std::string_view {"struct"},
    std::string_view {"unsigned"},
    std::string_view {"void"},
};

// The maximum nesting depth of our expressions (i.e. of their parenthesised expressions, subscripts, arguments, second
// operands of the conditional operator, unary operators, and casts), beyond which we report an error rather than risk a
// stack overflow (as Clang does, which default bracket depth is also 256).

constexpr size_t MAX_NESTING_DEPTH {256};

// The maximum height of the tree of our expressions, beyond which we report an error rather than risk a stack overflow
// when generating some code for them (or when evaluating or deleting them). Indeed, unlike their nesting depth, the
// height of our expressions can grow without us recursing when parsing them, e.g. a + b + ... + z is a left-deep tree
// and c1 ? v1 : c2 ? v2 : ... : vn (i.e. a piecewise expression) a right-deep tree.
// Note: the height of a sum is (about) its number of terms and that of a piecewise expression its number of pieces,
//       both of which are typically much smaller than our maximum height.

constexpr size_t MAX_EXPRESSION_HEIGHT {1024};

constexpr std::array TWO_CHARACTER_PUNCTUATORS {
    std::string_view {"->"},
    std::string_view {"&&"},
    std::string_view {"||"},
    std::string_view {"=="},
    std::string_view {"!="},
    std::string_view {"<="},
    std::string_view {">="},
};

constexpr std::string_view ONE_CHARACTER_PUNCTUATORS {"(){}[];,+-*/!=<>^?:&"};

// Our binary operators and their precedence (the higher the value, the higher the precedence), see parseBinary().

constexpr std::array BINARY_OPERATORS {
    std::pair<std::string_view, size_t> {"||", 1},
    std::pair<std::string_view, size_t> {"&&", 2},
    std::pair<std::string_view, size_t> {"^", 3},
    std::pair<std::string_view, size_t> {"==", 4},
    std::pair<std::string_view, size_t> {"!=", 4},
    std::pair<std::string_view, size_t> {"<", 5},
    std::pair<std::string_view, size_t> {">", 5},
    std::pair<std::string_view, size_t> {"<=", 5},
    std::pair<std::string_view, size_t> {">=", 5},
    std::pair<std::string_view, size_t> {"+", 6},
    std::pair<std::string_view, size_t> {"-", 6},
    std::pair<std::string_view, size_t> {"*", 7},
    std::pair<std::string_view, size_t> {"/", 7},
};

// The radices of our integer constants.

constexpr unsigned int DECIMAL {10};
constexpr unsigned int OCTAL {8};

// A marker, for acceptSequence(), that stands for a string token.

constexpr std::string_view STRING_TOKEN {"\""};

bool isKeyword(std::string_view pText)
{
    return std::ranges::find(KEYWORDS, pText) != KEYWORDS.end();
}

size_t binaryPrecedence(const Token &pToken)
{
    // The precedence of the binary operator that the given token is, or 0 if it isn't one.

    for (const auto &[text, precedence] : BINARY_OPERATORS) {
        if (pToken.text == text) {
            return precedence;
        }
    }

    return 0;
}

// Keep track of the nesting depth of our expressions (see MAX_NESTING_DEPTH).

class DepthGuard
{
public:
    explicit DepthGuard(size_t &pDepth)
        : mDepth(pDepth)
    {
    }

    ~DepthGuard()
    {
        mDepth -= mLevels;
    }

    DepthGuard(const DepthGuard &pOther) = delete;
    DepthGuard(DepthGuard &&pOther) noexcept = delete;

    DepthGuard &operator=(const DepthGuard &pRhs) = delete;
    DepthGuard &operator=(DepthGuard &&pRhs) noexcept = delete;

    bool deeper()
    {
        // Go one level deeper and return whether we are now too deep.

        ++mDepth;
        ++mLevels;

        return mDepth > MAX_NESTING_DEPTH;
    }

private:
    size_t &mDepth;
    size_t mLevels {0};
};

class Parser
{
public:
    explicit Parser(const std::string &pCode, unsigned int pLongBits)
        : mCode(pCode)
        , mTypes(pLongBits)
    {
        mScopes.emplace_back();
    }

    bool parse()
    {
        if (!tokenise()) {
            return false;
        }

        while (current().kind != TokenKind::END) {
            if (!parseExternalDeclaration()) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] const std::string &error() const
    {
        return mError;
    }

    std::vector<FunctionDefinition> &functions()
    {
        return mFunctions;
    }

private:
    std::string_view mCode;
    Types mTypes; // Note: our types must outlive our parsing since our code generator uses them.
    std::string mError;

    std::vector<Token> mTokens;
    size_t mPosition {0};

    std::deque<Symbol> mSymbols;
    std::vector<std::unordered_map<std::string_view, Symbol *>> mScopes;
    std::vector<FunctionDefinition> mFunctions;

    FunctionDefinition *mFunction {nullptr};
    bool mHasReturn {false};
    size_t mDepth {0};

    // Errors.

    bool fail(size_t pLine, size_t pColumn, const std::string &pMessage)
    {
        // Retrieve the line where the error occurred and describe the error in the same way as Clang does.

        size_t lineStart {0};

        for (size_t line {1}; line < pLine; ++line) {
            lineStart = mCode.find('\n', lineStart) + 1;
        }

        auto lineEnd {std::min(mCode.find('\n', lineStart), mCode.size())};

        mError = std::format("{}:\n{:>5} | {}\n      | {}^", pMessage, pLine, mCode.substr(lineStart, lineEnd - lineStart),
                             std::string(pColumn - 1, ' '));

        return false;
    }

    bool fail(const Token &pToken, const std::string &pMessage)
    {
        return fail(pToken.line, pToken.column, pMessage);
    }

    ExprPtr failExpr(const Token &pToken, const std::string &pMessage)
    {
        fail(pToken, pMessage);

        return nullptr;
    }

    // Tokens.

    bool tokenise()
    {
        size_t line {1};
        size_t lineStart {0};
        size_t i {0};

        auto isIdentifierCharacter = [](char pCharacter) {
            return (std::isalnum(static_cast<unsigned char>(pCharacter)) != 0) || (pCharacter == '_');
        };
        auto isDigit = [this](size_t pIndex) {
            return (pIndex < mCode.size()) && (std::isdigit(static_cast<unsigned char>(mCode[pIndex])) != 0);
        };

        while (true) {
            // Skip white spaces.

            while ((i < mCode.size()) && (std::isspace(static_cast<unsigned char>(mCode[i])) != 0)) {
                if (mCode[i] == '\n') {
                    ++line;

                    lineStart = i + 1;
                }

                ++i;
            }

            Token token {.kind = TokenKind::END, .text = {}, .line = line, .column = i - lineStart + 1};

            if (i == mCode.size()) {
                mTokens.push_back(token);

                return true;
            }

            auto start {i};
            auto character {mCode[i]};

            if ((std::isalpha(static_cast<unsigned char>(character)) != 0) || (character == '_')) {
                while ((i < mCode.size()) && isIdentifierCharacter(mCode[i])) {
                    ++i;
                }

                token.kind = TokenKind::IDENTIFIER;
            } else if (isDigit(i) || ((character == '.') && isDigit(i + 1))) {
                // A number, i.e. an integer (e.g., 123) or a floating-point number (e.g., 123.0, .5, 5., or 1.0e-3).

                while (isDigit(i)) {
                    ++i;
                }

                if ((i < mCode.size()) && (mCode[i] == '.')) {
                    ++i;

                    while (isDigit(i)) {
                        ++i;
                    }
                }

                if ((i < mCode.size()) && ((mCode[i] == 'e') || (mCode[i] == 'E'))) {
                    auto exponent {i + 1};

                    if ((exponent < mCode.size()) && ((mCode[exponent] == '+') || (mCode[exponent] == '-'))) {
                        ++exponent;
                    }

                    if (!isDigit(exponent)) {
                        return fail(line, i - lineStart + 1, "Exponent has no digits");
                    }

                    i = exponent;

                    while (isDigit(i)) {
                        ++i;
                    }
                }

                if ((i < mCode.size()) && isIdentifierCharacter(mCode[i])) {
                    return fail(line, i - lineStart + 1, "Invalid suffix on numeric constant");
                }

                token.kind = TokenKind::NUMBER;
            } else if (character == '"') {
                i = mCode.find_first_of("\"\n", i + 1);

                if ((i == std::string_view::npos) || (mCode[i] != '"')) {
                    return fail(token, "Missing terminating '\"' character");
                }

                ++i;

                token.kind = TokenKind::STRING;
            } else {
                auto twoCharacterPunctuator {mCode.substr(i, 2)};

                if (std::ranges::find(TWO_CHARACTER_PUNCTUATORS, twoCharacterPunctuator) != TWO_CHARACTER_PUNCTUATORS.end()) {
                    i += 2;
                } else if (ONE_CHARACTER_PUNCTUATORS.find(character) != std::string_view::npos) {
                    ++i;
                } else {
                    return fail(token, std::format("Unexpected character '{}'", character));
                }

                token.kind = TokenKind::PUNCTUATOR;
            }

            token.text = mCode.substr(start, i - start);

            mTokens.push_back(token);
        }
    }

    [[nodiscard]] const Token &current() const
    {
        return mTokens[mPosition];
    }

    [[nodiscard]] const Token &next() const
    {
        return mTokens[std::min(mPosition + 1, mTokens.size() - 1)];
    }

    [[nodiscard]] static bool isToken(const Token &pToken, std::string_view pText)
    {
        return (pToken.kind != TokenKind::STRING) && (pToken.text == pText);
    }

    [[nodiscard]] bool isToken(std::string_view pText) const
    {
        return isToken(current(), pText);
    }

    bool accept(std::string_view pText)
    {
        if (isToken(pText)) {
            ++mPosition;

            return true;
        }

        return false;
    }

    bool acceptSequence(std::initializer_list<std::string_view> pTexts)
    {
        // Accept the given sequence of tokens, if it is there, where STRING_TOKEN stands for any string token.

        auto position {mPosition};

        for (auto text : pTexts) {
            const auto &token {mTokens[position]};

            if ((text == STRING_TOKEN) ? (token.kind != TokenKind::STRING) : !isToken(token, text)) {
                return false;
            }

            ++position;
        }

        mPosition = position;

        return true;
    }

    bool expect(std::string_view pText, const std::string &pMessage)
    {
        // Expect the given token, reporting an error right after the previous token (i.e. where the given token was
        // expected) if it is not there.

        if (accept(pText)) {
            return true;
        }

        const auto &previous {mTokens[mPosition - 1]};

        return fail(previous.line, previous.column + previous.text.size(), pMessage);
    }

    // Symbols.

    [[nodiscard]] Symbol *lookup(std::string_view pName) const
    {
        for (auto scope {mScopes.rbegin()}; scope != mScopes.rend(); ++scope) {
            auto symbol {scope->find(pName)};

            if (symbol != scope->end()) {
                return symbol->second;
            }
        }

        return nullptr;
    }

    Symbol *newSymbol(Symbol::Kind pKind, std::string_view pName, TypePtr pType)
    {
        auto &res {mSymbols.emplace_back()};

        res.kind = pKind;
        res.name = pName;
        res.type = pType;

        return &res;
    }

    Symbol *declare(Symbol::Kind pKind, std::string_view pName, TypePtr pType)
    {
        auto *res {newSymbol(pKind, pName, pType)};

        mScopes.back()[pName] = res;

        return res;
    }

    Symbol *builtin(std::string_view pName)
    {
        // Return the symbol for the given mathematical function, declaring it (globally) if needed.

        for (const auto &function : BUILTINS) {
            if (function.name == pName) {
                auto *res {newSymbol(Symbol::Kind::FUNCTION, pName,
                                     mTypes.functionOf(mTypes.doubleType(), std::vector<TypePtr>(function.paramCount, mTypes.doubleType())))};

                res->builtin = &function;

                mScopes.front()[pName] = res;

                return res;
            }
        }

        return nullptr;
    }

    [[nodiscard]] bool isTypedefName(const Token &pToken) const
    {
        auto *symbol {lookup(pToken.text)};

        return (symbol != nullptr) && (symbol->kind == Symbol::Kind::TYPEDEF);
    }

    [[nodiscard]] bool isTypeStart(const Token &pToken) const
    {
        return (std::ranges::find(TYPE_KEYWORDS, pToken.text) != TYPE_KEYWORDS.end()) || isTypedefName(pToken);
    }

    [[nodiscard]] bool isDeclarationStart(const Token &pToken) const
    {
        return (std::ranges::find(STORAGE_CLASSES, pToken.text) != STORAGE_CLASSES.end()) || isTypeStart(pToken);
    }

    // Declarations.

    bool parseDeclSpec(DeclSpec &pDeclSpec, Context pContext)
    {
        // Storage class (at most one), which must be valid in the given context, i.e. typedef or extern at the top
        // level, and static in a block.

        auto storageToken {current()};
        auto storageIndex {static_cast<size_t>(std::ranges::find(STORAGE_CLASSES, storageToken.text) - STORAGE_CLASSES.begin())};

        if (storageIndex < STORAGE_CLASSES.size()) {
            auto storage {static_cast<Storage>(storageIndex + 1)};

            if ((pContext == Context::OTHER) || ((storage == Storage::STATIC) == (pContext == Context::TOP_LEVEL))) {
                return fail(storageToken, std::format("Unsupported storage class '{}'", storageToken.text));
            }

            pDeclSpec.storage = storage;

            ++mPosition;
        }

        // Attributes.

        while (isToken("__attribute__")) {
            auto attributeToken {current()};

            if (!acceptSequence({"__attribute__", "(", "(", "export_name", "(", STRING_TOKEN, ")", ")", ")"})) {
                return fail(attributeToken, "Unsupported attribute (only __attribute__((export_name(\"<name>\"))) is supported)");
            }

            auto exportName {mTokens[mPosition - 4].text};

            pDeclSpec.exportName = exportName.substr(1, exportName.size() - 2);
        }

        // Type specifier.

        auto typeToken {current()};

        if (accept("double")) {
            pDeclSpec.type = mTypes.doubleType();
        } else if (accept("int")) {
            pDeclSpec.type = mTypes.intType();
        } else if (accept("void")) {
            pDeclSpec.type = mTypes.voidType();
        } else if (acceptSequence({"unsigned", "long", "long"})) {
            pDeclSpec.type = mTypes.unsignedLongLongType();
        } else if (acceptSequence({"unsigned", "long"})) {
            pDeclSpec.type = mTypes.unsignedLongType();
        } else if (accept("struct")) {
            return parseStruct(pDeclSpec);
        } else if (isTypedefName(typeToken)) {
            ++mPosition;

            pDeclSpec.type = lookup(typeToken.text)->type;
        } else {
            return fail(typeToken, "Expected a type specifier");
        }

        return true;
    }

    bool parseStruct(DeclSpec &pDeclSpec)
    {
        // A structure (without a tag since we only ever need to typedef it).

        if (!expect("{", "Expected '{' after 'struct'")) {
            return false;
        }

        auto *type {mTypes.newStruct()};

        while (!accept("}")) {
            DeclSpec declSpec;
            Declarator declarator;

            if (!parseDeclSpec(declSpec, Context::OTHER)
                || !parseDeclarator(declSpec.type, false, declarator)
                || !checkObject(declarator, "Field")
                || !expect(";", "Expected ';' at end of declaration list")) {
                return false;
            }

            if (std::ranges::any_of(type->fields, [&declarator](const Field &pField) {
                    return pField.name == declarator.name;
                })) {
                return fail(declarator.token, std::format("Duplicate member '{}'", declarator.name));
            }

            type->fields.push_back({declarator.name, declarator.type});
        }

        pDeclSpec.type = type;

        return true;
    }

    bool parseDeclarator(TypePtr pType, bool pAbstract, Declarator &pDeclarator)
    {
        // A declarator, i.e. a name (unless the declarator is abstract), possibly preceded by pointers (possibly
        // restrict-qualified) and followed by array sizes and/or function parameters (e.g., *name[3]). It can also be a
        // pointer to a function (e.g., (*name)(double, double)).
        // Note: as in C, a restrict qualifier isn't part of the type of what is being declared, but only of the pointer
        //       that it qualifies and it only matters (see generateFunction()) if it qualifies a parameter, i.e. if it
        //       follows the last '*' of a parameter declarator.

        const auto *type {pType};

        while (accept("*")) {
            type = mTypes.pointerTo(type);

            pDeclarator.isRestrict = accept("restrict");
        }

        auto isFunctionPointer {accept("(")};

        if (isFunctionPointer && !expect("*", "Expected '*'")) {
            return false;
        }

        pDeclarator.token = current();

        if ((current().kind == TokenKind::IDENTIFIER) && !isKeyword(current().text)) {
            pDeclarator.name = current().text;

            ++mPosition;
        } else if (!pAbstract) {
            return fail(current(), "Expected identifier or '('");
        }

        if (isFunctionPointer && !expect(")", "Expected ')'")) {
            return false;
        }

        // Array sizes and/or function parameters, which we apply from right to left (e.g., name[2][3] is an array of 2
        // arrays of 3 elements).

        struct Suffix
        {
            uint64_t arraySize; // 0 for function parameters.
            std::vector<TypePtr> params;
        };

        std::vector<Suffix> suffixes;

        while (true) {
            if (accept("[")) {
                auto sizeToken {current()};
                uint64_t size {0};

                if ((sizeToken.kind != TokenKind::NUMBER) || llvm::StringRef(sizeToken.text).getAsInteger(DECIMAL, size) || (size == 0)) {
                    return fail(sizeToken, "Expected a positive integer constant");
                }

                ++mPosition;

                if (!expect("]", "Expected ']'")) {
                    return false;
                }

                suffixes.push_back({size, {}});
            } else if (isToken("(")) {
                std::vector<Param> params;

                if (!parseParams(params)) {
                    return false;
                }

                std::vector<TypePtr> paramTypes;

                paramTypes.reserve(params.size());

                for (const auto &param : params) {
                    paramTypes.push_back(param.type);
                }

                if (suffixes.empty()) {
                    pDeclarator.params = params;
                }

                suffixes.push_back({0, paramTypes});
            } else {
                break;
            }
        }

        for (auto suffix {suffixes.rbegin()}; suffix != suffixes.rend(); ++suffix) {
            // Note: as in C, a function cannot return a function or an array, and an array cannot contain functions.

            auto isFunction {suffix->arraySize == 0};

            if ((type->kind == Type::Kind::FUNCTION) || (isFunction && (type->kind == Type::Kind::ARRAY))) {
                return fail(pDeclarator.token, std::format("Invalid declarator for '{}'", pDeclarator.name));
            }

            type = isFunction ?
                       mTypes.functionOf(type, suffix->params) :
                       mTypes.arrayOf(type, suffix->arraySize);
        }

        pDeclarator.type = isFunctionPointer ? mTypes.pointerTo(type) : type;

        return true;
    }

    bool parseParams(std::vector<Param> &pParams)
    {
        // Function parameters, i.e. (), (void), or a list of (possibly abstract) parameter declarations.
        // Note: as in C23, () means that the function has no parameters, i.e. it is the same as (void). (Our generator
        //       profile never declares a function without a prototype, so there is no need for us to support them.)

        ++mPosition;

        if (accept(")") || acceptSequence({"void", ")"})) {
            return true;
        }

        while (true) {
            DeclSpec declSpec;
            Declarator declarator;

            if (!parseDeclSpec(declSpec, Context::OTHER) || !parseDeclarator(declSpec.type, true, declarator)) {
                return false;
            }

            // An array parameter is a pointer to its first element and a function parameter is a pointer to it.

            const auto *type {declarator.type};

            if (type->kind == Type::Kind::ARRAY) {
                type = mTypes.pointerTo(type->element);
            } else if (type->kind == Type::Kind::FUNCTION) {
                type = mTypes.pointerTo(type);
            }

            if (type->kind == Type::Kind::VOID) {
                return fail(declarator.token, "Parameter has invalid type 'void'");
            }

            pParams.push_back({declarator.name, declarator.token, type, declarator.isRestrict});

            if (!accept(",")) {
                return expect(")", "Expected ')'");
            }
        }
    }

    bool checkObject(const Declarator &pDeclarator, std::string_view pWhat)
    {
        if (!isObject(pDeclarator.type)) {
            return fail(pDeclarator.token, std::format("{} has invalid type '{}'", pWhat, typeName(pDeclarator.type)));
        }

        return true;
    }

    bool checkRedefinition(const Declarator &pDeclarator)
    {
        if (mScopes.back().contains(pDeclarator.name)) {
            return fail(pDeclarator.token, std::format("Redefinition of '{}'", pDeclarator.name));
        }

        return true;
    }

    bool parseExternalDeclaration()
    {
        // A typedef, a function declaration, or a function definition.

        DeclSpec declSpec;
        Declarator declarator;

        if (!parseDeclSpec(declSpec, Context::TOP_LEVEL) || !parseDeclarator(declSpec.type, false, declarator)) {
            return false;
        }

        if (declSpec.storage == Storage::TYPEDEF) {
            if (!checkRedefinition(declarator)) {
                return false;
            }

            const auto *type {declarator.type};

            if ((type->kind == Type::Kind::STRUCT) && type->name.empty()) {
                const_cast<Type *>(type)->name = declarator.name; // NOLINT
            }

            declare(Symbol::Kind::TYPEDEF, declarator.name, type);

            return expect(";", "Expected ';' after top level declarator");
        }

        if (declarator.type->kind != Type::Kind::FUNCTION) {
            return fail(declarator.token, "Only functions and typedefs can be declared at the top level");
        }

        // Declare our function, making sure that it is consistent with any previous declaration of it.

        auto *function {mScopes.front().contains(declarator.name) ?
                            mScopes.front()[declarator.name] :
                            builtin(declarator.name)};

        if (function == nullptr) {
            function = declare(Symbol::Kind::FUNCTION, declarator.name, declarator.type);
        } else if ((function->kind != Symbol::Kind::FUNCTION) || (function->type != declarator.type)) {
            return fail(declarator.token, std::format("Conflicting types for '{}'", declarator.name));
        }

        if (!declSpec.exportName.empty()) {
            function->exportName = declSpec.exportName;
        }

        if (!isToken("{")) {
            return expect(";", "Expected function body after function declarator");
        }

        return parseFunctionDefinition(function, declarator);
    }

    bool parseFunctionDefinition(Symbol *pFunction, const Declarator &pDeclarator)
    {
        if (pFunction->isDefined || (pFunction->builtin != nullptr)) {
            return fail(pDeclarator.token, std::format("Redefinition of '{}'", pDeclarator.name));
        }

        pFunction->isDefined = true;

        mFunction = &mFunctions.emplace_back();
        mFunction->function = pFunction;
        mHasReturn = false;

        // Declare our parameters.

        mScopes.emplace_back();

        for (const auto &param : pDeclarator.params) {
            if (param.name.empty()) {
                return fail(param.token, "Parameter name omitted");
            }

            const Declarator paramDeclarator {.name = param.name, .token = param.token, .type = param.type, .params = {}};

            if (!checkRedefinition(paramDeclarator)) {
                return false;
            }

            mFunction->params.push_back(declare(Symbol::Kind::VARIABLE, param.name, param.type));
            mFunction->params.back()->isRestrict = param.isRestrict;
        }

        // Parse the body of our function.

        ++mPosition;

        while (!isToken("}")) {
            if (current().kind == TokenKind::END) {
                return expect("}", "Expected '}'");
            }

            if (!parseStatement()) {
                return false;
            }
        }

        if ((pFunction->type->element->kind != Type::Kind::VOID) && !mHasReturn) {
            return fail(current(), "Non-void function does not return a value");
        }

        ++mPosition;

        mScopes.pop_back();

        return true;
    }

    // Statements.

    static StmtPtr newStmt(Stmt::Kind pKind, ExprPtr pExpr)
    {
        auto res {std::make_unique<Stmt>()};

        res->kind = pKind;
        res->expr = std::move(pExpr);

        return res;
    }

    bool parseStatement()
    {
        if (isToken("return")) {
            return parseReturn();
        }

        if (isDeclarationStart(current())) {
            return parseDeclaration();
        }

        auto expr {parseAssignment()};

        if ((expr == nullptr) || !expect(";", "Expected ';' after expression")) {
            return false;
        }

        mFunction->body.push_back(newStmt(Stmt::Kind::EXPRESSION, rvalue(std::move(expr))));

        return true;
    }

    bool parseReturn()
    {
        auto returnToken {current()};
        const auto *returnType {mFunction->function->type->element};
        ExprPtr expr;

        ++mPosition;

        if (!isToken(";")) {
            expr = parseAssignment();

            if (expr == nullptr) {
                return false;
            }
        }

        if ((expr == nullptr) != (returnType->kind == Type::Kind::VOID)) {
            return fail(returnToken, std::format("{} function '{}' should {}return a value",
                                                 (expr == nullptr) ? "Non-void" : "Void",
                                                 mFunction->function->name,
                                                 (expr == nullptr) ? "" : "not "));
        }

        if (expr != nullptr) {
            expr = convert(std::move(expr), returnType);

            if (expr == nullptr) {
                return false;
            }
        }

        if (!expect(";", "Expected ';' after return statement")) {
            return false;
        }

        mFunction->body.push_back(newStmt(Stmt::Kind::RETURN, std::move(expr)));

        mHasReturn = true;

        return true;
    }

    bool parseDeclaration()
    {
        DeclSpec declSpec;
        Declarator declarator;

        if (!parseDeclSpec(declSpec, Context::BLOCK)
            || !parseDeclarator(declSpec.type, false, declarator)
            || !checkObject(declarator, "Variable")
            || !checkRedefinition(declarator)) {
            return false;
        }

        auto *variable {declare(Symbol::Kind::VARIABLE, declarator.name, declarator.type)};
        auto stmt {newStmt(Stmt::Kind::DECLARATION, nullptr)};

        stmt->variable = variable;
        variable->isStatic = declSpec.storage == Storage::STATIC;

        if (accept("=")) {
            if (variable->isStatic) {
                return fail(declarator.token, "Static variables cannot be initialised");
            }

            if (accept("{")) {
                // An initialiser list, which is only supported for a structure, all the fields of which must be
                // initialised (as is the case in the code that libCellML generates).

                const auto *type {variable->type};

                if (type->kind != Type::Kind::STRUCT) {
                    return fail(declarator.token, std::format("Invalid initialiser list for type '{}'", typeName(type)));
                }

                for (const auto &field : type->fields) {
                    if (!stmt->initialisers.empty() && !expect(",", "Expected ','")) {
                        return false;
                    }

                    auto element {parseAssignment()};

                    if (element == nullptr) {
                        return false;
                    }

                    element = convert(std::move(element), field.type);

                    if (element == nullptr) {
                        return false;
                    }

                    stmt->initialisers.push_back(std::move(element));
                }

                if (!expect("}", "Expected '}'")) {
                    return false;
                }
            } else {
                auto expr {parseAssignment()};

                if (expr == nullptr) {
                    return false;
                }

                stmt->expr = convert(std::move(expr), variable->type);

                if (stmt->expr == nullptr) {
                    return false;
                }
            }
        }

        if (!expect(";", "Expected ';' at end of declaration")) {
            return false;
        }

        mFunction->body.push_back(std::move(stmt));

        return true;
    }

    // Expressions.

    static ExprPtr newExpr(Expr::Kind pKind, TypePtr pType, const Token &pToken)
    {
        auto res {std::make_unique<Expr>()};

        res->kind = pKind;
        res->type = pType;
        res->token = pToken;

        return res;
    }

    static void addOperand(Expr &pExpr, ExprPtr pOperand)
    {
        pExpr.height = std::max(pExpr.height, pOperand->height + 1);

        pExpr.operands.push_back(std::move(pOperand));
    }

    ExprPtr checkHeight(const Token &pToken, ExprPtr pExpr)
    {
        // Make sure that the given expression is not too high (see MAX_EXPRESSION_HEIGHT).
        // Note: we do this for every expression that we build without recursing, i.e. for our assignments, conditional
        //       operators, and binary operators (see parseAssignment(), parseConditional(), and parseBinary()), as well
        //       as for our unary operators, casts, and postfix expressions (see parseCast()).

        if (pExpr->height > MAX_EXPRESSION_HEIGHT) {
            return failExpr(pToken, "Expression is too deeply nested");
        }

        return pExpr;
    }

    static ExprPtr newCast(ExprPtr pExpr, Expr::Cast pCast, TypePtr pType)
    {
        auto res {newExpr(Expr::Kind::CAST, pType, pExpr->token)};

        res->cast = pCast;

        addOperand(*res, std::move(pExpr));

        return res;
    }

    ExprPtr rvalue(ExprPtr pExpr)
    {
        // Convert the given expression to an rvalue, i.e. a function to a pointer to it, an array to a pointer to its
        // first element, and an lvalue to its value.

        if (pExpr->type->kind == Type::Kind::FUNCTION) {
            pExpr->type = mTypes.pointerTo(pExpr->type);

            return pExpr;
        }

        if (pExpr->type->kind == Type::Kind::ARRAY) {
            const auto *type {mTypes.pointerTo(pExpr->type->element)};

            return newCast(std::move(pExpr), Expr::Cast::DECAY, type);
        }

        if (pExpr->isLvalue) {
            const auto *type {pExpr->type};

            return newCast(std::move(pExpr), Expr::Cast::LOAD, type);
        }

        return pExpr;
    }

    static ExprPtr convertArithmetic(ExprPtr pExpr, TypePtr pType)
    {
        // Convert the given arithmetic rvalue to the given arithmetic type.

        if (pExpr->type == pType) {
            return pExpr;
        }

        if (pType->kind == Type::Kind::DOUBLE) {
            return newCast(std::move(pExpr), Expr::Cast::INTEGER_TO_DOUBLE, pType);
        }

        auto cast {(pExpr->type->kind == Type::Kind::DOUBLE) ? Expr::Cast::DOUBLE_TO_INTEGER : Expr::Cast::INTEGER_TO_INTEGER};

        return newCast(std::move(pExpr), cast, pType);
    }

    ExprPtr convert(ExprPtr pExpr, TypePtr pType)
    {
        // Implicitly convert the given expression to the given type, i.e. an arithmetic value to another arithmetic
        // type and a pointer to void *.

        auto res {rvalue(std::move(pExpr))};

        if (res->type != pType) {
            if (isArithmetic(res->type) && isArithmetic(pType)) {
                res = convertArithmetic(std::move(res), pType);
            } else if ((res->type->kind == Type::Kind::POINTER) && (pType == mTypes.pointerTo(mTypes.voidType()))) {
                res = newCast(std::move(res), Expr::Cast::POINTER_TO_POINTER, pType);
            } else {
                res = failExpr(res->token, std::format("Cannot convert '{}' to '{}'", typeName(res->type), typeName(pType)));
            }
        }

        return res;
    }

    TypePtr commonType(TypePtr pType1, TypePtr pType2) const
    {
        // The usual arithmetic conversions, i.e. double if any of the types is double, otherwise the widest integer
        // type (or the unsigned one if they are equally wide).

        if ((pType1->kind == Type::Kind::DOUBLE) || (pType2->kind == Type::Kind::DOUBLE)) {
            return mTypes.doubleType();
        }

        if (pType1->bits != pType2->bits) {
            return (pType1->bits > pType2->bits) ? pType1 : pType2;
        }

        return pType1->isSigned ? pType2 : pType1;
    }

    // Note: to parse an expression, we only recurse for what is nested in it (i.e. a parenthesised expression, a
    //       subscript, an argument, and the second operand of the conditional operator), so that its nesting depth is
    //       what determines how much stack we use (see MAX_NESTING_DEPTH). Also, we check the operands of an operator
    //       in a function that is not inlined, so that its local variables don't use any stack while we are parsing
    //       some nested expression.

    ExprPtr parseAssignment()
    {
        // An assignment, which is right associative, i.e. a = b = c is a = (b = c).

        std::vector<std::pair<const Token *, ExprPtr>> lhss;
        auto expr {parseConditional()};

        while ((expr != nullptr) && isToken("=")) {
            lhss.emplace_back(&current(), std::move(expr));

            ++mPosition;

            expr = parseConditional();
        }

        while ((expr != nullptr) && !lhss.empty()) {
            expr = newAssignment(*lhss.back().first, std::move(lhss.back().second), std::move(expr));

            lhss.pop_back();
        }

        return expr;
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newAssignment(const Token &pToken, ExprPtr pLhs, ExprPtr pRhs)
    {
        if (!pLhs->isLvalue || (pLhs->type->kind == Type::Kind::ARRAY)) {
            return failExpr(pToken, "Expression is not assignable");
        }

        auto rhs {convert(std::move(pRhs), pLhs->type)};

        if (rhs == nullptr) {
            return nullptr;
        }

        auto res {newExpr(Expr::Kind::ASSIGNMENT, pLhs->type, pToken)};

        addOperand(*res, std::move(pLhs));
        addOperand(*res, std::move(rhs));

        return checkHeight(pToken, std::move(res));
    }

    ExprPtr parseConditional()
    {
        // A conditional operator, which is right associative, i.e. a ? b : c ? d : e is a ? b : (c ? d : e).

        std::vector<std::tuple<const Token *, ExprPtr, ExprPtr>> conditionsAndTrueExprs;
        auto expr {parseBinary()};

        while ((expr != nullptr) && isToken("?")) {
            const auto &questionToken {current()};

            ++mPosition;

            // Note: our expression is now nullptr, which is what we return if something goes wrong.

            auto condition {newCondition(questionToken, std::move(expr))};
            auto trueExpr {(condition != nullptr) ? parseAssignment() : nullptr};

            if ((trueExpr == nullptr) || !expect(":", "Expected ':'")) {
                break;
            }

            conditionsAndTrueExprs.emplace_back(&questionToken, std::move(condition), std::move(trueExpr));

            expr = parseBinary();
        }

        while ((expr != nullptr) && !conditionsAndTrueExprs.empty()) {
            auto &[questionToken, condition, trueExpr] {conditionsAndTrueExprs.back()};

            expr = newConditional(*questionToken, std::move(condition), std::move(trueExpr), std::move(expr));

            conditionsAndTrueExprs.pop_back();
        }

        return expr;
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newCondition(const Token &pQuestionToken, ExprPtr pCondition)
    {
        auto res {rvalue(std::move(pCondition))};

        if (!isScalar(res->type)) {
            res = failExpr(pQuestionToken, std::format("Used type '{}' where arithmetic or pointer type is required", typeName(res->type)));
        }

        return res;
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newConditional(const Token &pQuestionToken, ExprPtr pCondition, ExprPtr pTrueExpr,
                                                   ExprPtr pFalseExpr)
    {
        auto trueExpr {rvalue(std::move(pTrueExpr))};
        auto falseExpr {rvalue(std::move(pFalseExpr))};
        TypePtr type {nullptr};

        if (isArithmetic(trueExpr->type) && isArithmetic(falseExpr->type)) {
            type = commonType(trueExpr->type, falseExpr->type);
            trueExpr = convertArithmetic(std::move(trueExpr), type);
            falseExpr = convertArithmetic(std::move(falseExpr), type);
        } else if (trueExpr->type == falseExpr->type) {
            type = trueExpr->type;
        } else {
            return failExpr(pQuestionToken, std::format("Incompatible operand types ('{}' and '{}')",
                                                        typeName(trueExpr->type), typeName(falseExpr->type)));
        }

        auto res {newExpr(Expr::Kind::CONDITIONAL, type, pQuestionToken)};

        addOperand(*res, std::move(pCondition));
        addOperand(*res, std::move(trueExpr));
        addOperand(*res, std::move(falseExpr));

        return checkHeight(pQuestionToken, std::move(res));
    }

    ExprPtr parseBinary()
    {
        // Our binary operators, which are all left associative, parsed using operator precedence, i.e. the left operand
        // of an operator waits for its right operand until we come across an operator that has the same or a lower
        // precedence.

        std::vector<std::tuple<const Token *, size_t, ExprPtr>> lhss;
        auto expr {parseCast()};

        while (expr != nullptr) {
            const auto &operatorToken {current()};
            auto precedence {binaryPrecedence(operatorToken)};

            if (precedence == 0) {
                break;
            }

            while ((expr != nullptr) && !lhss.empty() && (std::get<1>(lhss.back()) >= precedence)) {
                expr = newBinary(*std::get<0>(lhss.back()), std::move(std::get<2>(lhss.back())), std::move(expr));

                lhss.pop_back();
            }

            if (expr == nullptr) {
                break;
            }

            lhss.emplace_back(&operatorToken, precedence, std::move(expr));

            ++mPosition;

            expr = parseCast();
        }

        while ((expr != nullptr) && !lhss.empty()) {
            expr = newBinary(*std::get<0>(lhss.back()), std::move(std::get<2>(lhss.back())), std::move(expr));

            lhss.pop_back();
        }

        return expr;
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newBinary(const Token &pToken, ExprPtr pLhs, ExprPtr pRhs)
    {
        auto lhs {rvalue(std::move(pLhs))};
        auto rhs {rvalue(std::move(pRhs))};
        auto op {pToken.text};
        auto isLogical {(op == "&&") || (op == "||")};
        auto isXor {op == "^"};
        auto invalidOperands = [&]() {
            return failExpr(pToken, std::format("Invalid operands to binary expression ('{}' and '{}')",
                                                typeName(lhs->type), typeName(rhs->type)));
        };

        if (isLogical) {
            if (!isScalar(lhs->type) || !isScalar(rhs->type)) {
                return invalidOperands();
            }

            auto res {newExpr(Expr::Kind::LOGICAL, mTypes.intType(), pToken)};

            addOperand(*res, std::move(lhs));
            addOperand(*res, std::move(rhs));

            return checkHeight(pToken, std::move(res));
        }

        if (!isArithmetic(lhs->type) || !isArithmetic(rhs->type)
            || (isXor && ((lhs->type->kind == Type::Kind::DOUBLE) || (rhs->type->kind == Type::Kind::DOUBLE)))) {
            return invalidOperands();
        }

        const auto *type {commonType(lhs->type, rhs->type)};
        auto isArithmeticOperator {(op == "+") || (op == "-") || (op == "*") || (op == "/")};
        auto res {newExpr(Expr::Kind::BINARY, (isArithmeticOperator || isXor) ? type : mTypes.intType(), pToken)};

        addOperand(*res, convertArithmetic(std::move(lhs), type));
        addOperand(*res, convertArithmetic(std::move(rhs), type));

        return checkHeight(pToken, std::move(res));
    }

    ExprPtr parseCast()
    {
        // A postfix expression preceded by some unary operators and/or casts, which apply from right to left, i.e.
        // -(double) !x is -((double) (!x)).
        // Note: this is where we keep track of the nesting depth of our expressions since a parenthesised expression, a
        //       subscript, an argument, and the second operand of the conditional operator are all parsed from here, and
        //       since our unary operators and casts are nested in one another.

        DepthGuard depthGuard(mDepth);

        if (depthGuard.deeper()) {
            return failExpr(current(), "Expression is too deeply nested");
        }

        std::vector<std::pair<const Token *, TypePtr>> prefixes; // An operator or a cast (i.e. a type).

        while (true) {
            const auto &token {current()};
            auto isCast {isToken("(") && isTypeStart(next())};

            if (!isCast && !isToken("-") && !isToken("!") && !isToken("&")) {
                break;
            }

            if (depthGuard.deeper()) {
                return failExpr(token, "Expression is too deeply nested");
            }

            ++mPosition;

            TypePtr type {nullptr};

            if (isCast) {
                type = parseTypeName();

                if (type == nullptr) {
                    return nullptr;
                }
            }

            prefixes.emplace_back(&token, type);
        }

        auto expr {parsePrimary()};

        while (expr != nullptr) {
            const auto &operatorToken {current()};

            if (accept("[")) {
                expr = parseIndex(operatorToken, std::move(expr));
            } else if (accept("(")) {
                expr = parseCall(operatorToken, std::move(expr));
            } else if (accept("->")) {
                expr = parseMember(operatorToken, std::move(expr));
            } else {
                break;
            }
        }

        while ((expr != nullptr) && !prefixes.empty()) {
            auto [token, type] {prefixes.back()};

            expr = (type == nullptr) ? newUnary(*token, std::move(expr)) : newExplicitCast(*token, std::move(expr), type);

            prefixes.pop_back();
        }

        return (expr != nullptr) ? checkHeight(current(), std::move(expr)) : nullptr;
    }

    LLVM_ATTRIBUTE_NOINLINE TypePtr parseTypeName()
    {
        // The type name of a cast, which opening parenthesis has already been accepted.

        DeclSpec declSpec;
        Declarator declarator;

        if (!parseDeclSpec(declSpec, Context::OTHER)
            || !parseDeclarator(declSpec.type, true, declarator)) {
            return nullptr;
        }

        if (!declarator.name.empty()) {
            fail(declarator.token, "Expected ')'");

            return nullptr;
        }

        if (!expect(")", "Expected ')'")) {
            return nullptr;
        }

        return declarator.type;
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newExplicitCast(const Token &pToken, ExprPtr pExpr, TypePtr pType)
    {
        // A cast, i.e. from a pointer to another pointer or to an integer, or from an arithmetic type to another.

        auto expr {rvalue(std::move(pExpr))};
        const auto *fromType {expr->type};

        // Note: as Clang does, we keep track of a cast to the same type since it is an expression in its own right
        //       (e.g., (double) (a ? b : c) is not a conditional operator when it comes to branching on it).

        if (isArithmetic(fromType) && isArithmetic(pType)) {
            return (fromType == pType) ?
                       newCast(std::move(expr), Expr::Cast::NONE, pType) :
                       convertArithmetic(std::move(expr), pType);
        }

        if (fromType->kind == Type::Kind::POINTER) {
            if (pType->kind == Type::Kind::POINTER) {
                return newCast(std::move(expr), Expr::Cast::POINTER_TO_POINTER, pType);
            }

            if (pType->kind == Type::Kind::INTEGER) {
                return newCast(std::move(expr), Expr::Cast::POINTER_TO_INTEGER, pType);
            }
        }

        return failExpr(pToken, std::format("Invalid cast from '{}' to '{}'", typeName(fromType), typeName(pType)));
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newUnary(const Token &pToken, ExprPtr pOperand)
    {
        if (pToken.text == "&") {
            if (pOperand->kind == Expr::Kind::FUNCTION) {
                return rvalue(std::move(pOperand));
            }

            if (!pOperand->isLvalue) {
                return failExpr(pToken, std::format("Cannot take the address of an rvalue of type '{}'", typeName(pOperand->type)));
            }

            auto res {newExpr(Expr::Kind::ADDRESS, mTypes.pointerTo(pOperand->type), pToken)};

            addOperand(*res, std::move(pOperand));

            return res;
        }

        auto operand {rvalue(std::move(pOperand))};
        auto isMinus {pToken.text == "-"};

        if (!(isMinus ? isArithmetic(operand->type) : isScalar(operand->type))) {
            return failExpr(pToken, std::format("Invalid argument type '{}' to unary expression", typeName(operand->type)));
        }

        auto res {newExpr(Expr::Kind::UNARY, isMinus ? operand->type : mTypes.intType(), pToken)};

        addOperand(*res, std::move(operand));

        return res;
    }

    ExprPtr parseIndex(const Token &pToken, ExprPtr pBase)
    {
        auto index {parseAssignment()};

        if ((index == nullptr) || !expect("]", "Expected ']'")) {
            return nullptr;
        }

        return newIndex(pToken, std::move(pBase), std::move(index));
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newIndex(const Token &pToken, ExprPtr pBase, ExprPtr pIndex)
    {
        auto base {rvalue(std::move(pBase))};
        auto index {rvalue(std::move(pIndex))};

        if ((base->type->kind != Type::Kind::POINTER) || !isObject(base->type->element)) {
            return failExpr(pToken, "Subscripted value is not an array or a pointer");
        }

        if (index->type->kind != Type::Kind::INTEGER) {
            return failExpr(index->token, "Array subscript is not an integer");
        }

        auto res {newExpr(Expr::Kind::INDEX, base->type->element, pToken)};

        res->isLvalue = true;

        addOperand(*res, std::move(base));
        addOperand(*res, std::move(index));

        return res;
    }

    ExprPtr parseCall(const Token &pToken, ExprPtr pCallee)
    {
        std::vector<ExprPtr> args;

        if (!accept(")")) {
            while (true) {
                auto arg {parseAssignment()};

                if (arg == nullptr) {
                    return nullptr;
                }

                args.push_back(std::move(arg));

                if (!accept(",")) {
                    break;
                }
            }

            if (!expect(")", "Expected ')'")) {
                return nullptr;
            }
        }

        return newCall(pToken, std::move(pCallee), args);
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr newCall(const Token &pToken, ExprPtr pCallee, std::vector<ExprPtr> &pArgs)
    {
        // Retrieve the type of the function that is being called (a function having been converted to a pointer to
        // it).

        auto callee {rvalue(std::move(pCallee))};
        const auto *type {callee->type};

        if ((type->kind != Type::Kind::POINTER) || (type->element->kind != Type::Kind::FUNCTION)) {
            return failExpr(pToken, std::format("Called object type '{}' is not a function or function pointer", typeName(type)));
        }

        type = type->element;

        if (pArgs.size() != type->params.size()) {
            return failExpr(pToken, std::format("Expected {} argument(s), got {}", type->params.size(), pArgs.size()));
        }

        for (size_t i {0}; i < pArgs.size(); ++i) {
            pArgs[i] = convert(std::move(pArgs[i]), type->params[i]);

            if (pArgs[i] == nullptr) {
                return nullptr;
            }
        }

        auto res {newExpr(Expr::Kind::CALL, type->element, pToken)};

        res->operands.reserve(1 + pArgs.size());
        addOperand(*res, std::move(callee));

        for (auto &arg : pArgs) {
            addOperand(*res, std::move(arg));
        }

        return res;
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr parseMember(const Token &pToken, ExprPtr pBase)
    {
        auto base {rvalue(std::move(pBase))};
        const auto &fieldToken {current()};

        if ((base->type->kind != Type::Kind::POINTER) || (base->type->element->kind != Type::Kind::STRUCT)) {
            return failExpr(pToken, std::format("Member reference type '{}' is not a pointer to a structure", typeName(base->type)));
        }

        const auto &fields {base->type->element->fields};
        auto field {std::ranges::find_if(fields, [&fieldToken](const Field &pField) {
            return pField.name == fieldToken.text;
        })};

        if ((fieldToken.kind != TokenKind::IDENTIFIER) || (field == fields.end())) {
            return failExpr(fieldToken, std::format("No member named '{}'", fieldToken.text));
        }

        ++mPosition;

        auto res {newExpr(Expr::Kind::MEMBER, field->type, pToken)};

        res->isLvalue = true;
        res->field = static_cast<size_t>(field - fields.begin());

        addOperand(*res, std::move(base));

        return res;
    }

    ExprPtr parsePrimary()
    {
        // A primary expression, i.e. a parenthesised expression, a number, or an identifier.

        if (accept("(")) {
            auto res {parseAssignment()};

            if ((res != nullptr) && !expect(")", "Expected ')'")) {
                res = nullptr;
            }

            return res;
        }

        return (current().kind == TokenKind::NUMBER) ? parseNumber() : parseIdentifier();
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr parseNumber()
    {
        const auto &token {current()};

        ++mPosition;

        if (token.text.find_first_of(".eE") != std::string_view::npos) {
            llvm::APFloat value(llvm::APFloat::IEEEdouble());

            llvm::cantFail(value.convertFromString(token.text, llvm::APFloat::rmNearestTiesToEven));

            auto res {newExpr(Expr::Kind::NUMBER, mTypes.doubleType(), token)};

            res->doubleValue = value.convertToDouble();

            return res;
        }

        // Note: as in C, an integer constant with a leading 0 is an octal constant, and an integer constant is an int if
        //       it fits in one, otherwise a long long (and it must fit in one).

        uint64_t value {0};

        if (llvm::StringRef(token.text).getAsInteger((token.text.size() > 1) && (token.text[0] == '0') ? OCTAL : DECIMAL, value)
            || (value > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))) {
            return failExpr(token, "Invalid integer constant");
        }

        auto res {newExpr(Expr::Kind::NUMBER,
                          (value <= static_cast<uint64_t>(std::numeric_limits<int32_t>::max())) ? mTypes.intType() : mTypes.longLongType(),
                          token)};

        res->integerValue = value;

        return res;
    }

    LLVM_ATTRIBUTE_NOINLINE ExprPtr parseIdentifier()
    {
        const auto &token {current()};

        if ((token.kind != TokenKind::IDENTIFIER) || isKeyword(token.text)) {
            return failExpr(token, "Expected expression");
        }

        ++mPosition;

        // INFINITY and NAN, unless they have been declared (as a variable, a function, or a typedef).

        auto *symbol {lookup(token.text)};

        if ((symbol == nullptr) && ((token.text == "INFINITY") || (token.text == "NAN"))) {
            auto res {newExpr(Expr::Kind::NUMBER, mTypes.doubleType(), token)};

            res->doubleValue = (token.text == "INFINITY") ?
                                   std::numeric_limits<double>::infinity() :
                                   std::numeric_limits<double>::quiet_NaN();

            return res;
        }

        if (symbol == nullptr) {
            symbol = builtin(token.text);

            if (symbol == nullptr) {
                return failExpr(token, std::format("Use of undeclared identifier '{}'", token.text));
            }
        }

        if (symbol->kind == Symbol::Kind::TYPEDEF) {
            return failExpr(token, std::format("Unexpected type name '{}': expected expression", token.text));
        }

        auto res {newExpr((symbol->kind == Symbol::Kind::VARIABLE) ? Expr::Kind::VARIABLE : Expr::Kind::FUNCTION,
                          symbol->type, token)};

        res->isLvalue = symbol->kind == Symbol::Kind::VARIABLE;
        res->symbol = symbol;

        return res;
    }
};

// Code generator.

class CodeGenerator
{
public:
    explicit CodeGenerator(llvm::Module &pModule, const IrGeneratorTarget &pTarget)
        : mModule(pModule)
        , mContext(pModule.getContext())
        , mTarget(pTarget)
        , mBuilder(mContext)
        , mAllocaBuilder(mContext)
        , mMdBuilder(mContext)
        , mTbaaChar(mMdBuilder.createTBAAScalarTypeNode("omnipotent char", mMdBuilder.createTBAARoot("Simple C/C++ TBAA")))
    {
    }

    void generate(std::vector<FunctionDefinition> &pFunctions)
    {
        for (auto &definition : pFunctions) {
            generateFunction(definition);
        }

        // Make sure that our exported functions are kept (as Clang does since export_name implies used).

        std::vector<llvm::GlobalValue *> exportedFunctions;

        for (auto &definition : pFunctions) {
            if (!definition.function->exportName.empty()) {
                exportedFunctions.push_back(llvm::cast<llvm::Function>(definition.function->value));
            }
        }

        if (!exportedFunctions.empty()) {
            llvm::appendToUsed(mModule, exportedFunctions);
        }

        // Let LLVM know how errno is accessed (as Clang does).

        auto *intTypeNode {tbaaScalar("int", mTbaaChar)};

        mModule.getOrInsertNamedMetadata("llvm.errno.tbaa")->addOperand(mMdBuilder.createTBAAStructTagNode(intTypeNode, intTypeNode, 0));
    }

private:
    llvm::Module &mModule;
    llvm::LLVMContext &mContext;
    const IrGeneratorTarget &mTarget;
    llvm::IRBuilder<> mBuilder;
    llvm::IRBuilder<> mAllocaBuilder;
    llvm::MDBuilder mMdBuilder;
    llvm::MDNode *mTbaaChar;
    std::map<std::string, llvm::MDNode *> mTbaaScalars;
    std::vector<llvm::MDNode *> mTbaaAnyPointers;
    std::unordered_map<TypePtr, llvm::MDNode *> mTbaaStructs;
    std::unordered_map<TypePtr, llvm::StructType *> mStructs;
    std::string_view mFunctionName;
    llvm::AllocaInst *mLastAlloca {nullptr};
    std::vector<llvm::Value *> mLocalVariables;

    llvm::Type *llvmType(TypePtr pType)
    {
        switch (pType->kind) {
        case Type::Kind::VOID:
            return mBuilder.getVoidTy();
        case Type::Kind::INTEGER:
            return mBuilder.getIntNTy(pType->bits);
        case Type::Kind::DOUBLE:
            return mBuilder.getDoubleTy();
        case Type::Kind::POINTER:
            return mBuilder.getPtrTy();
        case Type::Kind::ARRAY:
            return llvm::ArrayType::get(llvmType(pType->element), pType->count);
        default: // Type::Kind::STRUCT.
            // Note: we never need the LLVM type of a function type since we only ever deal with pointers to functions.

            return llvmStructType(pType);
        }
    }

    llvm::StructType *llvmStructType(TypePtr pType)
    {
        auto &res {mStructs[pType]};

        if (res == nullptr) {
            std::vector<llvm::Type *> fieldTypes;

            fieldTypes.reserve(pType->fields.size());

            for (const auto &field : pType->fields) {
                fieldTypes.push_back(llvmType(field.type));
            }

            res = llvm::StructType::create(mContext, fieldTypes, "struct." + pType->name);
        }

        return res;
    }

    uint64_t fieldOffset(TypePtr pStructType, size_t pField)
    {
        return mModule.getDataLayout().getStructLayout(llvmStructType(pStructType))->getElementOffset(static_cast<unsigned int>(pField));
    }

    llvm::Constant *intPtrZero()
    {
        return llvm::ConstantInt::get(mModule.getDataLayout().getIntPtrType(mContext), 0);
    }

    llvm::FunctionType *llvmFunctionType(TypePtr pType)
    {
        std::vector<llvm::Type *> paramTypes;

        paramTypes.reserve(pType->params.size());

        for (const auto *paramType : pType->params) {
            paramTypes.push_back(llvmType(paramType));
        }

        return llvm::FunctionType::get(llvmType(pType->element), paramTypes, false);
    }

    // Type-based alias analysis (TBAA) metadata, as generated by Clang (see CodeGenTBAA.cpp), i.e. using struct-path
    // TBAA and pointer TBAA.

    llvm::MDNode *tbaaScalar(const std::string &pName, llvm::MDNode *pParent)
    {
        auto &res {mTbaaScalars[pName]};

        if (res == nullptr) {
            res = mMdBuilder.createTBAAScalarTypeNode(pName, pParent);
        }

        return res;
    }

    llvm::MDNode *tbaaAnyPointer(size_t pDepth)
    {
        // The "any" pointer type nodes, i.e. "any pointer", "any p2 pointer", "any p3 pointer", etc., each of which
        // has the previous one as its parent.

        while (mTbaaAnyPointers.size() < pDepth) {
            mTbaaAnyPointers.push_back(mTbaaAnyPointers.empty() ?
                                           tbaaScalar("any pointer", mTbaaChar) :
                                           tbaaScalar(std::format("any p{} pointer", mTbaaAnyPointers.size() + 1), mTbaaAnyPointers.back()));
        }

        return mTbaaAnyPointers[pDepth - 1];
    }

    static std::string tbaaIntegerName(TypePtr pType)
    {
        // Note: as Clang does, we use the same type node for signed and unsigned integers.

        static constexpr std::string_view UNSIGNED {"unsigned "};

        return pType->name.starts_with(UNSIGNED) ? pType->name.substr(UNSIGNED.size()) : pType->name;
    }

    llvm::MDNode *tbaaType(TypePtr pType)
    {
        // The type node for the given type, i.e. a scalar type node for a double, an integer or a pointer ("p<depth>
        // <type>" for a pointer to a double or an integer, and an "any" pointer otherwise), a struct type node for a
        // structure, and "omnipotent char" for anything else (e.g., an array).

        switch (pType->kind) {
        case Type::Kind::DOUBLE:
            return tbaaScalar("double", mTbaaChar);
        case Type::Kind::INTEGER:
            return tbaaScalar(tbaaIntegerName(pType), mTbaaChar);
        case Type::Kind::POINTER: {
            size_t depth {0};
            const auto *type {pType};

            while (type->kind == Type::Kind::POINTER) {
                ++depth;

                type = type->element;
            }

            if ((type->kind != Type::Kind::DOUBLE) && (type->kind != Type::Kind::INTEGER)) {
                return tbaaAnyPointer(depth);
            }

            return tbaaScalar(std::format("p{} {}", depth, (type->kind == Type::Kind::DOUBLE) ? "double" : tbaaIntegerName(type)),
                              tbaaAnyPointer(depth));
        }
        case Type::Kind::STRUCT: {
            // Note: our structures have no tag, so their name is empty (as it is for Clang in C).

            auto &res {mTbaaStructs[pType]};

            if (res == nullptr) {
                std::vector<std::pair<llvm::MDNode *, uint64_t>> fields;

                for (size_t i {0}; i < pType->fields.size(); ++i) {
                    fields.emplace_back(tbaaType(pType->fields[i].type), fieldOffset(pType, i));
                }

                res = mMdBuilder.createTBAAStructTypeNode("", fields);
            }

            return res;
        }
        default:
            return mTbaaChar;
        }
    }

    llvm::MDNode *tbaaAccessTag(const Expr &pLvalue)
    {
        // The access tag for the given lvalue, i.e. a struct-path access tag for a field of a structure, and a scalar
        // access tag otherwise.

        if (pLvalue.kind == Expr::Kind::MEMBER) {
            const auto *structType {pLvalue.operands[0]->type->element};

            return mMdBuilder.createTBAAStructTagNode(tbaaType(structType), tbaaType(pLvalue.type),
                                                      fieldOffset(structType, pLvalue.field));
        }

        return tbaaScalarAccessTag(pLvalue.type);
    }

    llvm::MDNode *tbaaScalarAccessTag(TypePtr pType)
    {
        auto *typeNode {tbaaType(pType)};

        return mMdBuilder.createTBAAStructTagNode(typeNode, typeNode, 0);
    }

    llvm::Align alignment(TypePtr pType)
    {
        return mModule.getDataLayout().getABITypeAlign(llvmType(pType));
    }

    llvm::Align variableAlignment(TypePtr pType)
    {
        // The alignment of a variable, i.e. that of its type, unless it is a large array, in which case it may have to
        // be aligned further (as Clang does, see ASTContext::getDeclAlign()).

        auto res {alignment(pType)};

        if ((pType->kind == Type::Kind::ARRAY) && (mTarget.largeArrayAlignment != 0)
            && (mModule.getDataLayout().getTypeSizeInBits(llvmType(pType)) >= mTarget.largeArrayMinBits)) {
            res = std::max(res, llvm::Align(mTarget.largeArrayAlignment / CHAR_BIT));
        }

        return res;
    }

    llvm::Align accessAlignment(const Expr &pLvalue)
    {
        // The alignment with which the given lvalue can be accessed, i.e. that of its variable, of the field of its
        // structure (given the alignment of its structure and the offset of its field), or of its array element (given
        // the alignment of its array and, if known, the offset of its element), as Clang does.

        switch (pLvalue.kind) {
        case Expr::Kind::VARIABLE:
            return variableAlignment(pLvalue.type);
        case Expr::Kind::MEMBER:
            return llvm::commonAlignment(alignment(pLvalue.operands[0]->type->element),
                                         fieldOffset(pLvalue.operands[0]->type->element, pLvalue.field));
        default: { // Expr::Kind::INDEX.
            const auto &base {*pLvalue.operands[0]};
            auto elementSize {mModule.getDataLayout().getTypeAllocSize(llvmType(pLvalue.type)).getFixedValue()};

            if ((base.kind != Expr::Kind::CAST) || (base.cast != Expr::Cast::DECAY)) {
                return alignment(pLvalue.type);
            }

            auto arrayAlignment {accessAlignment(*base.operands[0])};
            const auto &index {*pLvalue.operands[1]};

            if (index.kind == Expr::Kind::NUMBER) {
                return llvm::commonAlignment(arrayAlignment, index.integerValue * elementSize);
            }

            return llvm::commonAlignment(arrayAlignment, elementSize);
        }
        }
    }

    template<typename T>
    static void addMathematicalFunctionAttributes(T *pFunctionOrCall)
    {
        pFunctionOrCall->addFnAttr(llvm::Attribute::NoUnwind);
        pFunctionOrCall->addFnAttr(llvm::Attribute::WillReturn);
        pFunctionOrCall->setDoesNotAccessMemory();
    }

    static void addAttributes(llvm::Function *pFunction, const std::vector<std::pair<std::string, std::string>> &pAttributes)
    {
        for (const auto &[name, value] : pAttributes) {
            pFunction->addFnAttr(name, value);
        }
    }

    llvm::Function *function(Symbol *pSymbol)
    {
        // Return the LLVM function for the given symbol, declaring it if needed.

        if (pSymbol->value == nullptr) {
            auto *res {llvm::Function::Create(llvmFunctionType(pSymbol->type), llvm::GlobalValue::ExternalLinkage,
                                              pSymbol->name, mModule)};

            addAttributes(res, mTarget.functionAttributes);

            for (auto &arg : res->args()) {
                arg.addAttr(llvm::Attribute::NoUndef);
            }

            // Note: as Clang does when math errno is disabled, our mathematical functions are declared as not accessing
            //       memory, even if they are only used through a pointer to them.

            if (pSymbol->builtin != nullptr) {
                addMathematicalFunctionAttributes(res);
            }

            res->setDSOLocal(mTarget.dsoLocalDeclarations);

            pSymbol->value = res;
        }

        return llvm::cast<llvm::Function>(pSymbol->value);
    }

    llvm::Value *allocate(TypePtr pType)
    {
        // Allocate some memory for a local variable, at the beginning of our function (so that it can be promoted to a
        // register) and after the memory we have already allocated (so that it is allocated in declaration order, as
        // Clang does).

        if (mLastAlloca == nullptr) {
            auto &entryBlock {mBuilder.GetInsertBlock()->getParent()->getEntryBlock()};

            mAllocaBuilder.SetInsertPoint(&entryBlock, entryBlock.begin());
        } else {
            mAllocaBuilder.SetInsertPoint(mLastAlloca->getNextNode());
        }

        mLastAlloca = mAllocaBuilder.CreateAlloca(llvmType(pType));

        mLastAlloca->setAlignment(variableAlignment(pType));

        return mLastAlloca;
    }

    llvm::Value *load(const Expr &pLvalue)
    {
        auto *res {mBuilder.CreateAlignedLoad(llvmType(pLvalue.type), generateAddress(pLvalue), accessAlignment(pLvalue))};

        res->setMetadata(llvm::LLVMContext::MD_tbaa, tbaaAccessTag(pLvalue));

        return res;
    }

    void store(llvm::Value *pValue, llvm::Value *pAddress, llvm::Align pAlignment, llvm::MDNode *pAccessTag)
    {
        mBuilder.CreateAlignedStore(pValue, pAddress, pAlignment)->setMetadata(llvm::LLVMContext::MD_tbaa, pAccessTag);
    }

    void generateFunction(FunctionDefinition &pFunction)
    {
        auto *symbol {pFunction.function};
        auto *llvmFunction {function(symbol)};

        llvmFunction->setDSOLocal(mTarget.dsoLocalDefinitions);
        llvmFunction->setVisibility(static_cast<llvm::GlobalValue::VisibilityTypes>(mTarget.definitionVisibility));
        llvmFunction->addFnAttr(llvm::Attribute::NoUnwind);
        llvmFunction->setUWTableKind(static_cast<llvm::UWTableKind>(mTarget.uwtable));

        if (!symbol->exportName.empty()) {
            llvmFunction->addFnAttr("wasm-export-name", symbol->exportName);
        }

        mFunctionName = symbol->name;
        mLastAlloca = nullptr;
        mLocalVariables.clear();

        mBuilder.SetInsertPoint(llvm::BasicBlock::Create(mContext, "", llvmFunction));

        // Store our parameters in local variables (as Clang does, which also flags a restrict-qualified pointer
        // parameter of a function definition, but not of a function declaration, as not aliasing any other pointer).

        for (size_t i {0}; i < pFunction.params.size(); ++i) {
            auto *param {pFunction.params[i]};
            auto *arg {llvmFunction->getArg(static_cast<unsigned int>(i))};

            if (param->isRestrict) {
                arg->addAttr(llvm::Attribute::NoAlias);
            }

            param->value = allocate(param->type);

            store(arg, param->value, variableAlignment(param->type), tbaaScalarAccessTag(param->type));
        }

        // Generate the code for our statements, up to the first return statement (as Clang does since any code after
        // it is unreachable).

        for (const auto &stmt : pFunction.body) {
            generateStatement(*stmt);

            if (stmt->kind == Stmt::Kind::RETURN) {
                return;
            }
        }

        // Our function doesn't end with a return statement, so it doesn't return anything (see
        // Parser::parseFunctionDefinition()) and we must return from it.

        endLifetimes();

        mBuilder.CreateRetVoid();
    }

    void endLifetimes()
    {
        for (auto localVariable {mLocalVariables.rbegin()}; localVariable != mLocalVariables.rend(); ++localVariable) {
            mBuilder.CreateLifetimeEnd(*localVariable)->setDoesNotThrow();
        }
    }

    void generateStatement(const Stmt &pStmt)
    {
        switch (pStmt.kind) {
        case Stmt::Kind::EXPRESSION:
            generateValue(*pStmt.expr);

            break;
        case Stmt::Kind::DECLARATION: {
            auto *variable {pStmt.variable};
            const auto *type {variable->type};

            if (variable->isStatic) {
                auto *globalType {llvmType(type)};
                auto *global {new llvm::GlobalVariable(mModule, globalType, false, llvm::GlobalValue::InternalLinkage,
                                                       llvm::Constant::getNullValue(globalType),
                                                       std::string(mFunctionName) + "." + std::string(variable->name))};

                global->setAlignment(variableAlignment(type));
                global->setDSOLocal(true);

                variable->value = global;
            } else {
                // Note: as Clang does, we mark the beginning of the lifetime of our local variable and its end when we
                //       return from our function (see endLifetimes()), so that its memory can be reused.

                variable->value = allocate(type);

                mBuilder.CreateLifetimeStart(variable->value)->setDoesNotThrow();

                mLocalVariables.push_back(variable->value);

                if (pStmt.expr != nullptr) {
                    store(generateValue(*pStmt.expr), variable->value, variableAlignment(type), tbaaScalarAccessTag(type));
                } else if (!pStmt.initialisers.empty()) {
                    // Initialise our structure, field by field, zero initialising any padding (as Clang does in C).

                    auto *structType {llvmStructType(type)};
                    const auto &dataLayout {mModule.getDataLayout()};
                    const auto *layout {dataLayout.getStructLayout(structType)};
                    auto structAlignment {variableAlignment(type)};
                    uint64_t paddingStart {0};
                    auto zeroInitialisePadding = [&](uint64_t pPaddingEnd) {
                        if (paddingStart < pPaddingEnd) {
                            auto *address {mBuilder.CreateConstGEP1_64(mBuilder.getInt8Ty(), variable->value, paddingStart)};

                            mBuilder.CreateMemSet(address, mBuilder.getInt8(0), mBuilder.getInt64(pPaddingEnd - paddingStart),
                                                  llvm::commonAlignment(structAlignment, paddingStart));
                        }
                    };

                    for (size_t i {0}; i < type->fields.size(); ++i) {
                        const auto *fieldType {type->fields[i].type};
                        auto offset {layout->getElementOffset(static_cast<unsigned int>(i))};

                        zeroInitialisePadding(offset);

                        auto *address {mBuilder.CreateStructGEP(structType, variable->value, static_cast<unsigned int>(i))};

                        store(generateValue(*pStmt.initialisers[i]), address, llvm::commonAlignment(structAlignment, offset),
                              mMdBuilder.createTBAAStructTagNode(tbaaType(type), tbaaType(fieldType), offset));

                        paddingStart = offset + dataLayout.getTypeAllocSize(llvmType(fieldType)).getFixedValue();
                    }

                    zeroInitialisePadding(layout->getSizeInBytes());
                }
            }
        } break;
        default: // Stmt::Kind::RETURN.
            if (pStmt.expr != nullptr) {
                auto *value {generateValue(*pStmt.expr)};

                endLifetimes();

                mBuilder.CreateRet(value);
            } else {
                endLifetimes();

                mBuilder.CreateRetVoid();
            }

            break;
        }
    }

    llvm::Value *generateAddress(const Expr &pExpr)
    {
        // Generate the address of the given lvalue.

        switch (pExpr.kind) {
        case Expr::Kind::VARIABLE:
            return pExpr.symbol->value;
        case Expr::Kind::INDEX: {
            // Note #1: as Clang does, the index is sign or zero extended to the size of a pointer, depending on whether
            //          it is signed or not.
            // Note #2: as Clang does, an array is indexed directly (rather than through a pointer to its first
            //          element).

            const auto &base {*pExpr.operands[0]};
            const auto &index {*pExpr.operands[1]};
            auto *intPtrType {mModule.getDataLayout().getIntPtrType(mContext)};

            if ((base.kind == Expr::Kind::CAST) && (base.cast == Expr::Cast::DECAY)) {
                const auto &array {*base.operands[0]};
                auto *arrayAddress {generateAddress(array)};
                auto *indexValue {mBuilder.CreateIntCast(generateValue(index), intPtrType, index.type->isSigned)};

                return mBuilder.CreateInBoundsGEP(llvmType(array.type), arrayAddress, {intPtrZero(), indexValue});
            }

            auto *baseValue {generateValue(base)};
            auto *indexValue {mBuilder.CreateIntCast(generateValue(index), intPtrType, index.type->isSigned)};

            return mBuilder.CreateInBoundsGEP(llvmType(pExpr.type), baseValue, indexValue);
        }
        default: // Expr::Kind::MEMBER.
            return mBuilder.CreateStructGEP(llvmType(pExpr.operands[0]->type->element),
                                            generateValue(*pExpr.operands[0]), static_cast<unsigned int>(pExpr.field));
        }
    }

    llvm::Value *generateCondition(const Expr &pExpr)
    {
        // Generate the truth value of the given scalar expression.

        auto *value {generateValue(pExpr)};

        if (pExpr.type->kind == Type::Kind::DOUBLE) {
            return mBuilder.CreateFCmpUNE(value, llvm::ConstantFP::get(value->getType(), 0.0));
        }

        // Because of the type rules of C, we often end up computing a logical value, then zero extending it to int,
        // then wanting it as a logical value again, so optimise this common case (as Clang does).

        if (auto *zext {llvm::dyn_cast<llvm::ZExtInst>(value)}; (zext != nullptr) && zext->getOperand(0)->getType()->isIntegerTy(1)) {
            auto *res {zext->getOperand(0)};

            // Note: the zero extension may still be used (e.g., if it is the result of an assignment).

            if (zext->use_empty()) {
                zext->eraseFromParent();
            }

            return res;
        }

        return mBuilder.CreateIsNotNull(value);
    }

    // Constant evaluation of an expression, as done by Clang when it tries to fold a condition or to find out whether
    // an expression can be evaluated at compile time (see CodeGenFunction::ConstantFoldsToSimpleInteger() and
    // Expr::isEvaluatable()), i.e. using IEEE floating-point semantics, but failing if the evaluation has undefined
    // behaviour (e.g., a floating-point operation resulting in a NaN, a signed integer overflow, or a double that
    // cannot be converted to an integer) or if an integer is divided by zero.

    struct ConstantValue
    {
        double doubleValue {0.0};
        llvm::APInt integerValue = llvm::APInt(1, 0);
    };

    static bool isTrue(const ConstantValue &pValue, TypePtr pType)
    {
        return (pType->kind == Type::Kind::DOUBLE) ? (pValue.doubleValue != 0.0) : !pValue.integerValue.isZero();
    }

    static ConstantValue integerValue(bool pValue)
    {
        return {.integerValue = llvm::APInt(32, pValue ? 1 : 0)}; // NOLINT
    }

    static std::optional<ConstantValue> evaluateCast(const Expr &pExpr, const ConstantValue &pOperand)
    {
        const auto *fromType {pExpr.operands[0]->type};
        const auto *toType {pExpr.type};

        switch (pExpr.cast) {
        case Expr::Cast::NONE:
            return pOperand;
        case Expr::Cast::INTEGER_TO_DOUBLE:
            return ConstantValue {.doubleValue = pOperand.integerValue.roundToDouble(fromType->isSigned)};
        case Expr::Cast::DOUBLE_TO_INTEGER: {
            auto value {std::trunc(pOperand.doubleValue)};
            auto max {std::ldexp(1.0, static_cast<int>(toType->bits - (toType->isSigned ? 1 : 0)))};

            if (!(value >= (toType->isSigned ? -max : 0.0)) || !(value < max)) {
                return std::nullopt;
            }

            return ConstantValue {.integerValue = toType->isSigned ?
                                                      llvm::APInt(toType->bits, static_cast<uint64_t>(static_cast<int64_t>(value)), true) :
                                                      llvm::APInt(toType->bits, static_cast<uint64_t>(value))};
        }
        default: // Expr::Cast::INTEGER_TO_INTEGER.
            return ConstantValue {.integerValue = fromType->isSigned ?
                                                      pOperand.integerValue.sextOrTrunc(toType->bits) :
                                                      pOperand.integerValue.zextOrTrunc(toType->bits)};
        }
    }

    static std::optional<llvm::CmpInst::Predicate> comparisonPredicate(std::string_view pOperator, TypePtr pType)
    {
        // The predicate of the given comparison operator for the given (operand) type, if it is a comparison operator,
        // as Clang uses it (see generateBinary()).

        struct Predicates
        {
            llvm::CmpInst::Predicate doublePredicate;
            llvm::CmpInst::Predicate signedPredicate;
            llvm::CmpInst::Predicate unsignedPredicate;
        };

        static const std::map<std::string_view, Predicates> PREDICATES {
            {"==", {llvm::CmpInst::FCMP_OEQ, llvm::CmpInst::ICMP_EQ, llvm::CmpInst::ICMP_EQ}},
            {"!=", {llvm::CmpInst::FCMP_UNE, llvm::CmpInst::ICMP_NE, llvm::CmpInst::ICMP_NE}},
            {"<", {llvm::CmpInst::FCMP_OLT, llvm::CmpInst::ICMP_SLT, llvm::CmpInst::ICMP_ULT}},
            {"<=", {llvm::CmpInst::FCMP_OLE, llvm::CmpInst::ICMP_SLE, llvm::CmpInst::ICMP_ULE}},
            {">", {llvm::CmpInst::FCMP_OGT, llvm::CmpInst::ICMP_SGT, llvm::CmpInst::ICMP_UGT}},
            {">=", {llvm::CmpInst::FCMP_OGE, llvm::CmpInst::ICMP_SGE, llvm::CmpInst::ICMP_UGE}},
        };

        auto predicates {PREDICATES.find(pOperator)};

        if (predicates == PREDICATES.end()) {
            return std::nullopt;
        }

        if (pType->kind == Type::Kind::DOUBLE) {
            return predicates->second.doublePredicate;
        }

        return pType->isSigned ? predicates->second.signedPredicate : predicates->second.unsignedPredicate;
    }

    static std::optional<ConstantValue> evaluateBinary(const Expr &pExpr, const ConstantValue &pLhs,
                                                       const ConstantValue &pRhs)
    {
        auto op {pExpr.token.text};
        const auto *type {pExpr.operands[0]->type};

        if (auto predicate {comparisonPredicate(op, type)}) {
            return integerValue((type->kind == Type::Kind::DOUBLE) ?
                                    llvm::FCmpInst::compare(llvm::APFloat(pLhs.doubleValue), llvm::APFloat(pRhs.doubleValue), *predicate) :
                                    llvm::ICmpInst::compare(pLhs.integerValue, pRhs.integerValue, *predicate));
        }

        if (type->kind == Type::Kind::DOUBLE) {
            auto lhs {pLhs.doubleValue};
            auto rhs {pRhs.doubleValue};
            static const std::map<std::string_view, double (*)(double, double)> OPERATIONS {
                {"+", [](double pX, double pY) {
                     return pX + pY;
                 }},
                {"-", [](double pX, double pY) {
                     return pX - pY;
                 }},
                {"*", [](double pX, double pY) {
                     return pX * pY;
                 }},
                {"/", [](double pX, double pY) {
                     return pX / pY;
                 }},
            };
            auto res {OPERATIONS.at(op)(lhs, rhs)};

            if (std::isnan(res)) {
                return std::nullopt;
            }

            return ConstantValue {.doubleValue = res};
        }

        const auto &lhs {pLhs.integerValue};
        const auto &rhs {pRhs.integerValue};
        auto isSigned {type->isSigned};

        if (op == "^") {
            return ConstantValue {.integerValue = lhs ^ rhs};
        }

        if ((op == "/") && rhs.isZero()) {
            return std::nullopt;
        }

        auto overflow {false};
        llvm::APInt res;

        if (op == "+") {
            res = isSigned ? lhs.sadd_ov(rhs, overflow) : (lhs + rhs);
        } else if (op == "-") {
            res = isSigned ? lhs.ssub_ov(rhs, overflow) : (lhs - rhs);
        } else if (op == "*") {
            res = isSigned ? lhs.smul_ov(rhs, overflow) : (lhs * rhs);
        } else {
            res = isSigned ? lhs.sdiv_ov(rhs, overflow) : lhs.udiv(rhs);
        }

        if (overflow) {
            return std::nullopt;
        }

        return ConstantValue {.integerValue = res};
    }

    static std::optional<ConstantValue> evaluate(const Expr &pExpr)
    {
        switch (pExpr.kind) {
        case Expr::Kind::NUMBER:
            if (pExpr.type->kind == Type::Kind::DOUBLE) {
                return ConstantValue {.doubleValue = pExpr.doubleValue};
            }

            return ConstantValue {.integerValue = llvm::APInt(pExpr.type->bits, pExpr.integerValue)};
        case Expr::Kind::CAST: {
            if ((pExpr.cast != Expr::Cast::NONE) && (pExpr.cast != Expr::Cast::INTEGER_TO_DOUBLE)
                && (pExpr.cast != Expr::Cast::DOUBLE_TO_INTEGER) && (pExpr.cast != Expr::Cast::INTEGER_TO_INTEGER)) {
                return std::nullopt;
            }

            auto operand {evaluate(*pExpr.operands[0])};

            return operand ? evaluateCast(pExpr, *operand) : std::nullopt;
        }
        case Expr::Kind::UNARY: {
            const auto &operandExpr {*pExpr.operands[0]};
            auto operand {evaluate(operandExpr)};

            if (!operand) {
                return std::nullopt;
            }

            if (pExpr.token.text == "!") {
                return integerValue(!isTrue(*operand, operandExpr.type));
            }

            if (pExpr.type->kind == Type::Kind::DOUBLE) {
                return ConstantValue {.doubleValue = -operand->doubleValue};
            }

            if (pExpr.type->isSigned && operand->integerValue.isMinSignedValue()) {
                return std::nullopt;
            }

            return ConstantValue {.integerValue = -operand->integerValue};
        }
        case Expr::Kind::BINARY: {
            auto lhs {evaluate(*pExpr.operands[0])};
            auto rhs {lhs ? evaluate(*pExpr.operands[1]) : std::nullopt};

            return rhs ? evaluateBinary(pExpr, *lhs, *rhs) : std::nullopt;
        }
        case Expr::Kind::LOGICAL: {
            // Note: the second operand is only evaluated if needed.

            auto lhs {evaluate(*pExpr.operands[0])};

            if (!lhs) {
                return std::nullopt;
            }

            auto isAnd {pExpr.token.text == "&&"};

            if (isTrue(*lhs, pExpr.operands[0]->type) != isAnd) {
                return integerValue(!isAnd);
            }

            auto rhs {evaluate(*pExpr.operands[1])};

            return rhs ? std::optional<ConstantValue>(integerValue(isTrue(*rhs, pExpr.operands[1]->type))) : std::nullopt;
        }
        case Expr::Kind::CONDITIONAL: {
            // Note: only the relevant operand is evaluated.

            auto condition {evaluate(*pExpr.operands[0])};

            return condition ?
                       evaluate(*pExpr.operands[isTrue(*condition, pExpr.operands[0]->type) ? 1 : 2]) :
                       std::nullopt;
        }
        default:
            return std::nullopt;
        }
    }

    static std::optional<bool> constantCondition(const Expr &pExpr)
    {
        // The value of the given condition, if it is an integer constant expression (as Clang requires).

        if (pExpr.type->kind != Type::Kind::INTEGER) {
            return std::nullopt;
        }

        auto value {evaluate(pExpr)};

        return value ? std::optional<bool>(!value->integerValue.isZero()) : std::nullopt;
    }

    static bool isConstant(const Expr &pExpr)
    {
        // Whether the given expression is a constant expression, i.e. an expression that Clang can evaluate at compile
        // time (see generateConditional()), which includes the address of a function.
        // Note: as for Clang, only the relevant operands of a logical or conditional operator are evaluated, so for
        //       instance "1.0 ? 2.0 : x" is a constant expression.

        return (pExpr.kind == Expr::Kind::FUNCTION) || evaluate(pExpr).has_value();
    }

    llvm::BasicBlock *newBlock()
    {
        // Create a block, which will only be added to our function when we start generating code in it (see
        // startBlock()), as Clang does.

        return llvm::BasicBlock::Create(mContext);
    }

    void startBlock(llvm::BasicBlock *pBlock)
    {
        // Start generating code in the given block, which we place after the current block, which we terminate with a
        // branch to the given block, if needed (as Clang does).

        auto *currentBlock {mBuilder.GetInsertBlock()};

        if (currentBlock->getTerminator() == nullptr) {
            mBuilder.CreateBr(pBlock);
        }

        currentBlock->getParent()->insert(std::next(currentBlock->getIterator()), pBlock);

        mBuilder.SetInsertPoint(pBlock);
    }

    void generateBranch(const Expr &pExpr, llvm::BasicBlock *pTrueBlock, llvm::BasicBlock *pFalseBlock)
    {
        // Branch to the given blocks depending on the truth value of the given scalar expression, handling logical
        // operators and the conditional operator by branching directly (as Clang does).

        switch (pExpr.kind) {
        case Expr::Kind::LOGICAL: {
            // Note: as Clang does, we simplify "1 && X", "X && 1", "0 || X" and "X || 0" to "X".

            auto isAnd {pExpr.token.text == "&&"};

            if (auto lhs {constantCondition(*pExpr.operands[0])}; lhs && (*lhs == isAnd)) {
                generateBranch(*pExpr.operands[1], pTrueBlock, pFalseBlock);

                break;
            }

            if (auto rhs {constantCondition(*pExpr.operands[1])}; rhs && (*rhs == isAnd)) {
                generateBranch(*pExpr.operands[0], pTrueBlock, pFalseBlock);

                break;
            }

            auto *block {newBlock()};

            if (isAnd) {
                generateBranch(*pExpr.operands[0], block, pFalseBlock);
            } else {
                generateBranch(*pExpr.operands[0], pTrueBlock, block);
            }

            startBlock(block);
            generateBranch(*pExpr.operands[1], pTrueBlock, pFalseBlock);
        } break;
        case Expr::Kind::CONDITIONAL: {
            auto *trueBlock {newBlock()};
            auto *falseBlock {newBlock()};

            generateBranch(*pExpr.operands[0], trueBlock, falseBlock);
            startBlock(trueBlock);
            generateBranch(*pExpr.operands[1], pTrueBlock, pFalseBlock);
            startBlock(falseBlock);
            generateBranch(*pExpr.operands[2], pTrueBlock, pFalseBlock);
        } break;
        default:
            if ((pExpr.kind == Expr::Kind::UNARY) && (pExpr.token.text == "!")) {
                generateBranch(*pExpr.operands[0], pFalseBlock, pTrueBlock);
            } else {
                mBuilder.CreateCondBr(generateCondition(pExpr), pTrueBlock, pFalseBlock);
            }

            break;
        }
    }

    llvm::Value *generateFMulAdd(llvm::Value *pLhs, llvm::Value *pRhs, bool pIsSub)
    {
        // Contract a multiplication followed by an addition or a subtraction into a fmuladd() call, as Clang does
        // with -ffp-contract=on, i.e. when the multiplication is one of the operands of the addition/subtraction,
        // possibly through a negation, and the result of the multiplication is not used for anything else.

        auto multiplication = [](llvm::Value *pValue, bool &pNegated) -> llvm::BinaryOperator * {
            auto *unaryOperator {llvm::dyn_cast<llvm::UnaryOperator>(pValue)};

            pNegated = (unaryOperator != nullptr) && unaryOperator->use_empty() && unaryOperator->getOperand(0)->hasOneUse();

            auto *res {llvm::dyn_cast<llvm::BinaryOperator>(pNegated ? unaryOperator->getOperand(0) : pValue)};

            return ((res != nullptr) && (res->getOpcode() == llvm::Instruction::FMul) && (res->use_empty() || pNegated)) ? res : nullptr;
        };
        auto fmuladd = [&](llvm::BinaryOperator *pMultiplication, llvm::Value *pNegation, llvm::Value *pAddend,
                           bool pNegateMultiplication, bool pNegateAddend) {
            auto *op0 {pMultiplication->getOperand(0)};
            auto *op1 {pMultiplication->getOperand(1)};

            if (pNegation != nullptr) {
                llvm::cast<llvm::Instruction>(pNegation)->eraseFromParent();
            }

            if (pNegateMultiplication) {
                op0 = mBuilder.CreateFNeg(op0);
            }

            if (pNegateAddend) {
                pAddend = mBuilder.CreateFNeg(pAddend);
            }

            auto *res {mBuilder.CreateIntrinsic(llvm::Intrinsic::fmuladd, {pAddend->getType()}, {op0, op1, pAddend})};

            pMultiplication->eraseFromParent();

            return res;
        };
        auto negated {false};

        if (auto *lhsMultiplication {multiplication(pLhs, negated)}; lhsMultiplication != nullptr) {
            return fmuladd(lhsMultiplication, negated ? pLhs : nullptr, pRhs, negated, pIsSub);
        }

        if (auto *rhsMultiplication {multiplication(pRhs, negated)}; rhsMultiplication != nullptr) {
            return fmuladd(rhsMultiplication, negated ? pRhs : nullptr, pLhs, pIsSub != negated, false);
        }

        return nullptr;
    }

    llvm::Value *generateBinary(const Expr &pExpr)
    {
        auto op {pExpr.token.text};
        const auto *operandType {pExpr.operands[0]->type};
        auto *lhs {generateValue(*pExpr.operands[0])};
        auto *rhs {generateValue(*pExpr.operands[1])};

        if (auto predicate {comparisonPredicate(op, operandType)}) {
            return mBuilder.CreateZExt(mBuilder.CreateCmp(*predicate, lhs, rhs), mBuilder.getInt32Ty());
        }

        if (operandType->kind == Type::Kind::DOUBLE) {
            if ((op == "+") || (op == "-")) {
                auto *res {generateFMulAdd(lhs, rhs, op == "-")};

                if (res != nullptr) {
                    return res;
                }

                return (op == "+") ? mBuilder.CreateFAdd(lhs, rhs) : mBuilder.CreateFSub(lhs, rhs);
            }

            return (op == "*") ? mBuilder.CreateFMul(lhs, rhs) : mBuilder.CreateFDiv(lhs, rhs);
        }

        auto isSigned {operandType->isSigned};

        if (op == "^") {
            return mBuilder.CreateXor(lhs, rhs);
        }

        if (op == "/") {
            return isSigned ? mBuilder.CreateSDiv(lhs, rhs) : mBuilder.CreateUDiv(lhs, rhs);
        }

        static const std::map<std::string_view, llvm::Instruction::BinaryOps> OPERATIONS {
            {"+", llvm::Instruction::Add},
            {"-", llvm::Instruction::Sub},
            {"*", llvm::Instruction::Mul},
        };
        auto *res {mBuilder.CreateBinOp(OPERATIONS.at(op), lhs, rhs)};

        if (auto *instruction {llvm::dyn_cast<llvm::BinaryOperator>(res)}; instruction != nullptr) {
            instruction->setHasNoSignedWrap(isSigned);
        }

        return res;
    }

    llvm::Value *generateLogical(const Expr &pExpr)
    {
        // Generate some code for && or ||, only evaluating the second operand if needed (as Clang does).

        auto isAnd {pExpr.token.text == "&&"};

        // Note: as Clang does, we simplify "1 && X" and "0 || X" to "X", and "0 && X" and "1 || X" to 0 and 1.

        if (auto lhs {constantCondition(*pExpr.operands[0])}) {
            if (*lhs == isAnd) {
                return mBuilder.CreateZExt(generateCondition(*pExpr.operands[1]), mBuilder.getInt32Ty());
            }

            return mBuilder.getInt32(isAnd ? 0 : 1);
        }

        auto *rhsBlock {newBlock()};
        auto *endBlock {newBlock()};

        generateBranch(*pExpr.operands[0], isAnd ? rhsBlock : endBlock, isAnd ? endBlock : rhsBlock);

        auto *res {llvm::PHINode::Create(mBuilder.getInt1Ty(), 2, "", endBlock)};

        for (auto *predecessor : llvm::predecessors(endBlock)) {
            res->addIncoming(mBuilder.getInt1(!isAnd), predecessor);
        }

        startBlock(rhsBlock);

        auto *rhsValue {generateCondition(*pExpr.operands[1])};

        rhsBlock = mBuilder.GetInsertBlock();

        startBlock(endBlock);

        res->addIncoming(rhsValue, rhsBlock);

        return mBuilder.CreateZExt(res, mBuilder.getInt32Ty());
    }

    llvm::Value *generateConditional(const Expr &pExpr)
    {
        // Generate some code for the conditional operator, i.e. only the relevant operand if the condition is constant,
        // a select if both operands are constant, otherwise some control flow that only evaluates the relevant operand
        // (as Clang does).

        if (auto condition {constantCondition(*pExpr.operands[0])}) {
            return generateValue(*pExpr.operands[*condition ? 1 : 2]);
        }

        if (isConstant(*pExpr.operands[1]) && isConstant(*pExpr.operands[2])) {
            // Note: Clang also zero extends the condition to 64 bits (for its profile counter). The result is not used,
            //       but it is a use of the condition, which can affect the order in which LLVM optimises our code, so
            //       we do the same.

            auto *condition {generateCondition(*pExpr.operands[0])};

            mBuilder.CreateZExt(condition, mBuilder.getInt64Ty());

            return mBuilder.CreateSelect(condition, generateValue(*pExpr.operands[1]), generateValue(*pExpr.operands[2]));
        }

        auto *trueBlock {newBlock()};
        auto *falseBlock {newBlock()};
        auto *endBlock {newBlock()};
        auto generateOperand = [&](llvm::BasicBlock *pBlock, const Expr &pOperand) {
            // Generate the given operand in the given block and return its value and the block in which we end up.

            startBlock(pBlock);

            auto *value {generateValue(pOperand)};
            auto *block {mBuilder.GetInsertBlock()};

            mBuilder.CreateBr(endBlock);

            return std::pair {value, block};
        };

        generateBranch(*pExpr.operands[0], trueBlock, falseBlock);

        auto [trueValue, trueEndBlock] {generateOperand(trueBlock, *pExpr.operands[1])};
        auto [falseValue, falseEndBlock] {generateOperand(falseBlock, *pExpr.operands[2])};

        startBlock(endBlock);

        if (pExpr.type->kind == Type::Kind::VOID) {
            return nullptr;
        }

        auto *res {mBuilder.CreatePHI(trueValue->getType(), 2)};

        res->addIncoming(trueValue, trueEndBlock);
        res->addIncoming(falseValue, falseEndBlock);

        return res;
    }

    llvm::Value *generateCall(const Expr &pExpr)
    {
        // Our mathematical functions are lowered to an intrinsic, an frem instruction, or a call to a function that
        // doesn't access memory (as Clang does when math errno is disabled).
        // Note: as Clang does, the function that is called is evaluated before its arguments, unless it is lowered to
        //       an intrinsic or an frem instruction.

        const auto &callee {*pExpr.operands[0]};
        const auto *builtin {(callee.kind == Expr::Kind::FUNCTION) ? callee.symbol->builtin : nullptr};
        auto isIntrinsic {(builtin != nullptr) && (builtin->intrinsic != llvm::Intrinsic::not_intrinsic)};
        auto *calleeValue {isIntrinsic ? nullptr : generateValue(callee)};
        std::vector<llvm::Value *> args;

        args.reserve(pExpr.operands.size() - 1);

        for (size_t i {1}; i < pExpr.operands.size(); ++i) {
            args.push_back(generateValue(*pExpr.operands[i]));
        }

        if (isIntrinsic) {
            if (builtin->intrinsic == FMOD) {
                return mBuilder.CreateFRem(args[0], args[1]);
            }

            return mBuilder.CreateIntrinsic(builtin->intrinsic, {mBuilder.getDoubleTy()}, args);
        }

        auto *callType {llvmFunctionType(callee.type->element)};
        auto *res {mBuilder.CreateCall(callType, calleeValue, args)};

        // Note: as Clang does, only a call to one of our mathematical functions has some attributes (those of its
        //       declaration, see function()).

        if (builtin != nullptr) {
            addMathematicalFunctionAttributes(res);
        }

        for (unsigned int i {0}; i < args.size(); ++i) {
            res->addParamAttr(i, llvm::Attribute::NoUndef);
        }

        return res;
    }

    llvm::Value *generateCast(const Expr &pExpr)
    {
        const auto &operand {*pExpr.operands[0]};

        switch (pExpr.cast) {
        case Expr::Cast::LOAD:
            return load(operand);
        case Expr::Cast::DECAY:
            // Note: as Clang does, we get a pointer to the first element of the array.

            return mBuilder.CreateInBoundsGEP(llvmType(operand.type), generateAddress(operand), {intPtrZero(), intPtrZero()});
        case Expr::Cast::INTEGER_TO_DOUBLE:
            return operand.type->isSigned ?
                       mBuilder.CreateSIToFP(generateValue(operand), mBuilder.getDoubleTy()) :
                       mBuilder.CreateUIToFP(generateValue(operand), mBuilder.getDoubleTy());
        case Expr::Cast::DOUBLE_TO_INTEGER:
            return pExpr.type->isSigned ?
                       mBuilder.CreateFPToSI(generateValue(operand), llvmType(pExpr.type)) :
                       mBuilder.CreateFPToUI(generateValue(operand), llvmType(pExpr.type));
        case Expr::Cast::INTEGER_TO_INTEGER:
            return mBuilder.CreateIntCast(generateValue(operand), llvmType(pExpr.type), operand.type->isSigned);
        case Expr::Cast::NONE:
        case Expr::Cast::POINTER_TO_POINTER:
            return generateValue(operand);
        default: // Expr::Cast::POINTER_TO_INTEGER.
            return mBuilder.CreatePtrToInt(generateValue(operand), llvmType(pExpr.type));
        }
    }

    llvm::Value *generateValue(const Expr &pExpr)
    {
        // Generate the value of the given rvalue.

        switch (pExpr.kind) {
        case Expr::Kind::NUMBER:
            if (pExpr.type->kind == Type::Kind::DOUBLE) {
                return llvm::ConstantFP::get(mBuilder.getDoubleTy(), pExpr.doubleValue);
            }

            return llvm::ConstantInt::get(llvmType(pExpr.type), pExpr.integerValue);
        case Expr::Kind::FUNCTION:
            return function(pExpr.symbol);
        case Expr::Kind::CAST:
            return generateCast(pExpr);
        case Expr::Kind::UNARY: {
            const auto &operand {*pExpr.operands[0]};

            if (pExpr.token.text == "!") {
                return mBuilder.CreateZExt(mBuilder.CreateNot(generateCondition(operand)), mBuilder.getInt32Ty());
            }

            if (pExpr.type->kind == Type::Kind::DOUBLE) {
                return mBuilder.CreateFNeg(generateValue(operand));
            }

            return mBuilder.CreateNeg(generateValue(operand), "", pExpr.type->isSigned);
        }
        case Expr::Kind::BINARY:
            return generateBinary(pExpr);
        case Expr::Kind::LOGICAL:
            return generateLogical(pExpr);
        case Expr::Kind::CONDITIONAL:
            return generateConditional(pExpr);
        case Expr::Kind::CALL:
            return generateCall(pExpr);
        case Expr::Kind::ADDRESS:
            return generateAddress(*pExpr.operands[0]);
        default: { // Expr::Kind::ASSIGNMENT.
            // Note: as Clang does, we evaluate the right-hand side before the address of the left-hand side.

            const auto &lhs {*pExpr.operands[0]};
            auto *value {generateValue(*pExpr.operands[1])};

            store(value, generateAddress(lhs), accessAlignment(lhs), tbaaAccessTag(lhs));

            return value;
        }
        }
    }
};

// NOLINTEND(misc-no-recursion)

} // namespace

std::unique_ptr<llvm::Module> generateIr(const std::string &pCode, llvm::LLVMContext &pContext,
                                         const IrGeneratorTarget &pTarget, std::string &pError)
{
    // Parse the given code.

    Parser parser(pCode, pTarget.longBits);

    if (!parser.parse()) {
        pError = parser.error();

        return nullptr;
    }

    // Generate some LLVM IR for it.

    auto res {std::make_unique<llvm::Module>("libOpenCOR", pContext)};

    res->setTargetTriple(llvm::Triple(pTarget.triple));
    res->setDataLayout(pTarget.dataLayout);

    for (const auto &[behavior, name, value] : pTarget.moduleFlags) {
        res->addModuleFlag(static_cast<llvm::Module::ModFlagBehavior>(behavior), name, value);
    }

    CodeGenerator(*res, pTarget).generate(parser.functions());

    return res;
}

std::string generateIrText(const std::string &pCode, const IrGeneratorTarget &pTarget, std::string &pError)
{
    llvm::LLVMContext context;
    auto module {generateIr(pCode, context, pTarget, pError)};
    std::string res;

    if (module != nullptr) {
        llvm::raw_string_ostream stream(res);

        module->print(stream, nullptr);
    }

    return res;
}

} // namespace libOpenCOR
