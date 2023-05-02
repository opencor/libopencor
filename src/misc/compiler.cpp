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

#include <iostream>

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

bool Compiler::compile(const std::string &pCode)
{
    // Reset ourselves.

    mLljit.reset(nullptr);

    // Create a diagnostics engine.

    auto diagnosticOptions = llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions>(new clang::DiagnosticOptions());
    auto diagnosticsEngine = llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine>(new clang::DiagnosticsEngine(llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(new clang::DiagnosticIDs()),
                                                                                                             &*diagnosticOptions,
                                                                                                             new clang::TextDiagnosticPrinter(llvm::nulls(), &*diagnosticOptions)));

    diagnosticsEngine->setWarningsAsErrors(true);

    // Get a driver object and ask it not to check that input files exist.

    clang::driver::Driver driver("clang", llvm::sys::getProcessTriple(), *diagnosticsEngine);

    driver.setCheckInputsExist(false);

    // Get a compilation object to which we pass some arguments.

    static constexpr auto DUMMY_FILE_NAME = "dummy.c";
    static const std::vector<const char *> COMPILATION_ARGUMENTS = {"clang", "-fsyntax-only",
#ifdef NDEBUG
                                                                    "-O3",
#else
                                                                    "-g", "-O0",
#endif
                                                                    "-fno-math-errno",
                                                                    DUMMY_FILE_NAME};

    std::unique_ptr<clang::driver::Compilation> compilation(driver.BuildCompilation(COMPILATION_ARGUMENTS));

#ifdef NLLVMCOV
    if (compilation == nullptr) {
        return false;
    }
#endif

    // The compilation object should have only one command, so if it doesn't then something went wrong.

    clang::driver::JobList &jobs = compilation->getJobs();

#ifdef NLLVMCOV
    if ((jobs.size() != 1) || !llvm::isa<clang::driver::Command>(*jobs.begin())) {
        return false;
    }
#endif

    // Retrieve the command name and make sure that it is "clang".

    auto &command = llvm::cast<clang::driver::Command>(*jobs.begin());

#ifdef NLLVMCOV
    static constexpr auto CLANG = "clang";

    if (strcmp(command.getCreator().getName(), CLANG) != 0) {
        return false;
    }
#endif

    // Prevent the Clang driver from asking CC1 to leak memory, this by removing -disable-free from the command
    // arguments.

    auto commandArguments = command.getArguments();
#ifdef NLLVMCOV
    auto *commandArgument = find(commandArguments, llvm::StringRef("-disable-free"));

    if (commandArgument != commandArguments.end()) {
        commandArguments.erase(commandArgument);
    }
#else
    commandArguments.erase(find(commandArguments, llvm::StringRef("-disable-free")));
#endif

    // Create a compiler instance.

    clang::CompilerInstance compilerInstance;

    compilerInstance.setDiagnostics(diagnosticsEngine.get());
    compilerInstance.setVerboseOutputStream(llvm::nulls());

    // Create a compiler invocation object.

#ifdef NLLVMCOV
    bool res =
#endif
        clang::CompilerInvocation::CreateFromArgs(compilerInstance.getInvocation(),
                                                  commandArguments,
                                                  *diagnosticsEngine);

#ifdef NLLVMCOV
    if (!res) {
        return false;
    }
#endif

    // Map our code to a memory buffer.

    compilerInstance.getInvocation().getPreprocessorOpts().addRemappedFile(DUMMY_FILE_NAME,
                                                                           llvm::MemoryBuffer::getMemBuffer(pCode).release());

    // Compile the given code, resulting in an LLVM bitcode module.

    std::unique_ptr<clang::CodeGenAction> codeGenAction(new clang::EmitLLVMOnlyAction(llvm::unwrap(LLVMGetGlobalContext())));

    if (!compilerInstance.ExecuteAction(*codeGenAction)) {
        return false;
    }

    // Retrieve the LLVM bitcode module.

    auto module = codeGenAction->takeModule();

#ifdef NLLVMCOV
    if (module == nullptr) {
        return false;
    }
#endif

    // Initialise the native target (and its ASM printer), so not only can we then create an execution engine, but more
    // importantly its data layout will match that of our target platform.

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Create an ORC-based JIT and keep track of it (so that we can use it in function()).

    auto lljit = llvm::orc::LLJITBuilder().create();

#ifdef NLLVMCOV
    if (lljit == nullptr) {
        return false;
    }
#endif

    mLljit = std::move(*lljit);

    // Add our LLVM bitcode module to our ORC-based JIT.

    auto llvmContext = std::make_unique<llvm::LLVMContext>();
    auto threadSafeModule = llvm::orc::ThreadSafeModule(std::move(module), std::move(llvmContext));

    return !mLljit->addIRModule(std::move(threadSafeModule));
}

void *Compiler::function(const std::string &pName)
{
    // Return the address of the requested function.

    if ((mLljit != nullptr) && !pName.empty()) {
        auto symbol = mLljit->lookup(pName);

        if (symbol) {
            return reinterpret_cast<void *>(symbol->getAddress()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
        }
    }

    return nullptr;
}

} // namespace libOpenCOR
