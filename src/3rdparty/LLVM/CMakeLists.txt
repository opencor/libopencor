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

set(PACKAGE_NAME LLVM)
set(PACKAGE_VERSION 22.1.8)
set(PACKAGE_REPOSITORY llvm-project)
set(RELEASE_TAG llvmorg-22.1.8)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# The LLVM libraries that we need.

# The libraries that are specific to the platform for which we are building, i.e. the backend of the target for which
# LLVM generates code and, for our native versions only, LLVM's ORC-based JIT (our WASM version doesn't JIT anything;
# see compiler.cpp).
# Note #1: we never assemble or disassemble any code, so we don't need our target's assembly parser or disassembler.
# Note #2: LLVMOrcJIT always depends on LLVMWindowsDriver (and therefore on LLVMOption), whatever the platform, since
#          its COFF platform support (used to locate the MSVC toolchain) is built on every host.

if(EMSCRIPTEN)
    set(EXTRA_LLVM_LIBRARIES
        LLVMWebAssemblyCodeGen
        LLVMWebAssemblyDesc
        LLVMWebAssemblyInfo
        LLVMWebAssemblyUtils
    )
else()
    set(EXTRA_LLVM_LIBRARIES
        LLVMExecutionEngine
        LLVMJITLink
        LLVMOption
        LLVMOrcJIT
        LLVMOrcShared
        LLVMOrcTargetProcess
        LLVMRuntimeDyld
        LLVMWindowsDriver
    )

    if(INTEL_MODE)
        # Note: LLVMX86Desc needs LLVMMCDisassembler since it registers an MC relocation info for our target, which
        #       LLVMMCDisassembler provides (see createMCRelocationInfo()).

        list(APPEND EXTRA_LLVM_LIBRARIES
            LLVMMCDisassembler
            LLVMX86CodeGen
            LLVMX86Desc
            LLVMX86Info
        )
    else()
        list(APPEND EXTRA_LLVM_LIBRARIES
            LLVMAArch64CodeGen
            LLVMAArch64Desc
            LLVMAArch64Info
            LLVMAArch64Utils
        )
    endif()
endif()

# The LLVM libraries that libOpenCOR needs on every platform, i.e. those that LLVM's optimisation pipeline and code
# generation need. The platform-specific ones are added through EXTRA_LLVM_LIBRARIES above.
# Note: this list is also the list of LLVM libraries that we build and package (see LLVM_DISTRIBUTION_COMPONENTS
#       below), so it must be the full transitive closure of the libraries that we link against, and not just those
#       that we reference directly (LLVM's configuration fails otherwise, naming the missing library). This is why, for
#       instance, LLVMSymbolize and the debug information libraries that it needs are in the list even though we never
#       symbolise anything: LLVMProfileData, which LLVMPasses needs, depends on them.

set(LLVM_LIBRARIES
    LLVMAggressiveInstCombine
    LLVMAnalysis
    LLVMAsmParser
    LLVMAsmPrinter
    LLVMBinaryFormat
    LLVMBitReader
    LLVMBitstreamReader
    LLVMBitWriter
    LLVMCFGuard
    LLVMCGData
    LLVMCodeGen
    LLVMCodeGenTypes
    LLVMCore
    LLVMCoroutines
    LLVMDebugInfoBTF
    LLVMDebugInfoCodeView
    LLVMDebugInfoDWARF
    LLVMDebugInfoDWARFLowLevel
    LLVMDebugInfoGSYM
    LLVMDebugInfoMSF
    LLVMDebugInfoPDB
    LLVMDemangle
    LLVMFrontendAtomic
    LLVMFrontendDirective
    LLVMFrontendHLSL
    LLVMFrontendOffloading
    LLVMFrontendOpenMP
    LLVMGlobalISel
    LLVMHipStdPar
    LLVMInstCombine
    LLVMInstrumentation
    LLVMipo
    LLVMIRPrinter
    LLVMIRReader
    LLVMLinker
    LLVMMC
    LLVMMCParser
    LLVMObjCARCOpts
    LLVMObject
    LLVMObjectYAML
    LLVMPasses
    LLVMProfileData
    LLVMRemarks
    LLVMSandboxIR
    LLVMScalarOpts
    LLVMSelectionDAG
    LLVMSupport
    LLVMSymbolize
    LLVMTarget
    LLVMTargetParser
    LLVMTextAPI
    LLVMTransformUtils
    LLVMVectorize
    ${EXTRA_LLVM_LIBRARIES}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LLVM)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         e45e31d2a995833b9717f006a88410e1bf8cc103)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     8f3148b572aef0f39aab4026cbef9facccc35e9c)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     13c553bb46e5ff3d2fdb6a53a2bdfa30eeb09cef)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     d0cc8fb14724811cfe07c418b595c8f411c8ebac)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     32007627351d1e3fbafd1926d4f50a4b0bb0f387)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 d81b9cd812c85d9c7e4695235342e9a570b1c4ec)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 10caa0fb75da8227d5074be200c363a76b4f3583)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 f76cc9edadfe2ad3d5928d970676ebee701d0b8a)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 f1d5ce80cb4a5f431f0c8ec64ac1975aa00d1729)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.
    # Note #1: we only build LLVM, not Clang. Indeed, libOpenCOR compiles the C code generated by libCellML itself (i.e.
    #          it generates the corresponding LLVM IR (see src/misc/irgenerator.cpp) and then has LLVM optimise it and
    #          generate some machine code for it (see src/misc/compiler.cpp)).
    # Note #2: we never give LLVM any profile data or vector library, never use LTO, nor enable any optimisation through
    #          a command-line option, a target option or a pipeline tuning option that is off by default (other than SLP
    #          vectorisation for our native versions), and our code is never optimised for size nor uses OpenMP,
    #          coroutines, Objective-C ARC, allocation tokens or alignment assumptions, so we don't need the
    #          optimisations that LLVM's default pipelines only perform in those cases (see
    #          LLVM_ENABLE_OPTIONAL_OPTIMIZATIONS below). Our code never contains any loops either (CellML's mathematics
    #          has no iteration constructs and libCellML generates one assignment per equation), so we don't need LLVM's
    #          loop optimisations (see LLVM_ENABLE_LOOP_OPTIMIZATIONS below), and we always skip dead store elimination
    #          since it never removes anything from our code (see src/misc/compiler.cpp and
    #          LLVM_ENABLE_DEAD_STORE_ELIMINATION below). Similarly, our code never contains any debug information (see
    #          LLVM_ENABLE_DEBUG_INFORMATION below), inline assembly, exception handling, Swift error handling, garbage
    #          collection, stack maps, patch points, statepoints, stack protection or fake uses, and we never need basic
    #          block sections, SFrame unwind tables, Windows secure hot patching or textual assembly (see
    #          LLVM_ENABLE_OPTIONAL_CODE_GENERATION below). We also never debug LLVM itself (see
    #          LLVM_ENABLE_DEBUGGING_FACILITIES below), always generate optimised code (see
    #          LLVM_ENABLE_UNOPTIMIZED_CODE_GENERATION below), only generate code for (and, for our native versions, JIT
    #          it on) the platform for which we build LLVM, so we only need its object format, and only its relocatable
    #          object files (and, for ELF, only those of its class and data encoding), and its target (see
    #          LLVM_ENABLE_ALL_OBJECT_FORMATS and LLVM_ENABLE_ALL_TARGETS_SUPPORT below), and never use static
    #          initialisers, ORC's concurrent compilation or resource tracker transfers, nor JIT arm64e code (see
    #          LLVM_ENABLE_OPTIONAL_JIT_FEATURES below). We also never parse LLVM's command-line options, so we don't
    #          need them to be registered (see LLVM_ENABLE_COMMAND_LINE_OPTIONS below). Finally, our WASM version
    #          doesn't use SLP vectorisation (see src/misc/compiler.cpp and LLVM_ENABLE_SLP_VECTORIZER below), so its
    #          code never contains any vector operations (see LLVM_ENABLE_VECTOR_IR below). None of this changes the
    #          code that LLVM generates for us.
    # Note #3: libOpenCOR never uses LLVM's RTTI (we don't derive from any of LLVM's classes nor use dynamic_cast or
    #          typeid on them), so we build LLVM without RTTI, whatever the platform. We also compile the only files
    #          of ours that include LLVM headers without RTTI (see src/CMakeLists.txt) since some compilers would
    #          otherwise emit, in those files, some typeinfo that references LLVM's own typeinfo.
    # Note #4: we only build and install the LLVM libraries that we need (see LLVM_LIBRARIES), together with
    #          LLVM's headers and CMake files, using LLVM's distribution support. So, our package, as well as the CMake
    #          files that it contains, only include those libraries.

    if(EMSCRIPTEN)
        set(LLVM_TARGETS_TO_BUILD WebAssembly)
        set(LLVM_HOST_TRIPLE wasm32-unknown-emscripten)

        set(LLVM_ENABLE_LIBCXX
            -DLLVM_ENABLE_LIBCXX=ON
        )

        set(LLVM_ENABLE_SLP_VECTORIZER
            -DLLVM_ENABLE_SLP_VECTORIZER=OFF
        )

        set(LLVM_ENABLE_VECTOR_IR
            -DLLVM_ENABLE_VECTOR_IR=OFF
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
    endif()

    # Build our WASM version of LLVM with size optimisation and as LLVM bitcode (-flto), so that it gets optimised as a
    # whole (and stripped of whatever we don't use) when we link our .wasm file (see src/CMakeLists.txt).
    # Note #1: this doesn't affect the quality of the code that LLVM generates for us. It only affects its own size
    #          and, to some extent, how fast it can compile code.
    # Note #2: we only do this for our WASM version, which size is what matters most to us since our Web OpenCOR
    #          dynamically imports it. OpenCOR keeps LLVM's default release flags so that they compile our models as
    #          fast as possible.
    # Note #3: building LLVM as LLVM bitcode makes our .wasm file smaller. We only do this for LLVM since also doing it
    #          for our other third-party libraries, or for libOpenCOR itself, makes our .wasm file bigger again.

    if(EMSCRIPTEN)
        set(LLVM_RELEASE_FLAGS
            "-DCMAKE_CXX_FLAGS_RELEASE=-Oz -flto -DNDEBUG"
            "-DCMAKE_C_FLAGS_RELEASE=-Oz -flto -DNDEBUG"
        )
    endif()

    # Don't build LLVM for x86-64-v3 when using MSVC.
    # Note: MSVC is known to miscompile parts of LLVM when AVX instructions are enabled, which is why LLVM refuses to be
    #       configured with /arch:AVX* (see https://github.com/llvm/llvm-project/issues/54645). So, we don't pass on to
    #       LLVM the flags that we otherwise pass on to our third-party libraries for x86-64-v3 (see packages.cmake).
    #       This only affects how fast LLVM compiles our models, not the code that it generates for them, which targets
    #       the host CPU (see src/misc/compiler.cpp).

    set(LLVM_CMAKE_ARGS ${CMAKE_ARGS})

    if(BUILDING_USING_MSVC AND X86_64_V3_COMPILER_FLAGS)
        list(REMOVE_ITEM LLVM_CMAKE_ARGS ${CMAKE_C_FLAGS_ARGS} ${CMAKE_CXX_FLAGS_ARGS})
    endif()

    # Always build a release version of LLVM.
    # Note: a debug version of LLVM is huge and slow, and we never debug LLVM itself. On Linux and macOS, a debug
    #       version of libOpenCOR simply uses our release package, but on Windows it must use the same C runtime and
    #       _ITERATOR_DEBUG_LEVEL value as LLVM (MSVC's linker fails otherwise), so we build LLVM against the debug
    #       version of the C runtime (which implies _ITERATOR_DEBUG_LEVEL=2, as for libOpenCOR) in that case. LLVM's ABI
    #       doesn't depend on NDEBUG (only on LLVM_ENABLE_ABI_BREAKING_CHECKS, which LLVM_ENABLE_ASSERTIONS=OFF
    #       disables), so this is safe.

    list(REMOVE_ITEM LLVM_CMAKE_ARGS -DCMAKE_BUILD_TYPE=${LIBOPENCOR_BUILD_TYPE})
    list(APPEND LLVM_CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release)

    if(WIN32 AND NOT RELEASE_MODE)
        list(APPEND LLVM_CMAKE_ARGS -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL)
    endif()

    # Apply our patches to LLVM's source code.

    patch_package_args(${PACKAGE_NAME} ${RELEASE_TAG} LLVM_PATCH_ARGS)

    # Build LLVM.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        ${LLVM_PATCH_ARGS}
        DOWNLOAD_NO_PROGRESS ON
        SOURCE_SUBDIR
            llvm
        CMAKE_CACHE_ARGS
            -DLLVM_DISTRIBUTION_COMPONENTS:STRING=${LLVM_LIBRARIES};llvm-headers;cmake-exports
        CMAKE_ARGS
            ${LLVM_CMAKE_ARGS}
            ${LLVM_RELEASE_FLAGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLLVM_BUILD_LLVM_C_DYLIB=OFF
            -DLLVM_BUILD_RUNTIME=OFF
            -DLLVM_BUILD_RUNTIMES=OFF
            -DLLVM_BUILD_TOOLS=OFF
            -DLLVM_BUILD_UTILS=OFF
            -DLLVM_ENABLE_ALL_OBJECT_FORMATS=OFF
            -DLLVM_ENABLE_ALL_TARGETS_SUPPORT=OFF
            -DLLVM_ENABLE_ASSERTIONS=OFF
            -DLLVM_ENABLE_BACKTRACES=OFF
            -DLLVM_ENABLE_BINDINGS=OFF
            -DLLVM_ENABLE_COMMAND_LINE_OPTIONS=OFF
            -DLLVM_ENABLE_CRASH_OVERRIDES=OFF
            -DLLVM_ENABLE_DEAD_STORE_ELIMINATION=OFF
            -DLLVM_ENABLE_DEBUGGING_FACILITIES=OFF
            -DLLVM_ENABLE_DEBUG_INFORMATION=OFF
            -DLLVM_ENABLE_DIA_SDK=OFF
            -DLLVM_ENABLE_LIBEDIT=OFF
            -DLLVM_ENABLE_LIBPFM=OFF
            -DLLVM_ENABLE_LIBXML2=OFF
            ${LLVM_ENABLE_LIBCXX}
            -DLLVM_ENABLE_LOOP_OPTIMIZATIONS=OFF
            -DLLVM_ENABLE_MODULE_DEBUGGING=OFF
            -DLLVM_ENABLE_OCAMLDOC=OFF
            -DLLVM_ENABLE_ONDISK_CAS=OFF
            -DLLVM_ENABLE_OPTIONAL_CODE_GENERATION=OFF
            -DLLVM_ENABLE_OPTIONAL_JIT_FEATURES=OFF
            -DLLVM_ENABLE_OPTIONAL_OPTIMIZATIONS=OFF
            -DLLVM_ENABLE_PEDANTIC=OFF
            -DLLVM_ENABLE_PLUGINS=OFF
            -DLLVM_ENABLE_RTTI=OFF
            ${LLVM_ENABLE_SLP_VECTORIZER}
            -DLLVM_ENABLE_TELEMETRY=OFF
            -DLLVM_ENABLE_UNOPTIMIZED_CODE_GENERATION=OFF
            -DLLVM_ENABLE_UNWIND_TABLES=OFF
            ${LLVM_ENABLE_VECTOR_IR}
            -DLLVM_ENABLE_WARNINGS=OFF
            -DLLVM_ENABLE_ZLIB=OFF
            -DLLVM_ENABLE_ZSTD=OFF
            -DLLVM_HOST_TRIPLE=${LLVM_HOST_TRIPLE}
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_DOCS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_RUNTIMES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DLLVM_INCLUDE_TOOLS=OFF
            -DLLVM_INCLUDE_UTILS=OFF
            -DLLVM_TARGETS_TO_BUILD=${LLVM_TARGETS_TO_BUILD}
        BUILD_COMMAND
            ${CMAKE_COMMAND} --build <BINARY_DIR> --target distribution
        INSTALL_COMMAND
            ${CMAKE_COMMAND} --build <BINARY_DIR> --target install-distribution
    )

    # Remove the headers that we never need (i.e. LLVM's testing headers and TableGen files).

    file(REMOVE_RECURSE ${INSTALL_DIR}/include/llvm/Testing)
    file(GLOB_RECURSE LLVM_TD_FILES ${INSTALL_DIR}/include/*.td)
    file(REMOVE ${LLVM_TD_FILES})

    # Create our package.
    # Note: we don't build any of LLVM's tools, so there is no bin directory to package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(LLVM_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LLVM_CMAKE_DIR ${INSTALL_DIR}/lib/cmake/llvm CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LLVM_CMAKE_PACKAGE_NAME LLVM CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LLVM_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
