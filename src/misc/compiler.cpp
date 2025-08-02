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

#include "clangbegin.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clangend.h"

#include "llvmbegin.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm-c/Core.h"
#include "llvmend.h"

#include <random>
#include <sstream>

namespace libOpenCOR {

namespace {

#ifndef CODE_COVERAGE_ENABLED
std::string llvmClangError(llvm::Error pError)
{
    std::string res;

    llvm::handleAllErrors(std::move(pError), [&res](const llvm::ErrorInfoBase &errorInfo) {
        res = errorInfo.message();
    });

    res[0] = static_cast<char>(toupper(res[0]));

    return res.insert(0, " (").append(")");
}
#endif

} // namespace

#ifdef __EMSCRIPTEN__
bool Compiler::Impl::compile(const std::string &pCode, UnsignedChars &pWasmModule)
#else
bool Compiler::Impl::compile(const std::string &pCode)
#endif
{
    // Reset ourselves.

#ifndef __EMSCRIPTEN__
    mLljit.reset(nullptr);
#endif

    removeAllIssues();

    // Create a diagnostics engine.

    auto diagnosticOptions = llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions>(std::make_unique<clang::DiagnosticOptions>());
    std::string diagnostics;
    llvm::raw_string_ostream outputStream(diagnostics);
    auto diagnosticsEngine = llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine>(std::make_unique<clang::DiagnosticsEngine>(llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(std::make_unique<clang::DiagnosticIDs>()),
                                                                                                                           &*diagnosticOptions,
                                                                                                                           std::make_unique<clang::TextDiagnosticPrinter>(outputStream, &*diagnosticOptions).release()));

    diagnosticsEngine->setWarningsAsErrors(true);

    // Get a driver object and ask it not to check that input files exist.

    clang::driver::Driver driver("clang", llvm::sys::getProcessTriple(), *diagnosticsEngine);

    driver.setCheckInputsExist(false);

    // Get a compilation object to which we pass some arguments.

    static constexpr auto DUMMY_FILE_NAME = "dummy.c";
    static const std::vector<const char *> COMPILATION_ARGUMENTS = {"clang", "-fsyntax-only",
                                                                    "-O3",
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
        addError(std::string("The command name must be 'clang' while it is '").append(command.getCreator().getName()).append("'."));

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

    auto compilerInstance = std::make_unique<clang::CompilerInstance>();

    compilerInstance->setDiagnostics(diagnosticsEngine.get());
    compilerInstance->setVerboseOutputStream(llvm::nulls());

    // Create a compiler invocation object.

#ifndef CODE_COVERAGE_ENABLED
    bool res =
#endif
        clang::CompilerInvocation::CreateFromArgs(compilerInstance->getInvocation(),
                                                  commandArguments,
                                                  *diagnosticsEngine);

#ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        addError("A compiler invocation object could not be created.");

        return false;
    }
#endif

    // Map our code to a memory buffer.

    auto code = R"(// Arithmetic operators.

extern double pow(double, double);
extern double sqrt(double);
extern double fabs(double);
extern double exp(double);
extern double log(double);
extern double log10(double);
extern double ceil(double);
extern double floor(double);
extern double fmin(double, double);
extern double fmax(double, double);
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

    compilerInstance->getInvocation().getPreprocessorOpts().addRemappedFile(DUMMY_FILE_NAME,
                                                                            llvm::MemoryBuffer::getMemBuffer(code).release());

    // Compile the given code, resulting in an LLVM bitcode module.

    auto llvmContext = std::make_unique<llvm::LLVMContext>();
    auto codeGenAction = std::make_unique<clang::EmitLLVMOnlyAction>(llvmContext.get());

    if (!compilerInstance->ExecuteAction(*codeGenAction)) {
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

    // Retrieve the LLVM module.

    auto module = codeGenAction->takeModule();

#ifndef CODE_COVERAGE_ENABLED
    if (module == nullptr) {
        addError("The LLVM module could not be retrieved.");

        return false;
    }
#endif

    // Initialise the native target and its ASM printer.

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

#ifdef __EMSCRIPTEN__
    // Look up the target.

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(module->getTargetTriple(), error);

    if (target == nullptr) {
        error[0] = static_cast<char>(tolower(error[0]));

        addError(std::string("the target (").append(module->getTargetTriple()).append(") could not be found: ").append(error));

        return false;
    }

    // Create a target machine.

    auto targetMachine = std::unique_ptr<llvm::TargetMachine>(target->createTargetMachine(module->getTargetTriple(),
                                                                                          "generic", "",
                                                                                          llvm::TargetOptions(),
                                                                                          llvm::Reloc::Static));

    if (targetMachine == nullptr) {
        addError("A target machine could not be created.");

        return false;
    }

    // Get the target machine to emit some WebAssembly code.

    llvm::legacy::PassManager passManager;
    llvm::SmallVector<char, 0> outputBuffer;
    llvm::raw_svector_ostream output(outputBuffer);

    if (targetMachine->addPassesToEmitFile(passManager, output, nullptr, llvm::CGFT_ObjectFile)) {
        addError("The target machine cannot emit some WebAssembly code.");

        return false;
    }

    passManager.run(*module);

    // Retrieve the WebAssembly code.

    pWasmModule.assign(outputBuffer.begin(), outputBuffer.end());

    if (pWasmModule.empty()) {
        addError("No WebAssembly code could be generated.");

        return false;
    }

    return true;
#else
    // Create an ORC-based JIT and keep track of it.

    auto lljit = llvm::orc::LLJITBuilder().create();

#    ifndef CODE_COVERAGE_ENABLED
    if (!lljit) {
        addError(std::string("An ORC-based JIT could not be created").append(llvmClangError(lljit.takeError())).append("."));

        return false;
    }
#    endif

    mLljit = std::move(*lljit);

    // Make sure that we can find various mathematical functions in the standard C library.

    auto dynamicLibrarySearchGenerator = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(mLljit->getDataLayout().getGlobalPrefix());

#    ifndef CODE_COVERAGE_ENABLED
    if (!dynamicLibrarySearchGenerator) {
        addError(std::string("The dynamic library search generator could not be created").append(llvmClangError(dynamicLibrarySearchGenerator.takeError())).append("."));

        return false;
    }
#    endif

    mLljit->getMainJITDylib().addGenerator(std::move(*dynamicLibrarySearchGenerator));

    // Add our LLVM bitcode module to our ORC-based JIT.

    auto threadSafeModule = llvm::orc::ThreadSafeModule(std::move(module), std::move(llvmContext));

#    ifdef CODE_COVERAGE_ENABLED
    const bool res =
#    else
    res =
#    endif
        !mLljit->addIRModule(std::move(threadSafeModule));

#    ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        addError("The LLVM bitcode module could not be added to the ORC-based JIT.");
    }
#    endif

    return res;
#endif
}

#ifndef __EMSCRIPTEN__
bool Compiler::Impl::addFunction(const std::string &pName, void *pFunction)
{
    // Add the given function to our ORC-based JIT. Note that we assume that we have a valid ORC-based JIT, function
    // name, and function.

    const bool res = !mLljit->getMainJITDylib().define(llvm::orc::absoluteSymbols({
        {mLljit->mangleAndIntern(pName), llvm::JITEvaluatedSymbol(llvm::pointerToJITTargetAddress(pFunction), llvm::JITSymbolFlags::Exported)},
    }));

#    ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        addError(std::string("The ").append(pName).append("() function could not be added to the compiler."));
    }
#    endif

    return res;
}

void *Compiler::Impl::function(const std::string &pName) const
{
    // Return the address of the requested function. Note that we assume that we have a valid ORC-based JIT and function
    // name.

    auto symbol = mLljit->lookup(pName);

    if (symbol) {
        return reinterpret_cast<void *>(symbol->getValue()); // NOLINT
    }

    return {};
}
#endif

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
    return static_cast<Impl *>(Logger::mPimpl);
}

const Compiler::Impl *Compiler::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

CompilerPtr Compiler::create()
{
    return CompilerPtr {new Compiler {}};
}

#ifdef __EMSCRIPTEN__
bool Compiler::compile(const std::string &pCode, UnsignedChars &pWasmModule)
{
    return pimpl()->compile(pCode, pWasmModule);
}
#else
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
#endif

} // namespace libOpenCOR
