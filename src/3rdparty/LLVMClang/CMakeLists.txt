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
set(PACKAGE_VERSION 14.0.6)
set(PACKAGE_REPOSITORY llvm-project)
set(RELEASE_TAG llvmorg-14.0.6-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LLVMCLANG)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 5bd0c82b3015a170ac2e53942872fc9bb1e4e250)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 8979f92eb44c9612082efc619c58609f391a06e6)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 c9dddc138d6dc9ea90554152f5274fb4820a52c5)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 5ff417c2906c15dee2155116fb8882df5ec0b762)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             d0738ed7481fcae143c1f8b52a1ea82ad765d09e)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             868de8ec69bb19f2bc229a8a256d5ba3bdd59f54)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             965d782bcc86872230315c0915fadb0219cfe9b5)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             582fc94cd57deed6674454eb94a24e759797b5bc)
        endif()
    endif()
else()
    # Build our package.

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

    if(LLVMCLANG_LLVM_TABLEGEN)
        set(LLVM_TABLEGEN -DLLVM_TABLEGEN=${LLVMCLANG_LLVM_TABLEGEN})
    endif()

    if(LLVMCLANG_CLANG_TABLEGEN)
        set(CLANG_TABLEGEN -DCLANG_TABLEGEN=${LLVMCLANG_CLANG_TABLEGEN})
    endif()

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        SOURCE_SUBDIR
            llvm
        CMAKE_ARGS
            -DCLANG_BUILD_TOOLS=OFF
            -DCLANG_ENABLE_ARCMT=OFF
            -DCLANG_ENABLE_STATIC_ANALYZER=OFF
            -DCLANG_PLUGIN_SUPPORT=OFF
            ${CLANG_TABLEGEN}
            -DCLANG_TOOLING_BUILD_AST_INTROSPECTION=OFF
            -DCLANG_TOOL_HANDLE_CXX_BUILD=OFF
            -DCLANG_TOOL_HANDLE_LLVM_BUILD=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBCLANG_BUILD_STATIC=ON
            -DLIBOPENCOR=ON
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
            -DLLVM_ENABLE_PEDANTIC=OFF
            -DLLVM_ENABLE_PLUGINS=OFF
            -DLLVM_ENABLE_PROJECTS=clang
            -DLLVM_ENABLE_RTTI=ON
            -DLLVM_ENABLE_TERMINFO=OFF
            -DLLVM_ENABLE_UNWIND_TABLES=OFF
            -DLLVM_ENABLE_WARNINGS=OFF
            -DLLVM_ENABLE_ZLIB=OFF
            -DLLVM_HOST_TRIPLE=${LLVM_HOST_TRIPLE}
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_DOCS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_RUNTIMES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DLLVM_INCLUDE_TOOLS=ON
            -DLLVM_INCLUDE_UTILS=OFF
            ${LLVM_TABLEGEN}
            -DLLVM_TARGETS_TO_BUILD=${LLVM_TARGETS_TO_BUILD}
    )

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   bin include lib)
endif()

# Make our package accessible.

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

set(LLVMCLANG_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LLVMCLANG_CMAKE_DIR ${INSTALL_DIR}/lib/cmake/clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LLVMCLANG_CMAKE_DIRS ${INSTALL_DIR}/lib/cmake/llvm ${INSTALL_DIR}/lib/cmake/clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake directories.")
set(LLVMCLANG_CMAKE_PACKAGE_NAME Clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LLVMCLANG_CMAKE_PACKAGE_NAMES LLVM Clang CACHE INTERNAL "${PACKAGE_NAME}'s CMake package names.")
set(LLVMCLANG_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LLVMCLANG_LIBRARIES
    clangAnalysis
    clangAnalysisFlowSensitive
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
    clangFormat
    clangFrontend
    clangFrontendTool
    clangIndex
    clangIndexSerialization
    clangInterpreter
    clangLex
    clangParse
    clangRewrite
    clangRewriteFrontend
    clangSema
    clangSerialization
    clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore
    clangStaticAnalyzerFrontend
    clangTesting
    clangTooling
    clangToolingASTDiff
    clangToolingCore
    clangToolingInclusions
    clangToolingRefactoring
    clangToolingSyntax
    clangTransformer
    libclang
    LLVMAggressiveInstCombine
    LLVMAnalysis
    LLVMAsmParser
    LLVMAsmPrinter
    LLVMBinaryFormat
    LLVMBitReader
    LLVMBitstreamReader
    LLVMBitWriter
    LLVMCFGuard
    LLVMCFIVerify
    LLVMCodeGen
    LLVMCore
    LLVMCoroutines
    LLVMCoverage
    LLVMDebugInfoCodeView
    LLVMDebuginfod
    LLVMDebugInfoDWARF
    LLVMDebugInfoGSYM
    LLVMDebugInfoMSF
    LLVMDebugInfoPDB
    LLVMDemangle
    LLVMDiff
    LLVMDlltoolDriver
    LLVMDWARFLinker
    LLVMDWP
    LLVMExecutionEngine
    LLVMExegesis
    LLVMExtensions
    LLVMFileCheck
    LLVMFrontendOpenACC
    LLVMFrontendOpenMP
    LLVMFuzzMutate
    LLVMGlobalISel
    LLVMInstCombine
    LLVMInstrumentation
    LLVMInterfaceStub
    LLVMInterpreter
    LLVMipo
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
    LLVMObject
    LLVMObjectYAML
    LLVMOption
    LLVMOrcJIT
    LLVMOrcShared
    LLVMOrcTargetProcess
    LLVMPasses
    LLVMProfileData
    LLVMRemarks
    LLVMRuntimeDyld
    LLVMScalarOpts
    LLVMSelectionDAG
    LLVMSupport
    LLVMSymbolize
    LLVMTableGen
    LLVMTableGenGlobalISel
    LLVMTarget
    LLVMTextAPI
    LLVMTransformUtils
    LLVMVectorize
    LLVMWindowsManifest
    LLVMXRay
    ${EXTRA_LLVMCLANG_LIBRARIES}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
