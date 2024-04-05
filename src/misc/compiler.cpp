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

#include "compiler_p.h"

#include "utils.h"

#include <iostream>
#include <sstream>

#include "clangbegin.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clangend.h"

#include "llvmbegin.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm-c/Core.h"
#include "llvmend.h"

namespace libOpenCOR {

bool Compiler::Impl::compile(const std::string &pCode)
{
    // Reset ourselves.

    mLljit.reset(nullptr);

    removeAllIssues();

    // Create a diagnostics engine.

    auto diagnosticOptions = llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions>(new clang::DiagnosticOptions {});
    std::string diagnostics;
    llvm::raw_string_ostream outputStream(diagnostics);
    auto diagnosticsEngine = llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine>(new clang::DiagnosticsEngine {llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(new clang::DiagnosticIDs {}),
                                                                                                              &*diagnosticOptions,
                                                                                                              new clang::TextDiagnosticPrinter {outputStream, &*diagnosticOptions}});

    diagnosticsEngine->setWarningsAsErrors(true);

    // Get a driver object and ask it not to check that input files exist.

    clang::driver::Driver driver("clang", llvm::sys::getProcessTriple(), *diagnosticsEngine);

    driver.setCheckInputsExist(false);

    // Get a compilation object to which we pass some arguments.

    static constexpr auto DUMMY_FILE_NAME = "dummy.c";
    static const ConstCharPtrVector COMPILATION_ARGUMENTS = {"clang", "-fsyntax-only",
#ifdef NDEBUG
                                                             "-O3",
#else
                                                             "-g", "-O0",
#endif
                                                             "-fno-math-errno",
                                                             "-fno-stack-protector",
                                                             DUMMY_FILE_NAME};

    std::unique_ptr<clang::driver::Compilation> compilation(driver.BuildCompilation(COMPILATION_ARGUMENTS));

#ifndef CODE_COVERAGE_ENABLED
    if (compilation == nullptr) {
        addError("A compilation object could not be created.");

        return false;
    }
#endif

    // The compilation object should have one command, so if it doesn't then something went wrong.

    clang::driver::JobList &jobs = compilation->getJobs();

#ifndef CODE_COVERAGE_ENABLED
    if ((jobs.size() != 1) || !llvm::isa<clang::driver::Command>(*jobs.begin())) {
        addError("The compilation object must have one command.");

        return false;
    }
#endif

    // Retrieve the command and make sure that its name is "clang".

    auto &command = llvm::cast<clang::driver::Command>(*jobs.begin());

#ifndef CODE_COVERAGE_ENABLED
    static constexpr auto CLANG = "clang";

    if (strcmp(command.getCreator().getName(), CLANG) != 0) {
        addError(std::string("The command name must be 'clang' while it is '") + command.getCreator().getName() + "'.");

        return false;
    }
#endif

    // Prevent the Clang driver from asking cc1 to leak memory, this by removing -disable-free from the command
    // arguments.

    auto commandArguments = command.getArguments();

#ifdef CODE_COVERAGE_ENABLED
    commandArguments.erase(find(commandArguments, llvm::StringRef("-disable-free")));
#else
    auto *commandArgument = find(commandArguments, llvm::StringRef("-disable-free"));

    if (commandArgument != commandArguments.end()) {
        commandArguments.erase(commandArgument);
    }
#endif

    // Create a compiler instance.

    clang::CompilerInstance compilerInstance;

    compilerInstance.setDiagnostics(diagnosticsEngine.get());
    compilerInstance.setVerboseOutputStream(llvm::nulls());

    // Create a compiler invocation object.

#ifndef CODE_COVERAGE_ENABLED
    bool res =
#endif
        clang::CompilerInvocation::CreateFromArgs(compilerInstance.getInvocation(),
                                                  commandArguments,
                                                  *diagnosticsEngine);

#ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        addError("A compiler invocation object could not be created.");

        return false;
    }
#endif

    // Map our code to a memory buffer.

    auto code = R"(
// Arithmetic operators.

extern double pow(double, double);
extern double sqrt(double);
extern double fabs(double);
extern double exp(double);
extern double log(double);
extern double log10(double);
extern double ceil(double);
extern double floor(double);
extern double fmod(double, double);

// Trigonometric operators.

extern double sin(double);
extern double cos(double);
extern double tan(double);

extern double sinh(double);
extern double cosh(double);
extern double tanh(double);

extern double asin(double);
extern double acos(double);
extern double atan(double);

extern double asinh(double);
extern double acosh(double);
extern double atanh(double);

// Constants.

#define INFINITY (__builtin_inf())
#define NAN (__builtin_nan(""))
)" + pCode;

    compilerInstance.getInvocation().getPreprocessorOpts().addRemappedFile(DUMMY_FILE_NAME,
                                                                           llvm::MemoryBuffer::getMemBuffer(code).release());

    // Compile the given code, resulting in an LLVM bitcode module.

    std::unique_ptr<clang::CodeGenAction> codeGenAction(new clang::EmitLLVMOnlyAction {llvm::unwrap(LLVMGetGlobalContext())});

    if (!compilerInstance.ExecuteAction(*codeGenAction)) {
        addError("The given code could not be compiled.");

        static constexpr auto ERROR = ": error: ";
        static auto ERROR_LENGTH = strlen(ERROR);
        static constexpr auto NOTE = ": note: ";
        static auto NOTE_LENGTH = strlen(NOTE);

        std::istringstream input(diagnostics);
        std::string line;

        std::getline(input, line);

        while (!input.eof()) {
            std::string error = line.substr(line.find(ERROR) + ERROR_LENGTH) + ":";

            error[0] = static_cast<char>(std::toupper(error[0]));

            std::getline(input, line);

            bool hasErrorDetails = false;

            while (!input.eof()) {
                const auto notePos = line.find(NOTE);

                if (notePos != std::string::npos) {
                    if (hasErrorDetails) {
                        error += "\n";
                    } else {
                        error[error.size() - 1] = ' ';
                    }

                    error += line.substr(notePos + NOTE_LENGTH) + ":";
                } else if (line.find(DUMMY_FILE_NAME) != std::string::npos) {
                    break;
                } else {
                    error += "\n" + line;

                    hasErrorDetails = true;
                }

                std::getline(input, line);
            }

            addError(error);
        }

        return false;
    }

    // Retrieve the LLVM bitcode module.

    auto module = codeGenAction->takeModule();

#ifndef CODE_COVERAGE_ENABLED
    if (module == nullptr) {
        addError("The LLVM bitcode module could not be retrieved.");

        return false;
    }
#endif

    // Initialise the native target (and its ASM printer), so not only can we then create an execution engine, but more
    // importantly its data layout will match that of our target platform.

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Create an ORC-based JIT and keep track of it.

    auto lljit = llvm::orc::LLJITBuilder().create();

#ifndef CODE_COVERAGE_ENABLED
    if (!lljit) {
        addError("An ORC-based JIT could not be created.");

        return false;
    }
#endif

    mLljit = std::move(*lljit);

    // Make sure that we can find various mathematical functions in the standard C library.

    auto dynamicLibrarySearchGenerator = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(mLljit->getDataLayout().getGlobalPrefix());

#ifndef CODE_COVERAGE_ENABLED
    if (!dynamicLibrarySearchGenerator) {
        addError("The dynamic library search generator could not be created.");

        return false;
    }
#endif

    mLljit->getMainJITDylib().addGenerator(std::move(*dynamicLibrarySearchGenerator));

    // Add our LLVM bitcode module to our ORC-based JIT.

    auto llvmContext = std::make_unique<llvm::LLVMContext>();
    auto threadSafeModule = llvm::orc::ThreadSafeModule(std::move(module), std::move(llvmContext));

#ifdef CODE_COVERAGE_ENABLED
    const bool res =
#else
    res =
#endif
        !mLljit->addIRModule(std::move(threadSafeModule));

#ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        addError("The LLVM bitcode module could not be added to the ORC-based JIT.");
    }
#endif

    return res;
}

bool Compiler::Impl::addFunction(const std::string &pName, void *pFunction)
{
    // Add the given function to our ORC-based JIT. Note that we assume that we have a valid ORC-based JIT, function
    // name, and function.

    const bool res = !mLljit->getMainJITDylib().define(llvm::orc::absoluteSymbols({
        {mLljit->mangleAndIntern(pName), llvm::JITEvaluatedSymbol(llvm::pointerToJITTargetAddress(pFunction), llvm::JITSymbolFlags::Exported)},
    }));

#ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        addError("The " + pName + "() function could not be added to the compiler.");
    }
#endif

    return res;
}

void *Compiler::Impl::function(const std::string &pName) const
{
    // Return the address of the requested function. Note that we assume that we have a valid ORC-based JIT and function
    // name.

    auto symbol = mLljit->lookup(pName);

    if (symbol) {
        return reinterpret_cast<void *>(symbol->getAddress()); // NOLINT
    }

    return {};
}

Compiler::Compiler()
    : Logger(new Impl {})
{
}

Compiler::~Compiler()
{
    delete pimpl();
}

Compiler::Impl *Compiler::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

const Compiler::Impl *Compiler::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}

CompilerPtr Compiler::create()
{
    return CompilerPtr {new Compiler {}};
}

bool Compiler::compile(const std::string &pCode)
{
    return pimpl()->compile(pCode);
}

bool Compiler::addFunction(const std::string &pName, void *pFunction)
{
    return pimpl()->addFunction(pName, pFunction);
}

void *Compiler::function(const std::string &pName) const
{
    return pimpl()->function(pName);
}

} // namespace libOpenCOR
