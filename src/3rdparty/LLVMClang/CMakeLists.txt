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
set(RELEASE_TAG llvmorg-22.1.8-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LLVMCLANG)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         4f2906d4b6df1b0be5a636a88f7582425b30df49)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     7eca038345d2bb5354d469da999fe38ac7a51e96)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     38178c9f5550499a796c24ecebe5d45325e34e66)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     0a77b2f65286f89a4175cc52ceaf07909a7cec4e)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     b394f358e96039d5d844547b2cafda678a824776)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 da706ed5933abedcee5e9f8c6006b71b061c5ebf)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 30642e4e208a800e60b8ada95e15425c89a4b4f9)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 540878be30175da91557ab1a9f546383b458a06e)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 2a68f667a7c571561416451a543024fff9096012)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.

    if(EMSCRIPTEN)
        set(LIBCLANG_BUILD_STATIC OFF)

        set(LLVM_TARGETS_TO_BUILD WebAssembly)
        set(LLVM_HOST_TRIPLE wasm32-unknown-emscripten)

        find_program(LLVM_TBLGEN_EXE NAMES llvm-tblgen)
        find_program(CLANG_TBLGEN_EXE NAMES clang-tblgen)

        if(NOT LLVM_TBLGEN_EXE OR NOT CLANG_TBLGEN_EXE)
            message(FATAL_ERROR "llvm-tblgen and clang-tblgen executables are required to build ${PACKAGE_NAME} with Emscripten.")
        endif()

        set(TABLEGENS
            -DLLVM_TABLEGEN=${LLVM_TBLGEN_EXE}
            -DCLANG_TABLEGEN=${CLANG_TBLGEN_EXE}
        )
    else()
        set(LIBCLANG_BUILD_STATIC ON)

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
    endif()

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        SOURCE_SUBDIR
            llvm
        CMAKE_ARGS
            -DCLANG_BUILD_TOOLS=OFF
            -DCLANG_ENABLE_LIBXML2=OFF
            -DCLANG_ENABLE_STATIC_ANALYZER=OFF
            -DCLANG_INCLUDE_DOCS=OFF
            -DCLANG_INCLUDE_TESTS=OFF
            -DCLANG_PLUGIN_SUPPORT=OFF
            -DCLANG_TOOL_DICTIONARY_BUILD=OFF
            -DCLANG_TOOL_HANDLE_CXX_BUILD=OFF
            -DCLANG_TOOL_HANDLE_LLVM_BUILD=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBCLANG_BUILD_STATIC=${LIBCLANG_BUILD_STATIC}
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
            -DLLVM_ENABLE_MODULE_DEBUGGING=OFF
            -DLLVM_ENABLE_OCAMLDOC=OFF
            -DLLVM_ENABLE_ONDISK_CAS=OFF
            -DLLVM_ENABLE_PEDANTIC=OFF
            -DLLVM_ENABLE_PLUGINS=OFF
            -DLLVM_ENABLE_PROJECTS=clang
            -DLLVM_ENABLE_RTTI=ON
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
            ${TABLEGENS}
    )

    # Create our package.
    # Note: we also package llvm-tblgen and clang-tblgen since we need them to build our JavaScript bindings.

    if(NOT EMSCRIPTEN)
        set(TABLEGEN_EXES_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build/LLVMClang-prefix/src/LLVMClang-build)

        file(GLOB TABLEGEN_EXES "${TABLEGEN_EXES_BUILD_DIR}/bin/*tblgen")

        foreach(TABLEGEN_EXE ${TABLEGEN_EXES})
            file(COPY ${TABLEGEN_EXE} DESTINATION ${INSTALL_DIR}/bin)
        endforeach()

        string(REPLACE "${TABLEGEN_EXES_BUILD_DIR}/" "" TABLEGEN_EXES "${TABLEGEN_EXES}")
    endif()

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   bin include lib)
endif()

# Make our package accessible.

if(EMSCRIPTEN)
    set(EXTRA_LLVMCLANG_LIBRARIES
        LLVMWebAssemblyAsmParser
        LLVMWebAssemblyCodeGen
        LLVMWebAssemblyDesc
        LLVMWebAssemblyDisassembler
        LLVMWebAssemblyInfo
        LLVMWebAssemblyUtils
    )
else()
    if(INTEL_MODE)
        set(EXTRA_LLVMCLANG_LIBRARIES
            LLVMExegesisX86
            LLVMX86AsmParser
            LLVMX86CodeGen
            LLVMX86Desc
            LLVMX86Disassembler
            LLVMX86Info
            LLVMX86TargetMCA
        )
    else()
        set(EXTRA_LLVMCLANG_LIBRARIES
            LLVMAArch64AsmParser
            LLVMAArch64CodeGen
            LLVMAArch64Desc
            LLVMAArch64Disassembler
            LLVMAArch64Info
            LLVMAArch64Utils
            LLVMExegesisAArch64
        )
    endif()
endif()

set(LLVMCLANG_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LLVMCLANG_CMAKE_DIR ${INSTALL_DIR}/lib/cmake/clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LLVMCLANG_CMAKE_DIRS ${INSTALL_DIR}/lib/cmake/llvm ${INSTALL_DIR}/lib/cmake/clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake directories.")
set(LLVMCLANG_CMAKE_PACKAGE_NAME Clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LLVMCLANG_CMAKE_PACKAGE_NAMES LLVM Clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake package names.")
set(LLVMCLANG_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LLVMCLANG_LIBRARIES
    clangAnalysis
    clangAnalysisFlowSensitive
    clangAnalysisFlowSensitiveModels
    clangAnalysisLifetimeSafety
    clangAnalysisScalable
    clangAPINotes
    clangAST
    clangASTMatchers
    clangBasic
    clangCodeGen
    clangCrossTU
    clangDependencyScanning
    clangDirectoryWatcher
    clangDriver
    clangDynamicASTMatchers
    clangEdit
    clangExtractAPI
    clangFormat
    clangFrontend
    clangFrontendTool
    clangIndex
    clangIndexSerialization
    clangInstallAPI
    clangInterpreter
    clangLex
    clangOptions
    clangParse
    clangRewrite
    clangRewriteFrontend
    clangSema
    clangSerialization
    clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore
    clangStaticAnalyzerFrontend
    clangSupport
    clangTooling
    clangToolingASTDiff
    clangToolingCore
    clangToolingInclusions
    clangToolingInclusionsStdlib
    clangToolingRefactoring
    clangToolingSyntax
    clangTransformer
    LLVMABI
    LLVMAggressiveInstCombine
    LLVMAnalysis
    LLVMAsmParser
    LLVMAsmPrinter
    LLVMBinaryFormat
    LLVMBitReader
    LLVMBitstreamReader
    LLVMBitWriter
    LLVMCAS
    LLVMCFGuard
    LLVMCFIVerify
    LLVMCGData
    LLVMCodeGen
    LLVMCodeGenTypes
    LLVMCore
    LLVMCoroutines
    LLVMCoverage
    LLVMDebugInfoBTF
    LLVMDebugInfoCodeView
    LLVMDebuginfod
    LLVMDebugInfoDWARF
    LLVMDebugInfoDWARFLowLevel
    LLVMDebugInfoGSYM
    LLVMDebugInfoLogicalView
    LLVMDebugInfoMSF
    LLVMDebugInfoPDB
    LLVMDemangle
    LLVMDiff
    LLVMDlltoolDriver
    LLVMDTLTO
    LLVMDWARFCFIChecker
    LLVMDWARFLinker
    LLVMDWARFLinkerClassic
    LLVMDWARFLinkerParallel
    LLVMDWP
    LLVMExecutionEngine
    LLVMExegesis
    LLVMExtensions
    LLVMFileCheck
    LLVMFrontendAtomic
    LLVMFrontendDirective
    LLVMFrontendDriver
    LLVMFrontendHLSL
    LLVMFrontendOffloading
    LLVMFrontendOpenACC
    LLVMFrontendOpenMP
    LLVMFuzzerCLI
    LLVMFuzzMutate
    LLVMGlobalISel
    LLVMHipStdPar
    LLVMInstCombine
    LLVMInstrumentation
    LLVMInterfaceStub
    LLVMInterpreter
    LLVMipo
    LLVMIRPrinter
    LLVMIRReader
    LLVMJITLink
    LLVMLibDriver
    LLVMLineEditor
    LLVMLinker
    LLVMLTO
    LLVMMC
    LLVMMCA
    LLVMMCDisassembler
    LLVMMCJIT
    LLVMMCParser
    LLVMMIRParser
    LLVMObjCARCOpts
    LLVMObjCopy
    LLVMObject
    LLVMObjectYAML
    LLVMOptDriver
    LLVMOption
    LLVMOrcDebugging
    LLVMOrcJIT
    LLVMOrcShared
    LLVMOrcTargetProcess
    LLVMPasses
    LLVMPlugins
    LLVMProfileData
    LLVMRemarks
    LLVMRuntimeDyld
    LLVMSandboxIR
    LLVMScalarOpts
    LLVMSelectionDAG
    LLVMSupport
    LLVMSupportLSP
    LLVMSymbolize
    LLVMTableGen
    LLVMTableGenBasic
    LLVMTableGenCommon
    LLVMTarget
    LLVMTargetParser
    LLVMTelemetry
    LLVMTextAPI
    LLVMTextAPIBinaryReader
    LLVMTransformUtils
    LLVMVectorize
    LLVMWindowsDriver
    LLVMWindowsManifest
    LLVMXRay
    ${EXTRA_LLVMCLANG_LIBRARIES}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
