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

#include "clangbegin.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clangend.h"

#include "llvmbegin.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm-c/Core.h"
#include "llvmend.h"

namespace libOpenCOR {

Compiler::~Compiler()
{
    delete mExecutionEngine;
}

bool Compiler::compile(const std::string &pCode)
{
    // Reset ourselves.

    delete mExecutionEngine;

    mExecutionEngine = nullptr;

    // Create a compiler instance.

    clang::CompilerInstance compilerInstance;

    // Create and set the compiler instance's diagnostics engine and make sure
    // that it doesn't output anything on the console.

    auto diagnosticOtpions = llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions>(new clang::DiagnosticOptions());
    auto diagnosticsEngine = clang::CompilerInstance::createDiagnostics(&*diagnosticOtpions,
                                                                        new clang::TextDiagnosticPrinter(llvm::nulls(),
                                                                                                         &*diagnosticOtpions));

    compilerInstance.setDiagnostics(&*diagnosticsEngine);
    compilerInstance.setVerboseOutputStream(llvm::nulls());

    // Get a driver object and ask it not to check that input files exist.

    clang::driver::Driver driver("clang",
                                 llvm::sys::getProcessTriple(),
                                 *diagnosticsEngine);

    driver.setCheckInputsExist(false);

    // Get a compilation object to which we pass some arguments.

    static const char *DummyFileName = "dummy.c";

    llvm::SmallVector<const char *, 16> compilationArguments;

    compilationArguments.push_back("clang");
    compilationArguments.push_back("-fsyntax-only");
#ifdef NDEBUG
    compilationArguments.push_back("-O3");
#else
    compilationArguments.push_back("-g");
    compilationArguments.push_back("-O0");
#endif
    compilationArguments.push_back("-fno-math-errno");
    compilationArguments.push_back("-Wall");
    compilationArguments.push_back("-W");
    compilationArguments.push_back("-Werror");
    compilationArguments.push_back(DummyFileName);

    std::unique_ptr<clang::driver::Compilation> compilation(driver.BuildCompilation(compilationArguments));

    // Create a compiler invocation object.

    auto compilerInvocation = std::make_unique<clang::CompilerInvocation>();

    clang::CompilerInvocation::CreateFromArgs(*compilerInvocation,
                                              llvm::cast<clang::driver::Command>(*compilation->getJobs().begin()).getArguments(),
                                              *diagnosticsEngine);

    // Map our code to a memory buffer.

    compilerInvocation->getPreprocessorOpts().addRemappedFile(DummyFileName,
                                                              llvm::MemoryBuffer::getMemBuffer(pCode).release());

    // Have our compiler instance use our compiler invocation object.

    compilerInstance.setInvocation(std::move(compilerInvocation));

    // Compile the given code, resulting in an LLVM bitcode module.

    std::unique_ptr<clang::CodeGenAction> codeGenerationAction(new clang::EmitLLVMOnlyAction(llvm::unwrap(LLVMGetGlobalContext())));

    if (!compilerInstance.ExecuteAction(*codeGenerationAction)) {
        // The code couldn't be compiled.

        return false;
    }

    // Retrieve the LLVM bitcode module.

    std::unique_ptr<llvm::Module> module = codeGenerationAction->takeModule();

    // Initialise the native target (and its ASM printer), so not only can we
    // then create an execution engine, but more importantly its data layout
    // will match that of our target platform.

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Create and keep track of an execution engine.

    mExecutionEngine = llvm::EngineBuilder(std::move(module)).setEngineKind(llvm::EngineKind::JIT).create();

    return true;
}

void *Compiler::function(const std::string &pName)
{
    // Return the address of the requested function.

    if (mExecutionEngine != nullptr) {
        return reinterpret_cast<void *>(mExecutionEngine->getFunctionAddress(pName)); // NOLINT
    }

    return nullptr;
}

} // namespace libOpenCOR
