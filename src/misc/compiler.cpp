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
#include "irgenerator.h"

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/OptBisect.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#include <climits>
#include <cstring>

namespace libOpenCOR {

namespace {

#ifndef CODE_COVERAGE_ENABLED
std::string llvmErrorMessage(llvm::Error pError)
{
    std::string res;

    llvm::handleAllErrors(std::move(pError), [&res](const llvm::ErrorInfoBase &errorInfo) {
        res = errorInfo.message();
    });

    res[0] = static_cast<char>(toupper(res[0]));

    return " (" + res + ")";
}
#endif

void optimise(llvm::Module &pModule, llvm::TargetMachine &pTargetMachine)
{
    // Optimise the given module in the same way as Clang does at -O3 (with -funroll-loops, -vectorize-loops, and
    // -vectorize-slp), i.e. using LLVM's default per-module pipeline and default alias analysis pipeline.
    // Note: for our WASM version, we don't vectorise straight-line code (i.e. we don't use -vectorize-slp) since, for
    //       the code that libCellML generates for our models (i.e. straight-line code), it makes no measurable
    //       difference to the time it takes to compute a model while it is expensive to do.

    llvm::PipelineTuningOptions pipelineTuningOptions;

    pipelineTuningOptions.LoopUnrolling = true;
    pipelineTuningOptions.LoopInterleaving = true;
    pipelineTuningOptions.LoopVectorization = true;
#ifdef __EMSCRIPTEN__
    pipelineTuningOptions.SLPVectorization = false;
#else
    pipelineTuningOptions.SLPVectorization = true;
#endif

    // Skip dead store elimination (DSE) since, for the code that libCellML generates for our models, it doesn't remove
    // anything (each array element is stored at most once per function and the local arrays of our NLA systems are all
    // read by nlaSolve()) while it accounts for a significant part of the time it takes to optimise a model.

    llvm::PassInstrumentationCallbacks passInstrumentationCallbacks;

    passInstrumentationCallbacks.registerShouldRunOptionalPassCallback([](llvm::StringRef pPassId, const llvm::Any & /*pIr*/) {
        return pPassId != "DSEPass";
    });

    llvm::PassBuilder passBuilder(&pTargetMachine, pipelineTuningOptions, std::nullopt, &passInstrumentationCallbacks);
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cgsccAnalysisManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;

    functionAnalysisManager.registerPass([&passBuilder] {
        return passBuilder.buildDefaultAAPipeline();
    });

    passBuilder.registerModuleAnalyses(moduleAnalysisManager);
    passBuilder.registerCGSCCAnalyses(cgsccAnalysisManager);
    passBuilder.registerFunctionAnalyses(functionAnalysisManager);
    passBuilder.registerLoopAnalyses(loopAnalysisManager);
    passBuilder.crossRegisterProxies(loopAnalysisManager, functionAnalysisManager, cgsccAnalysisManager, moduleAnalysisManager);

    passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3).run(pModule, moduleAnalysisManager);
}

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

    // Initialise the native target and its ASM printer, once and for all, and in a thread-safe way (hence the static
    // initialisation).

    static const auto nativeTargetInitialised {[] {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();

        return true;
    }()};

    (void)nativeTargetInitialised;

    // Create a target machine for our target, i.e. a generic WebAssembly CPU with the atomics, bulk memory and SIMD
    // features (which libOpenCOR itself is built with) for our WASM version and the host CPU for our native versions
    // (since we JIT our code on the machine that runs it).
    // Note #1: our target machine is used to optimise our code (e.g., to decide whether and how to vectorise it) and,
    //          for our WASM version, to generate it. Our native versions generate it using our ORC-based JIT, which
    //          uses its own target machine, but for the same host CPU and features.
    // Note #2: Clang's driver maps the host CPU's features onto its own list of AArch64 extensions, which we cannot do
    //          without it, so on AArch64 we optimise our code using the features that the host CPU implies. On x86-64,
    //          we do use the host CPU's features, as the driver does for -march=native. Indeed, the operating system
    //          may not support all the features that the host CPU's name implies (e.g., AVX in some virtual machines),
    //          in which case getHostCPUFeatures() reports them as disabled.

#ifdef __EMSCRIPTEN__
    // Note: the features that Clang uses are those of the generic CPU and those that we want to use (i.e. atomics, bulk
    //       memory, and SIMD).

    static constexpr auto TRIPLE {"wasm32-unknown-emscripten"};
    static constexpr auto FEATURES {"+atomics,+bulk-memory,+bulk-memory-opt,+call-indirect-overlong,+multivalue,+mutable-globals,+nontrapping-fptoint,+reference-types,+sign-ext,+simd128"};

    std::string error;
    auto target {llvm::TargetRegistry::lookupTarget(llvm::Triple(TRIPLE), error)};

    if (target == nullptr) {
        addError("The target (" + std::string(TRIPLE) + ") could not be found (" + error + ").");

        return false;
    }

    std::unique_ptr<llvm::TargetMachine> targetMachine {target->createTargetMachine(llvm::Triple(TRIPLE), "generic", FEATURES,
                                                                                    llvm::TargetOptions(),
                                                                                    llvm::Reloc::Static,
                                                                                    std::nullopt,
                                                                                    llvm::CodeGenOptLevel::Aggressive)};

    if (targetMachine == nullptr) {
        addError("A target machine could not be created.");

        return false;
    }
#else
    auto jitTargetMachineBuilder {llvm::orc::JITTargetMachineBuilder::detectHost()};

#    ifndef CODE_COVERAGE_ENABLED
    if (!jitTargetMachineBuilder) {
        addError("A target machine builder for the host system could not be created" + llvmErrorMessage(jitTargetMachineBuilder.takeError()) + ".");

        return false;
    }
#    endif

    jitTargetMachineBuilder->setCodeGenOptLevel(llvm::CodeGenOptLevel::Aggressive);

    auto optimisationTargetMachineBuilder {*jitTargetMachineBuilder};

#    ifndef BUILDING_ON_INTEL
    optimisationTargetMachineBuilder.getFeatures() = llvm::SubtargetFeatures();
#    endif

    auto expectedTargetMachine {optimisationTargetMachineBuilder.createTargetMachine()};

#    ifndef CODE_COVERAGE_ENABLED
    if (!expectedTargetMachine) {
        addError("A target machine could not be created" + llvmErrorMessage(expectedTargetMachine.takeError()) + ".");

        return false;
    }
#    endif

    auto targetMachine {std::move(*expectedTargetMachine)};
#endif

    // Generate some LLVM IR for the given code.
    // Note: we generate the IR that Clang would have generated for us, i.e. the IR that its driver would have generated
    //       for
    //           clang -O3 -fno-math-errno -fno-trapping-math -fno-stack-protector -funroll-loops -march=native
    //       (with -mcpu=native rather than -march=native on AArch64), and our IR generator only needs to know about the
    //       parts of it that depend on our target. For our WASM version, it is the IR for wasm32-unknown-emscripten with
    //       -fvisibility=hidden and a static relocation model (we don't link our code, we just instantiate it).

    static const auto irGeneratorTarget {[&targetMachine] {
        IrGeneratorTarget res;

        res.triple = targetMachine->getTargetTriple().str();
        res.dataLayout = targetMachine->createDataLayout().getStringRepresentation();
        res.longBits = static_cast<unsigned int>(sizeof(long) * CHAR_BIT); // NOLINT
        res.functionAttributes = {
            {"no-trapping-math", "true"},
            {"stack-protector-buffer-size", "8"},
            {"target-cpu", targetMachine->getTargetCPU().str()},
        };

        // Note: the module flags are those that Clang sets (in that order), i.e. the size of wchar_t (2 bytes on
        //       Windows and 4 bytes elsewhere) and, for our native versions, the PIC level (-pic-level 2), the PIE
        //       level (-pic-is-pie, on Linux only), the kind of unwind tables (-funwind-tables), and the frame pointer
        //       (-mframe-pointer, unless it is none).

        static constexpr auto ERROR {static_cast<unsigned int>(llvm::Module::Error)};
        [[maybe_unused]] static constexpr auto MAX {static_cast<unsigned int>(llvm::Module::Max)};
        [[maybe_unused]] static constexpr auto BIG_PIC {static_cast<unsigned int>(llvm::PICLevel::BigPIC)};
        [[maybe_unused]] static constexpr auto SYNC_UWTABLE {static_cast<unsigned int>(llvm::UWTableKind::Sync)};
        [[maybe_unused]] static constexpr auto ASYNC_UWTABLE {static_cast<unsigned int>(llvm::UWTableKind::Async)};

#ifdef __EMSCRIPTEN__
        res.functionAttributes.emplace_back("target-features", FEATURES);
        res.unprototypedDeclarationAttributes.emplace_back("no-prototype", "");

        res.moduleFlags = {{ERROR, "wchar_size", 4}};
        res.largeArrayMinBits = 128; // NOLINT
        res.largeArrayAlignment = 128; // NOLINT
        res.definitionVisibility = llvm::GlobalValue::HiddenVisibility;
        res.dsoLocalDefinitions = true;
#else
#    if defined(BUILDING_ON_WINDOWS)
#        if defined(BUILDING_ON_INTEL)
        res.functionAttributes.emplace_back("frame-pointer", "none");

        res.moduleFlags = {{ERROR, "wchar_size", 2},
                           {MAX, "PIC Level", BIG_PIC},
                           {MAX, "uwtable", ASYNC_UWTABLE}};
#        else
        res.functionAttributes.emplace_back("frame-pointer", "reserved");

        res.moduleFlags = {{ERROR, "wchar_size", 2},
                           {MAX, "PIC Level", BIG_PIC},
                           {MAX, "uwtable", ASYNC_UWTABLE},
                           {MAX, "frame-pointer", static_cast<unsigned int>(llvm::FramePointerKind::Reserved)}};
#        endif

        res.uwtable = ASYNC_UWTABLE;
        res.dsoLocalDefinitions = true;
        res.dsoLocalDeclarations = true;
#    elif defined(BUILDING_ON_LINUX)
        static constexpr auto LARGE_PIE {static_cast<unsigned int>(llvm::PIELevel::Large)};

#        if defined(BUILDING_ON_INTEL)
        res.functionAttributes.emplace_back("frame-pointer", "none");

        res.moduleFlags = {{ERROR, "wchar_size", 4},
                           {MAX, "PIC Level", BIG_PIC},
                           {MAX, "PIE Level", LARGE_PIE},
                           {MAX, "uwtable", ASYNC_UWTABLE}};
#        else
        res.functionAttributes.emplace_back("frame-pointer", "non-leaf-no-reserve");

        res.moduleFlags = {{ERROR, "wchar_size", 4},
                           {MAX, "PIC Level", BIG_PIC},
                           {MAX, "PIE Level", LARGE_PIE},
                           {MAX, "uwtable", ASYNC_UWTABLE},
                           {MAX, "frame-pointer", static_cast<unsigned int>(llvm::FramePointerKind::NonLeafNoReserve)}};
#        endif

        res.uwtable = ASYNC_UWTABLE;
        res.dsoLocalDefinitions = true;
#    elif defined(BUILDING_ON_INTEL)
        res.functionAttributes.emplace_back("frame-pointer", "all");

        res.moduleFlags = {{ERROR, "wchar_size", 4},
                           {MAX, "PIC Level", BIG_PIC},
                           {MAX, "uwtable", ASYNC_UWTABLE},
                           {MAX, "frame-pointer", static_cast<unsigned int>(llvm::FramePointerKind::All)}};
        res.uwtable = ASYNC_UWTABLE;
#    else
        res.functionAttributes.emplace_back("frame-pointer", "non-leaf-no-reserve");

        res.moduleFlags = {{ERROR, "wchar_size", 4},
                           {MAX, "PIC Level", BIG_PIC},
                           {MAX, "uwtable", SYNC_UWTABLE},
                           {MAX, "frame-pointer", static_cast<unsigned int>(llvm::FramePointerKind::NonLeafNoReserve)}};
        res.uwtable = SYNC_UWTABLE;
#    endif

#    ifdef BUILDING_ON_INTEL
        res.functionAttributes.emplace_back("target-features", targetMachine->getTargetFeatureString().str());
        res.functionAttributes.emplace_back("min-legal-vector-width", "0");

        res.largeArrayMinBits = 128; // NOLINT
        res.largeArrayAlignment = 128; // NOLINT
#    endif
#endif

        return res;
    }()};

    // Note: as Clang does (with -discard-value-names), we discard the names of the values that are created since we
    //       have no use for them.

    auto llvmContext {std::make_unique<llvm::LLVMContext>()};

    llvmContext->setDiscardValueNames(true);

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_ARM)
    // Skip LLVM's machine instruction scheduler on Apple Silicon since, there, it accounts for about a third of the time
    // it takes to compile a big model (AArch64 schedules in both directions while tracking register pressure) while it
    // makes no measurable difference to the time it takes to compute it (Apple's cores are wide out-of-order cores).
    // Note: an OptPassGate is given the argument of a (legacy) pass, i.e. "machine-scheduler" in our case.

    class NoMachineSchedulerPassGate: public llvm::OptPassGate
    {
    public:
        [[nodiscard]] bool shouldRunPass(llvm::StringRef pPassName, llvm::StringRef /*pIrDescription*/) const override
        {
            return pPassName != "machine-scheduler";
        }

        [[nodiscard]] bool isEnabled() const override
        {
            return true;
        }
    };

    static NoMachineSchedulerPassGate noMachineSchedulerPassGate;

    llvmContext->setOptPassGate(noMachineSchedulerPassGate);
#endif

    std::string irGeneratorError;
    auto module {generateIr(pCode, *llvmContext, irGeneratorTarget, irGeneratorError)};

    if (module == nullptr) {
        addError("The given code could not be compiled.");
        addError(irGeneratorError);

        return false;
    }

    // Optimise our LLVM IR.

    optimise(*module, *targetMachine);

#if defined(BUILDING_ON_WINDOWS) && defined(BUILDING_ON_ARM)
    // On Windows on ARM, our ORC-based JIT uses RuntimeDyld which relocates the address of an external function (e.g.,
    // cos() when it is used as a function pointer) using an ADRP instruction, i.e. within ±4 GB of our code and without
    // checking that range. So, we would end up with an invalid address for a function that is further away (as is
    // typically the case for a function in a DLL). To avoid this, we import our external functions (as if they were in
    // a DLL), in which case RuntimeDyld stores their full address next to our code.

    for (auto &function : *module) {
        if (function.isDeclaration() && !function.isIntrinsic()) {
            function.setDSOLocal(false);
            function.setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);
        }
    }
#endif

#ifdef __EMSCRIPTEN__
    // Get our target machine to emit some WebAssembly code.

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
    // Create an ORC-based JIT with our target machine builder for the host system.

    auto lljit {llvm::orc::LLJITBuilder().setJITTargetMachineBuilder(std::move(*jitTargetMachineBuilder)).create()};

#    ifndef CODE_COVERAGE_ENABLED
    if (!lljit) {
        addError("An ORC-based JIT could not be created" + llvmErrorMessage(lljit.takeError()) + ".");

        return false;
    }
#    endif

    mLljit = std::move(*lljit);

    // Make sure that we can find various mathematical functions in the standard C library.

    auto dynamicLibrarySearchGenerator {llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(mLljit->getDataLayout().getGlobalPrefix())};

#    ifndef CODE_COVERAGE_ENABLED
    if (!dynamicLibrarySearchGenerator) {
        addError("The dynamic library search generator could not be created" + llvmErrorMessage(dynamicLibrarySearchGenerator.takeError()) + ".");

        return false;
    }
#    endif

    mLljit->getMainJITDylib().addGenerator(std::move(*dynamicLibrarySearchGenerator));

    // Add our LLVM IR module to our ORC-based JIT.

    const bool res {!mLljit->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(llvmContext)))};

#    ifndef CODE_COVERAGE_ENABLED
    if (!res) {
        addError("The LLVM IR module could not be added to the ORC-based JIT.");
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
    : Logger(std::make_unique<Impl>())
{
}

Compiler::~Compiler() = default;

Compiler::Impl *Compiler::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl.get());
}

const Compiler::Impl *Compiler::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl.get());
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
