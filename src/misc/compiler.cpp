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

#include "clang/Basic/TargetInfo.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm-c/Core.h"

#include <cstdio>
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

    std::string wrappedError;

    wrappedError.reserve(res.size() + 3); // NOLINT

    wrappedError += " (";
    wrappedError += res;
    wrappedError += ")";

    return wrappedError;
}
#endif

#ifdef __EMSCRIPTEN__
static void patchWasmSharedMemory(UnsignedChars &pWasmModule)
{
    // Patch the WASM module to mark the imported memory as shared and add the shared-mem target feature. Indeed, some
    // LLVM versions don't recognise +shared-mem as a WebAssembly CPU feature, and even those that do still output the
    // memory import flags as non-shared through the machine code layer. The shared flag is normally set by the linker,
    // but we don't use one. So, to patch the binary directly handles both issues reliably across all LLVM versions.

    // Helper to decode ULEB128.

    auto decodeULEB128 = [](const unsigned char *data, size_t &pos) -> size_t {
        size_t res {0};
        unsigned shift {0};

        while (true) {
            auto byte {data[pos++]};

            res |= static_cast<size_t>(byte & 0x7f) << shift;

            if ((byte & 0x80) == 0) {
                return res;
            }

            shift += 7;
        }
    };

    // Helper to encode ULEB128 into a buffer and return the number of bytes written.

    auto encodeULEB128 = [](unsigned char *buffer, size_t value) -> size_t {
        size_t res {0};

        do {
            auto byte {value & 0x7f};

            value >>= 7;

            if (value != 0) {
                byte |= 0x80;
            }

            buffer[res++] = byte;
        } while (value != 0);

        return res;
    };

    // Helper to add a delta to a fixed 5-byte LEB128 encoding. The WebAssembly backend always emits section sizes as a
    // non-canonical 5-byte encoding (all continuation bits set except the last). Simply incrementing the first byte
    // overflows when the base value exceeds 125, corrupting the multi-byte decode. This helper propagates the carry
    // through all 5 bytes.

    auto addToLeb128Size = [](unsigned char *bytes, size_t delta) {
        for (int i = 0; i < 5 && delta != 0; ++i) {
            auto val {bytes[i] & 0x7F};

            val += delta;

            bytes[i] = static_cast<unsigned char>((val & 0x7F) | (bytes[i] & 0x80));

            delta = val >> 7;
        }
    };

    // Check that the module is a valid WebAssembly binary with a magic number and version. If not, just return without
    // patching.

    if ((pWasmModule.size() < 8)
        || (pWasmModule[0] != 0x00) || (pWasmModule[1] != 0x61)
        || (pWasmModule[2] != 0x73) || (pWasmModule[3] != 0x6D)
        || (pWasmModule[4] != 0x01) || (pWasmModule[5] != 0x00)
        || (pWasmModule[6] != 0x00) || (pWasmModule[7] != 0x00)) {
        return;
    }

    // Patch the WebAssembly module by scanning its sections to find the import section and the target_features custom
    // section. If the

    size_t pos {8};

    while (pos < pWasmModule.size()) {
        auto sectionId {pWasmModule[pos++]};
        auto sizeFieldPos {pos};
        auto sectionSize {decodeULEB128(pWasmModule.data(), pos)};
        auto sectionContentStart {pos}; // Note: this is not the same value as sizeFieldPos since pos gets incremented
                                        //       by decodeULEB128().
        auto sectionEnd {sectionContentStart + sectionSize};

        if (sectionId == 0) { // Custom section.
            // Look for the target_features custom section to check if it contains the shared-mem feature.

            auto nameSize {decodeULEB128(pWasmModule.data(), pos)};
            std::string_view sectionName(reinterpret_cast<const char *>(pWasmModule.data()) + pos, nameSize);

            pos += nameSize;

            if (sectionName == "target_features") {
                size_t countPos {pos};
                auto count {decodeULEB128(pWasmModule.data(), pos)};
                auto hasSharedMem {false};

                for (size_t i = 0; i < count && pos < sectionEnd; ++i) {
                    auto prefix {pWasmModule[pos++]};
                    auto featureNameSize {decodeULEB128(pWasmModule.data(), pos)};

                    if ((prefix == static_cast<unsigned char>('+'))
                        && (featureNameSize == 10)
                        && (memcmp(pWasmModule.data() + pos, "shared-mem", 10) == 0)) {
                        hasSharedMem = true;
                    }

                    pos += featureNameSize;
                }

                if (!hasSharedMem) {
                    // The shared-mem feature is not present, so we need to add it. We do this by appending a new
                    // feature entry to the end of the section content and updating the section size and feature count
                    // accordingly.

                    static constexpr unsigned char SHARED_MEM_FEATURE[] = {
                        static_cast<unsigned char>('+'),
                        0x0A,
                        's',
                        'h',
                        'a',
                        'r',
                        'e',
                        'd',
                        '-',
                        'm',
                        'e',
                        'm',
                    };
                    static constexpr auto SHARED_MEM_FEATURE_SIZE = sizeof(SHARED_MEM_FEATURE);

                    pWasmModule.insert(pWasmModule.begin() + static_cast<std::vector<unsigned char>::difference_type>(sectionEnd),
                                       SHARED_MEM_FEATURE, SHARED_MEM_FEATURE + SHARED_MEM_FEATURE_SIZE);

                    pWasmModule[countPos] = static_cast<unsigned char>(count + 1);

                    addToLeb128Size(pWasmModule.data() + sizeFieldPos, SHARED_MEM_FEATURE_SIZE);

                    sectionEnd += SHARED_MEM_FEATURE_SIZE;
                }
            }
        } else if (sectionId == 2) { // Import section.
            auto numImports {decodeULEB128(pWasmModule.data(), pos)};

            for (size_t i = 0; i < numImports && pos < sectionEnd; ++i) {
                // Decode and skip the module name.

                auto moduleSize {decodeULEB128(pWasmModule.data(), pos)};

                pos += moduleSize;

                // Decode and skip the field name.

                auto fieldNameSize {decodeULEB128(pWasmModule.data(), pos)};

                pos += fieldNameSize;

                if (pos >= sectionEnd) {
                    break;
                }

                // Decode the kind of import and handle it accordingly.

                auto importKind {pWasmModule[pos++]};

                if (importKind == 0x00) {
                    // Function import: skip the type index.

                    decodeULEB128(pWasmModule.data(), pos);
                } else if (importKind == 0x01) {
                    // Table import: skip the element type and limits.

                    if (pos < sectionEnd) {
                        ++pos; // Element type.

                        auto limitsFlags {pWasmModule[pos++]};

                        decodeULEB128(pWasmModule.data(), pos); // Initial limit.

                        if (limitsFlags & 0x01) {
                            decodeULEB128(pWasmModule.data(), pos); // Maximum limit.
                        }
                    }
                } else if (importKind == 0x02) {
                    // Memory import: patch flags to mark it as shared and add a maximum value if not present.

                    if (pos < sectionEnd) {
                        pWasmModule[pos] = 0x03; // Set flags to 0x03 (i.e. has_maximum | shared).

                        ++pos; // Advance past the flags byte.

                        decodeULEB128(pWasmModule.data(), pos); // Initial value (should be 0 for LLVM modules).

                        // Encode the maximum value.

                        static constexpr size_t RUNTIME_MEMORY_MAX = 32768;
                        unsigned char runtimeMemory[5];
                        size_t runtimeMemorySize = encodeULEB128(runtimeMemory, RUNTIME_MEMORY_MAX);

                        // Insert the maximum value right after the initial value.

                        pWasmModule.insert(pWasmModule.begin() + static_cast<std::vector<unsigned char>::difference_type>(pos),
                                           runtimeMemory, runtimeMemory + runtimeMemorySize);

                        // Update the section size to account for the new maximum value.

                        addToLeb128Size(pWasmModule.data() + sizeFieldPos, runtimeMemorySize);

                        // Update the section end to account for the new maximum value.

                        sectionEnd += runtimeMemorySize;
                    }

                    break;
                } else if (importKind == 0x03) {
                    // Global import: skip the value type and mutability.

                    pos += 2;
                }
            }
        }

        pos = sectionEnd;
    }
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

    auto diagnosticOptions {std::make_unique<clang::DiagnosticOptions>()};
    std::string diagnostics;
    llvm::raw_string_ostream outputStream(diagnostics);
    auto diagnosticsEngine {llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine>(std::make_unique<clang::DiagnosticsEngine>(llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(std::make_unique<clang::DiagnosticIDs>()),
                                                                                                                          *diagnosticOptions,
                                                                                                                          new clang::TextDiagnosticPrinter(outputStream, *diagnosticOptions)))};

    diagnosticsEngine->setWarningsAsErrors(true);

    // Get a driver object and ask it not to check that input files exist.

    clang::driver::Driver driver("clang", llvm::sys::getProcessTriple(), *diagnosticsEngine);

    driver.setCheckInputsExist(false);

    // Get a compilation object to which we pass some arguments.

    static constexpr auto DUMMY_FILE_NAME {"dummy.c"};
    static const std::vector<const char *> COMPILATION_ARGUMENTS {{"clang", "-fsyntax-only",
                                                                   "-O3",
                                                                   "-fno-math-errno",
                                                                   "-fno-trapping-math",
                                                                   "-fno-stack-protector",
                                                                   "-funroll-loops",
                                                                   DUMMY_FILE_NAME}};

    std::unique_ptr<clang::driver::Compilation> compilation(driver.BuildCompilation(COMPILATION_ARGUMENTS));

#ifndef CODE_COVERAGE_ENABLED
    if (compilation == nullptr) {
        addError("A compilation object could not be created.");

        return false;
    }
#endif

    // The compilation object should have one command, so if it doesn't then something went wrong.

    clang::driver::JobList &jobs {compilation->getJobs()};

#ifndef CODE_COVERAGE_ENABLED
    if ((jobs.size() != 1) || !llvm::isa<clang::driver::Command>(*jobs.begin())) {
        addError("The compilation object must have one command.");

        return false;
    }
#endif

    // Retrieve the command and make sure that its name is "clang".

    auto &command {llvm::cast<clang::driver::Command>(*jobs.begin())};

#ifndef CODE_COVERAGE_ENABLED
    static constexpr auto CLANG {"clang"};

    if (strcmp(command.getCreator().getName(), CLANG) != 0) {
        const std::string commandName(command.getCreator().getName());
        std::string error;

        error.reserve(commandName.size() + 47); // NOLINT

        error += "The command name must be 'clang' while it is '";
        error += commandName;
        error += "'.";

        addError(error);

        return false;
    }
#endif

    // Prevent the Clang driver from asking cc1 to leak memory, this by removing -disable-free from the command
    // arguments.

    auto commandArguments {command.getArguments()};

#ifdef CODE_COVERAGE_ENABLED
    commandArguments.erase(find(commandArguments, llvm::StringRef("-disable-free")));
#else
    auto *commandArgument {find(commandArguments, llvm::StringRef("-disable-free"))};

    if (commandArgument != commandArguments.end()) {
        commandArguments.erase(commandArgument);
    }
#endif

    // Create a compiler instance.

    auto compilerInstance {std::make_unique<clang::CompilerInstance>()};

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

    std::string code {R"(// Arithmetic operators.

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

)"};

    code += pCode;

    compilerInstance->getInvocation().getPreprocessorOpts().addRemappedFile(DUMMY_FILE_NAME,
                                                                            llvm::MemoryBuffer::getMemBuffer(code).release());

    // Compile the given code, resulting in an LLVM bitcode module.

    auto llvmContext {std::make_unique<llvm::LLVMContext>()};
    auto codeGenAction {std::make_unique<clang::EmitLLVMOnlyAction>(llvmContext.get())};

    if (!compilerInstance->ExecuteAction(*codeGenAction)) {
        addError("The given code could not be compiled.");

        static constexpr auto ERROR {": error: "};
        static auto ERROR_LENGTH {strlen(ERROR)};
        static constexpr auto NOTE {": note: "};
        static auto NOTE_LENGTH {strlen(NOTE)};

        std::istringstream input(diagnostics);
        std::string line;

        std::getline(input, line);

        while (!input.eof()) {
            std::string error {line.substr(line.find(ERROR) + ERROR_LENGTH) + ":"};

            error[0] = static_cast<char>(std::toupper(error[0]));

            std::getline(input, line);

            auto hasErrorDetails {false};

            while (!input.eof()) {
                const auto notePos {line.find(NOTE)};

                if (notePos != std::string::npos) {
                    if (hasErrorDetails) {
                        error += "\n";
                    } else {
                        error[error.size() - 1] = ' ';
                    }

                    error += line.substr(notePos + NOTE_LENGTH);
                    error += ":";
                } else if (line.find(DUMMY_FILE_NAME) != std::string::npos) {
                    break;
                } else {
                    error += "\n";
                    error += line;

                    hasErrorDetails = true;
                }

                std::getline(input, line);
            }

            addError(error);
        }

        return false;
    }

    // Retrieve the LLVM module.

    auto module {codeGenAction->takeModule()};

#ifndef CODE_COVERAGE_ENABLED
    if (module == nullptr) {
        addError("The LLVM module could not be retrieved.");

        return false;
    }
#endif

#ifdef __EMSCRIPTEN__
    // Ensure all functions have atomics enabled in their target features. Clang doesn't add +atomics by default for
    // WebAssembly targets, but without it the WebAssembly backend won't mark the memory import as shared, causing
    // instantiation failures when loading the compiled module into a shared memory environment.

    for (auto &func : *module) {
        if (func.isDeclaration()) {
            continue;
        }

        const auto featuresAttr {func.getFnAttribute("target-features")};

        if (featuresAttr.isValid()) {
            std::string featuresString {featuresAttr.getValueAsString()};

            if (featuresString.find("+atomics") == std::string::npos) {
                featuresString += ",+atomics";

                func.addFnAttr("target-features", featuresString);
            }
        }
    }
#endif

    // Initialise the native target and its ASM printer.

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

#ifdef __EMSCRIPTEN__
    // Look up the target.

    std::string error;
    auto target {llvm::TargetRegistry::lookupTarget(module->getTargetTriple(), error)};

    if (target == nullptr) {
        error[0] = static_cast<char>(tolower(error[0]));

        const auto targetTriple {module->getTargetTriple().str()};
        std::string targetError;

        targetError.reserve(targetTriple.size() + error.size() + 32); // NOLINT

        targetError += "the target (";
        targetError += targetTriple;
        targetError += ") could not be found: ";
        targetError += error;

        addError(targetError);

        return false;
    }

    // Create a target machine.

    auto targetMachine {std::unique_ptr<llvm::TargetMachine>(target->createTargetMachine(module->getTargetTriple(),
                                                                                         "generic", "+atomics,+bulk-memory",
                                                                                         llvm::TargetOptions(),
                                                                                         llvm::Reloc::Static,
                                                                                         std::nullopt,
                                                                                         llvm::CodeGenOptLevel::Aggressive))};

    if (targetMachine == nullptr) {
        addError("A target machine could not be created.");

        return false;
    }

    // Get the target machine to emit some WebAssembly code.

    llvm::legacy::PassManager passManager;
    llvm::SmallVector<char, 0> outputBuffer;
    llvm::raw_svector_ostream output(outputBuffer);

    if (targetMachine->addPassesToEmitFile(passManager, output, nullptr, llvm::CodeGenFileType::ObjectFile)) {
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

    // Patch the WebAssembly code to ensure that it can be loaded into a shared memory environment.

    patchWasmSharedMemory(pWasmModule);

    return true;
#else
    // Create an ORC-based JIT with aggressive code generation and keep track of it.

    auto jitTargetMachineBuilder {llvm::orc::JITTargetMachineBuilder(llvm::Triple(llvm::sys::getProcessTriple()))};

    jitTargetMachineBuilder.setCodeGenOptLevel(llvm::CodeGenOptLevel::Aggressive);

    auto lljit {llvm::orc::LLJITBuilder().setJITTargetMachineBuilder(std::move(jitTargetMachineBuilder)).create()};

#    ifndef CODE_COVERAGE_ENABLED
    if (!lljit) {
        auto llvmError {llvmClangError(lljit.takeError())};
        std::string error;

        error.reserve(37 + llvmError.size()); // NOLINT

        error += "An ORC-based JIT could not be created";
        error += llvmError;
        error += ".";

        addError(error);

        return false;
    }
#    endif

    mLljit = std::move(*lljit);

    // Make sure that we can find various mathematical functions in the standard C library.

    auto dynamicLibrarySearchGenerator {llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(mLljit->getDataLayout().getGlobalPrefix())};

#    ifndef CODE_COVERAGE_ENABLED
    if (!dynamicLibrarySearchGenerator) {
        auto llvmError {llvmClangError(dynamicLibrarySearchGenerator.takeError())};
        std::string error;

        error.reserve(56 + llvmError.size()); // NOLINT

        error += "The dynamic library search generator could not be created";
        error += llvmError;
        error += ".";

        addError(error);

        return false;
    }
#    endif

    mLljit->getMainJITDylib().addGenerator(std::move(*dynamicLibrarySearchGenerator));

    // Add our LLVM bitcode module to our ORC-based JIT.

    auto threadSafeModule {llvm::orc::ThreadSafeModule(std::move(module), std::move(llvmContext))};

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

    const bool res {!mLljit->getMainJITDylib().define(llvm::orc::absoluteSymbols({
        {mLljit->mangleAndIntern(pName), llvm::orc::ExecutorSymbolDef(llvm::orc::ExecutorAddr::fromPtr(pFunction), llvm::JITSymbolFlags::Exported)},
    }))};

#    ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        std::string error;

        error.reserve(pName.size() + 48); // NOLINT

        error += "The ";
        error += pName;
        error += "() function could not be added to the compiler.";

        addError(error);
    }
#    endif

    return res;
}

void *Compiler::Impl::function(const std::string &pName) const
{
    // Return the address of the requested function. Note that we assume that we have a valid ORC-based JIT and function
    // name.

    auto symbol {mLljit->lookup(pName)};

    if (symbol) {
        return reinterpret_cast<void *>(symbol->getValue()); // NOLINT
    }

    return nullptr;
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
