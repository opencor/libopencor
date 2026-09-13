# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(PACKAGE_NAME LLVMClang)
set(PACKAGE_VERSION 22.1.8)
set(PACKAGE_REPOSITORY llvm-project)
set(RELEASE_TAG llvmorg-22.1.8)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LLVMCLANG)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         f2cc407da19873669391aaee0cfa0de87bfd4e45)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     38d26a253f5eddfa256be16adad7d1bd4434dd20)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     2e0ae5200b16854a5b74ee7482a26ee7700ee023)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     8b0172b7de13c9a86735778edee503f4f2265a72)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     acd1aeac95765256cff01fca60b9ffff8f49c208)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 573e4963e2c73e4ae5bac2f0fa3c37f370fd501b)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 052828c7510107cddf01a119421e0f4d38435508)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 309840bef9888908789bdec3bf5e344aafcffc6f)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 a437231186bc1442c92e673f7da5e6fd5f8d8cd2)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.
    # Note #1: we determine Clang's -cc1 arguments ourselves rather than have Clang's driver compute them for us (see
    #          compiler.cpp), this so that we don't drag all of Clang's toolchains into libOpenCOR. So, we don't need
    #          to build Clang's driver at all (see CLANG_ENABLE_DRIVER below).
    # Note #2: we only ever compile code for the architecture that we run on, so we don't need the target support that
    #          Clang would otherwise build for every single architecture that it knows about. CLANG_ENABLE_ALL_TARGETS
    #          (see below) narrows it down to LLVM_TARGETS_TO_BUILD, which is why that variable must name exactly one
    #          architecture.
    # Note #3: libOpenCOR's WASM version doesn't use LLVM+Clang's typeinfo (e.g., the JIT parts of compiler.cpp are
    #          not compiled for WebAssembly), so we can disable RTTI to reduce the size of our WASM package. On the
    #          other hand, libOpenCOR's native versions do use LLVM+Clang's typeinfo (e.g., compiler.cpp instantiates
    #          LLVM+Clang templates whose typeinfo references LLVM+Clang's own typeinfo), so we must enable RTTI for
    #          them.

    if(EMSCRIPTEN)
        set(LLVM_TARGETS_TO_BUILD WebAssembly)
        set(LLVM_HOST_TRIPLE wasm32-unknown-emscripten)

        set(LLVM_ENABLE_LIBCXX
            -DLLVM_ENABLE_LIBCXX=ON
        )

        set(LLVM_ENABLE_RTTI
            -DLLVM_ENABLE_RTTI=OFF
        )
    else()
        if(INTEL_MODE)
            set(LLVM_TARGETS_TO_BUILD X86)

            if(WIN32)
                set(LLVM_HOST_TRIPLE x86_64-pc-windows-msvc)
            elseif(APPLE)
                set(LLVM_HOST_TRIPLE x86_64-apple-darwin)
            else()
                set(LLVM_HOST_TRIPLE x86_64-pc-linux-gnu)
            endif()
        else()
            set(LLVM_TARGETS_TO_BUILD AArch64)

            if(WIN32)
                set(LLVM_HOST_TRIPLE aarch64-pc-windows-msvc)
            elseif(APPLE)
                set(LLVM_HOST_TRIPLE aarch64-apple-darwin)
            else()
                set(LLVM_HOST_TRIPLE aarch64-pc-linux-gnu)
            endif()
        endif()

        set(LLVM_ENABLE_RTTI
            -DLLVM_ENABLE_RTTI=ON
        )
    endif()

    # Build our WASM version of LLVM+Clang with size optimisation.
    # Note #1: this doesn't affect the quality of the code that LLVM+Clang generates for us. It only affects its own
    #          size and, to some extent, how fast it can compile code.
    # Note #2: we only do this for our WASM version, whose size is what matters most to us since our Web app
    #          dynamically imports it. Our native versions keep LLVM+Clang's default release flags so that they
    #          compile our models as fast as possible.

    if(EMSCRIPTEN)
        set(LLVMCLANG_RELEASE_FLAGS
            "-DCMAKE_CXX_FLAGS_RELEASE=-Oz -DNDEBUG"
            "-DCMAKE_C_FLAGS_RELEASE=-Oz -DNDEBUG"
        )
    endif()

    # Build LLVM+Clang.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        SOURCE_SUBDIR
            llvm
        PATCH_COMMAND
            ${CMAKE_COMMAND} -DLLVMCLANG_SOURCE_DIR=<SOURCE_DIR>
                             -DLLVMCLANG_PATCHES_DIR=${CMAKE_CURRENT_SOURCE_DIR}/patches
                             -P ${CMAKE_CURRENT_SOURCE_DIR}/applypatches.cmake
        CMAKE_ARGS
            -DCLANG_BUILD_TOOLS=OFF
            -DCLANG_ENABLE_ALL_TARGETS=OFF
            -DCLANG_ENABLE_DRIVER=OFF
            -DCLANG_ENABLE_INTERPRETER=OFF
            -DCLANG_ENABLE_LIBXML2=OFF
            -DCLANG_ENABLE_STATIC_ANALYZER=OFF
            -DCLANG_ENABLE_TOOLING_LIBS=OFF
            -DCLANG_INCLUDE_DOCS=OFF
            -DCLANG_INCLUDE_TESTS=OFF
            -DCLANG_PLUGIN_SUPPORT=OFF
            -DCLANG_TOOL_CLANG_DIFF_BUILD=OFF
            -DCLANG_TOOL_CLANG_IMPORT_TEST_BUILD=OFF
            -DCLANG_TOOL_CLANG_SHLIB_BUILD=OFF
            -DCLANG_TOOL_DICTIONARY_BUILD=OFF
            -DCLANG_TOOL_HANDLE_CXX_BUILD=OFF
            -DCLANG_TOOL_HANDLE_LLVM_BUILD=OFF
            -DCLANG_TOOL_LIBCLANG_BUILD=OFF
            ${CMAKE_ARGS}
            ${LLVMCLANG_RELEASE_FLAGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLLVM_BUILD_LLVM_C_DYLIB=OFF
            -DLLVM_BUILD_RUNTIME=OFF
            -DLLVM_BUILD_RUNTIMES=OFF
            -DLLVM_BUILD_TOOLS=OFF
            -DLLVM_BUILD_UTILS=OFF
            -DLLVM_ENABLE_ASSERTIONS=OFF
            -DLLVM_ENABLE_BACKTRACES=OFF
            -DLLVM_ENABLE_BINDINGS=OFF
            -DLLVM_ENABLE_CRASH_OVERRIDES=OFF
            -DLLVM_ENABLE_DIA_SDK=OFF
            -DLLVM_ENABLE_LIBEDIT=OFF
            -DLLVM_ENABLE_LIBPFM=OFF
            -DLLVM_ENABLE_LIBXML2=OFF
            ${LLVM_ENABLE_LIBCXX}
            -DLLVM_ENABLE_MODULE_DEBUGGING=OFF
            -DLLVM_ENABLE_OCAMLDOC=OFF
            -DLLVM_ENABLE_ONDISK_CAS=OFF
            -DLLVM_ENABLE_PEDANTIC=OFF
            -DLLVM_ENABLE_PLUGINS=OFF
            -DLLVM_ENABLE_PROJECTS=clang
            ${LLVM_ENABLE_RTTI}
            -DLLVM_ENABLE_TELEMETRY=OFF
            -DLLVM_ENABLE_UNWIND_TABLES=OFF
            -DLLVM_ENABLE_WARNINGS=OFF
            -DLLVM_ENABLE_ZLIB=OFF
            -DLLVM_ENABLE_ZSTD=OFF
            -DLLVM_HOST_TRIPLE=${LLVM_HOST_TRIPLE}
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_DOCS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_RUNTIMES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DLLVM_INCLUDE_TOOLS=ON
            -DLLVM_INCLUDE_UTILS=OFF
            -DLLVM_TARGETS_TO_BUILD=${LLVM_TARGETS_TO_BUILD}
            -DLLVM_TOOL_LTO_BUILD=OFF
            -DLLVM_TOOL_REMARKS_SHLIB_BUILD=OFF
    )

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   bin include lib)
endif()

# Make our package accessible.

# The libraries that are specific to the platform for which we are building, i.e. the backend of the target for which
# LLVM+Clang generates code and, for our native versions only, LLVM's ORC-based JIT (our WASM version doesn't JIT
# anything; see compiler.cpp).
# Note #1: we never assemble or disassemble any code, so we don't need our target's assembly parser or disassembler.
#          As elsewhere in this file, the libraries that we explicitly don't want are commented out rather than
#          omitted.
# Note #2: we don't need Clang's driver since we determine Clang's -cc1 arguments ourselves (see compiler.cpp).
#          clangOptions, which Clang's option table lives in and which clang::CompilerInvocation::CreateFromArgs()
#          needs, is a separate library, so it stays. So does LLVMWindowsDriver, which, despite its name, is not
#          only used by Clang's driver: LLVM's ORC-based JIT needs it to locate the MSVC toolchain.

if(EMSCRIPTEN)
    set(EXTRA_LLVMCLANG_LIBRARIES
        # LLVMWebAssemblyAsmParser
        LLVMWebAssemblyCodeGen
        LLVMWebAssemblyDesc
        # LLVMWebAssemblyDisassembler
        LLVMWebAssemblyInfo
        LLVMWebAssemblyUtils
    )
else()
    set(EXTRA_LLVMCLANG_LIBRARIES
        # clangDriver
        LLVMExecutionEngine
        LLVMJITLink
        LLVMOrcJIT
        LLVMOrcShared
        LLVMOrcTargetProcess
        LLVMRuntimeDyld
        LLVMWindowsDriver
    )

    if(INTEL_MODE)
        list(APPEND EXTRA_LLVMCLANG_LIBRARIES
            # LLVMX86AsmParser
            LLVMX86CodeGen
            LLVMX86Desc
            # LLVMX86Disassembler
            LLVMX86Info
        )
    else()
        list(APPEND EXTRA_LLVMCLANG_LIBRARIES
            # LLVMAArch64AsmParser
            LLVMAArch64CodeGen
            LLVMAArch64Desc
            # LLVMAArch64Disassembler
            LLVMAArch64Info
            LLVMAArch64Utils
        )
    endif()
endif()

set(LLVMCLANG_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LLVMCLANG_CMAKE_DIR ${INSTALL_DIR}/lib/cmake/clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LLVMCLANG_CMAKE_DIRS ${INSTALL_DIR}/lib/cmake/llvm ${INSTALL_DIR}/lib/cmake/clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake directories.")
set(LLVMCLANG_CMAKE_PACKAGE_NAME Clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LLVMCLANG_CMAKE_PACKAGE_NAMES LLVM Clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake package names.")
set(LLVMCLANG_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")

# The LLVM+Clang libraries that libOpenCOR needs on every platform, i.e. all of them except for those that are only
# used by LLVM+Clang's own tools and/or by libclang, none of which we use. Those are commented out below, so that it
# is clear which libraries we explicitly don't want. The platform-specific ones are added through
# EXTRA_LLVMCLANG_LIBRARIES above.
# Note: this list is also what decides which of LLVM+Clang's archives get merged into our static native library (see
#       src/CMakeLists.txt), so it must be the full transitive closure of the libraries that we link against, and not
#       just those that we reference directly. This is why, for instance, LLVMSymbolize and the debug information
#       libraries that it needs are in the list even though we never symbolise anything: LLVMProfileData, which
#       clangCodeGen needs, depends on them.

set(LLVMCLANG_LIBRARIES
    clangAnalysis
    # clangAnalysisFlowSensitive
    # clangAnalysisFlowSensitiveModels
    clangAnalysisLifetimeSafety
    # clangAnalysisScalable
    clangAPINotes
    clangAST
    clangASTMatchers
    clangBasic
    clangCodeGen
    # clangCrossTU
    # clangDependencyScanning
    # clangDirectoryWatcher
    # clangDynamicASTMatchers
    clangEdit
    # clangExtractAPI
    # clangFormat
    clangFrontend
    # clangFrontendTool
    # clangIndex
    # clangIndexSerialization
    # clangInstallAPI
    # clangInterpreter
    clangLex
    clangOptions
    clangParse
    # clangRewrite
    # clangRewriteFrontend
    clangSema
    clangSerialization
    # clangStaticAnalyzerCheckers
    # clangStaticAnalyzerCore
    # clangStaticAnalyzerFrontend
    clangSupport
    # clangTooling
    # clangToolingASTDiff
    # clangToolingCore
    # clangToolingInclusions
    # clangToolingInclusionsStdlib
    # clangToolingRefactoring
    # clangToolingSyntax
    # clangTransformer
    # LLVMABI
    LLVMAggressiveInstCombine
    LLVMAnalysis
    LLVMAsmParser
    LLVMAsmPrinter
    LLVMBinaryFormat
    LLVMBitReader
    LLVMBitstreamReader
    LLVMBitWriter
    # LLVMCAS
    LLVMCFGuard
    # LLVMCFIVerify
    LLVMCGData
    LLVMCodeGen
    LLVMCodeGenTypes
    LLVMCore
    LLVMCoroutines
    LLVMCoverage
    LLVMDebugInfoBTF
    LLVMDebugInfoCodeView
    # LLVMDebuginfod
    LLVMDebugInfoDWARF
    LLVMDebugInfoDWARFLowLevel
    LLVMDebugInfoGSYM
    # LLVMDebugInfoLogicalView
    LLVMDebugInfoMSF
    LLVMDebugInfoPDB
    LLVMDemangle
    # LLVMDiff
    # LLVMDlltoolDriver
    # LLVMDTLTO
    # LLVMDWARFCFIChecker
    # LLVMDWARFLinker
    # LLVMDWARFLinkerClassic
    # LLVMDWARFLinkerParallel
    # LLVMDWP
    # LLVMExegesis
    LLVMExtensions
    # LLVMFileCheck
    LLVMFrontendAtomic
    LLVMFrontendDirective
    LLVMFrontendDriver
    LLVMFrontendHLSL
    LLVMFrontendOffloading
    # LLVMFrontendOpenACC
    LLVMFrontendOpenMP
    # LLVMFuzzerCLI
    # LLVMFuzzMutate
    LLVMGlobalISel
    LLVMHipStdPar
    LLVMInstCombine
    LLVMInstrumentation
    # LLVMInterfaceStub
    # LLVMInterpreter
    LLVMipo
    LLVMIRPrinter
    LLVMIRReader
    # LLVMLibDriver
    # LLVMLineEditor
    LLVMLinker
    LLVMLTO
    LLVMMC
    # LLVMMCA
    LLVMMCDisassembler
    # LLVMMCJIT
    LLVMMCParser
    # LLVMMIRParser
    LLVMObjCARCOpts
    # LLVMObjCopy
    LLVMObject
    LLVMObjectYAML
    # LLVMOptDriver
    LLVMOption
    # LLVMOrcDebugging
    LLVMPasses
    LLVMPlugins
    LLVMProfileData
    LLVMRemarks
    LLVMSandboxIR
    LLVMScalarOpts
    LLVMSelectionDAG
    LLVMSupport
    # LLVMSupportLSP
    LLVMSymbolize
    # LLVMTableGen
    # LLVMTableGenBasic
    # LLVMTableGenCommon
    LLVMTarget
    LLVMTargetParser
    # LLVMTelemetry
    LLVMTextAPI
    # LLVMTextAPIBinaryReader
    LLVMTransformUtils
    LLVMVectorize
    # LLVMWindowsManifest
    # LLVMXRay
    ${EXTRA_LLVMCLANG_LIBRARIES}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
