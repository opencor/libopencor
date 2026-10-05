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

# Enable compiler caching.

if(LIBOPENCOR_COMPILER_CACHING)
    if(BUILDCACHE_EXE)
        set(CMAKE_C_COMPILER_LAUNCHER ${BUILDCACHE_EXE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${BUILDCACHE_EXE})
    elseif(CLCACHE_EXE)
        set(CLCACHEWRAPPER ${CMAKE_BINARY_DIR}/clcachewrapper)

        message(STATUS "Building clcachewrapper")

        try_compile(CLCACHEWRAPPER_EXE ${CMAKE_BINARY_DIR}
                    SOURCES ${CMAKE_SOURCE_DIR}/cmake/clcachewrapper.c
                    COPY_FILE ${CLCACHEWRAPPER})

        if(CLCACHEWRAPPER_EXE)
            message(STATUS "Building clcachewrapper - Success")

            set(CMAKE_C_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
            set(CMAKE_CXX_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
        else()
            message(STATUS "Building clcachewrapper - Failed")
            message(WARNING "clcachewrapper could not be built, so clcache will not be used.")
        endif()
    else()
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
    endif()
endif()

# Replace some compiler flags when building using MSVC.

if(BUILDING_USING_MSVC)
    # Replace the /Zi option (i.e. enable debugging information), if present, with the /Z7 option (i.e. enable old-style
    # debugging information) otherwise caching (be it with buildcache or clcache) won't work.

    if(BUILDCACHE_EXE OR CLCACHE_EXE)
        replace_compiler_flag("/Zi" "/Z7")
    endif()
endif()

# Prefer maximum optimisation for native non-MSVC release builds.
# Note: -fno-math-errno and -fno-trapping-math are a safe subset of -ffast-math that allow the compiler to vectorise
#       mathematical operations more aggressively.

if(RELEASE_MODE AND NOT BUILDING_USING_MSVC)
    replace_compiler_flag("-O2" "-O3")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-math-errno -fno-trapping-math")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno -fno-trapping-math")
endif()

# Target x86-64-v3 (i.e. AVX2, FMA, etc.) on Intel, so that the compiler can auto-vectorise our code and that of our
# third-party libraries (e.g., SUNDIALS' dense linear algebra which is where most of the simulation time is spent).
# Note #1: we use x86-64-v3 rather than the host CPU so that binaries built on one x86-64 machine run on any other
#          modern x86-64 machine. x86-64-v3 is supported by all CPUs since Intel Haswell (2013) and AMD Excavator
#          (2015). For ARM64, the compiler defaults already include NEON/ASIMD.
# Note #2: MSVC only uses FMA instructions if it is allowed to contract floating-point operations, hence /fp:contract.
# Note #3: we use those flags whatever the build type, so that our release and debug builds give the same results.
# Note #4: the flags are also passed on to our third-party libraries (see packages.cmake and, for SUNDIALS, which uses
#          CMAKE_C_FLAGS, src/3rdparty/SUNDIALS/CMakeLists.txt).
# Note #5: on macOS, we may build for Intel on an ARM machine. Some third-party libraries (e.g., SUNDIALS) run checks
#          that use CMAKE_C_FLAGS but not CMAKE_OSX_ARCHITECTURES, i.e. that compile for the host architecture which
#          fails with -march=x86-64-v3 on an ARM machine. So, we use -Xarch_x86_64 to only use -march=x86-64-v3 when
#          compiling for Intel.

if(INTEL_MODE AND NOT EMSCRIPTEN)
    if(BUILDING_USING_MSVC)
        set(X86_64_V3_COMPILER_FLAGS "/arch:AVX2 /fp:contract")
    elseif(APPLE)
        set(X86_64_V3_COMPILER_FLAGS "-Xarch_x86_64 -march=x86-64-v3")
    else()
        set(X86_64_V3_COMPILER_FLAGS "-march=x86-64-v3")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${X86_64_V3_COMPILER_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${X86_64_V3_COMPILER_FLAGS}")
endif()

# Enable link-time optimisation (LTO) for release builds to allow cross translation unit inlining and devirtualisation.
# Note: we don't enable LTO for code analysis builds since the custom Clang used for analysis produces LLVM bitcode
#       objects when LTO is enabled, which cannot be consumed by the system GNU linker.

if(RELEASE_MODE AND NOT EMSCRIPTEN AND NOT LIBOPENCOR_CODE_ANALYSIS)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

# Build our different third-party libraries.
# Note: for Emscripten, our C/C++ flags are those that src/bindings/javascript/CMakeLists.txt passes on to the
#       Emscripten build that it kicks off, so our third-party libraries are built with the same flags as libOpenCOR
#       (see packages.cmake which passes them on to each third-party library).

add_subdirectory(3rdparty)

if(ONLY_BUILD_THIRD_PARTY_LIBRARIES)
    add_executable(${CMAKE_PROJECT_NAME} dummy.cpp)

    add_subdirectory(bindings)

    return()
endif()

# Configure the version file.

foreach(VERSION_PART PROJECT_VERSION_MAJOR PROJECT_VERSION_PATCH)
    string(LENGTH ${${VERSION_PART}} VERSION_PART_LENGTH)

    if(VERSION_PART_LENGTH EQUAL 1)
        set(${VERSION_PART}_PAD 0)
    endif()
endforeach()

set(LIBOPENCOR_VERSION 0x${PROJECT_VERSION_MAJOR_PAD}${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH_PAD}${PROJECT_VERSION_PATCH})
set(LIBOPENCOR_VERSION_STRING ${PROJECT_VERSION})

set(VERSION_HEADER_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/version.in.h)
set(VERSION_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.h)

configure_file(${VERSION_HEADER_FILE_IN} ${VERSION_HEADER_FILE})

# Build libOpenCOR.

if(NOT WIN32 AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(GIT_API_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/filemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/issue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedabstracttask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedanalysis.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedchange.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedchangeattribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seddatadescription.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seddatagenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seddocument.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedinstancetask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedonestep.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedoutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedrepeatedtask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedsimulation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedsteadystate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedstyle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedtask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seduniformtimecourse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solvercvode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverforwardeuler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverfourthorderrungekutta.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverheun.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverkinsol.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solvernla.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverodefixedstep.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solversecondorderrungekutta.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/version.h
)

set(API_EXPORT_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/api/${CMAKE_PROJECT_NAME_LC}/export.h)

set(API_HEADER_FILES
    ${GIT_API_HEADER_FILES}
    ${API_EXPORT_HEADER_FILE}
)

set(GIT_API_MODULE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module/${CMAKE_PROJECT_NAME_LC})
set(API_MODULE_FILE ${GIT_API_MODULE_FILE})

set(INTERNAL_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/irgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvercvode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverforwardeuler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverfourthorderrungekutta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverheun.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverkinsol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvernla.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverodefixedstep.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solversecondorderrungekutta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile.cpp
)

set(GIT_API_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/issue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedabstracttask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedanalysis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchange.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchangeattribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatadescription.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatagenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddocument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstancetask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedonestep.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedoutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedrepeatedtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsimulation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsteadystate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedstyle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seduniformtimecourse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
)

set(GIT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/file_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/issue_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/logger_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedabstracttask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedbase_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchange_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchangeattribute_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatadescription_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatagenerator_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddocument_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstance_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstancetask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedmodel_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedonestep_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedoutput_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedrepeatedtask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsimulation_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsteadystate_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedstyle_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedtask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seduniformtimecourse_p.h
    ${GIT_API_SOURCE_FILES}
    ${INTERNAL_SOURCE_FILES}
)

set(SOURCE_FILES
    ${GIT_API_SOURCE_FILES}
    ${INTERNAL_SOURCE_FILES}
)

# Compile the only files that include LLVM headers without RTTI, as is LLVM itself (see
# src/3rdparty/LLVM/CMakeLists.txt).
# Note #1: with RTTI, compilers may emit the typeinfo of some of the LLVM classes that are fully defined in LLVM's
#          headers (e.g., GCC in release mode, for classes derived from llvm::orc::MaterializationUnit and
#          llvm::vfs::FileSystem, and Clang in debug mode, for llvm::ErrorInfo<>) and that typeinfo references the
#          typeinfo of LLVM classes that LLVM doesn't provide since it is built without RTTI.
# Note #2: those files don't use RTTI (e.g., dynamic_cast or typeid) and nothing uses RTTI on the classes that they
#          define (e.g., Compiler).
# Note #3: MSVC doesn't have that problem and to mix /GR and /GR- files is not safe with MSVC's STL (e.g.,
#          std::function's layout depends on _HAS_STATIC_RTTI), so we only do this for GCC and Clang.
# Note #4: our WASM version doesn't have that problem either (it doesn't use LLVM's ORC-based JIT) and Emscripten's
#          Embind requires RTTI.
# Note #5: without RTTI, Clang emits, in those files, a local copy of the typeinfo of the standard exceptions that
#          libc++'s inline code may throw (e.g., std::out_of_range for std::vector::at()). The linker then uses that
#          copy for the whole of libOpenCOR, including for the catch clauses of our third-party libraries (e.g.,
#          libCellML's catch (std::out_of_range &) around std::stoi()). However, libc++'s type_info comparison never
#          considers such a copy equal to libc++'s own typeinfo, which is what an exception thrown from within libc++
#          (e.g., by std::stoi()) uses, so such an exception would never be caught (e.g., libCellML's parser would
#          terminate the process on a units prefix that is out of the int range). Those files don't use exceptions, so
#          we also compile them without exceptions on macOS, which means that no such typeinfo gets emitted (libc++
#          supports mixing files compiled with and without exceptions). This is not needed on Linux since libstdc++'s
#          throwing code isn't inline and its type_info comparison relies on names.

if(NOT EMSCRIPTEN AND NOT BUILDING_USING_MSVC)
    set(NO_RTTI_COMPILE_OPTIONS -fno-rtti)

    if(APPLE)
        list(APPEND NO_RTTI_COMPILE_OPTIONS -fno-exceptions)
    endif()

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler.cpp
                                ${CMAKE_CURRENT_SOURCE_DIR}/misc/irgenerator.cpp
                                PROPERTIES COMPILE_OPTIONS "${NO_RTTI_COMPILE_OPTIONS}")
endif()

set(INTERNAL_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/irgenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solver_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvercvode_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverforwardeuler_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverfourthorderrungekutta_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverheun_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverkinsol_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvernla_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverode_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverodefixedstep_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solversecondorderrungekutta_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/unittestingexport.h
)

set(GIT_HEADER_FILES
    ${INTERNAL_HEADER_FILES}
    ${THIRD_PARTY_HEADER_FILES}
    ${VERSION_HEADER_FILE_IN}
)

set(HEADER_FILES
    ${INTERNAL_HEADER_FILES}
    ${VERSION_HEADER_FILE}
)

include_directories(file
                    logger
                    misc
                    solver
                    support/cellml
                    support/combine
                    support/sedml
                    ${CMAKE_CURRENT_BINARY_DIR})

set(JAVASCRIPT_SOURCE_DIR tests/bindings/javascript)

if(EMSCRIPTEN)
    set(JAVASCRIPT_BINDINGS_DIR ${CMAKE_BINARY_DIR}/../../../../${JAVASCRIPT_SOURCE_DIR})

    get_filename_component(JAVASCRIPT_BINDINGS_DIR "${JAVASCRIPT_BINDINGS_DIR}" REALPATH)
else()
    set(JAVASCRIPT_BINDINGS_DIR ${CMAKE_BINARY_DIR}/${JAVASCRIPT_SOURCE_DIR})
endif()

if(EMSCRIPTEN)
    add_executable(${CMAKE_PROJECT_NAME}
                   ${API_HEADER_FILES}
                   ${SOURCE_FILES}
                   ${HEADER_FILES})

    # Set the flags for our generated .js and .wasm files.
    # Note #1: for compiling, we use -O3 to get the best performance, -matomics to enable atomic operations,
    #          -mbulk-memory to enable bulk memory operations, -pthread to enable native threading support,
    #          -sSHARED_MEMORY=1 to enable shared memory support, and -DNDEBUG to disable debugging information (it is
    #          not needed in production).
    # Note #2: for linking, we use -O2 (rather than -O3) since our compressed .wasm file is then smaller with no loss of
    #          performance (and it links faster), -flto to use the LTO version of Emscripten's system libraries and to
    #          optimise LLVM, which we build as LLVM bitcode (see src/3rdparty/LLVM/CMakeLists.txt; our other object
    #          files are not LTO ones), -pthread to enable native threading support, -sALLOW_TABLE_GROWTH to allow the
    #          WebAssembly table to grow, -sASSERTIONS=0 to disable assertions (they are not needed in production),
    #          -sEXPORT_ES6=1 to export ES6 modules, -sEXPORTED_FUNCTIONS to export the functions we need (i.e. those
    #          needed by the WebAssembly modules that we generate for our models, see initialiseWorkerWasmJS()),
    #          -sEXPORTED_RUNTIME_METHODS to export the runtime methods we need, -sINITIAL_MEMORY=2GB to have enough
    #          memory for "big" models, -sMODULARIZE=1 to generate a module factory function, -sSHARED_MEMORY=1 to
    #          enable shared memory support, -sSTACK_SIZE=8MB to reserve a reasonable amount of memory for the stack at
    #          startup (which is needed for "big" models), --bind to generate the necessary Embind bindings, and
    #          --emit-tsd to generate a TypeScript type definitions file.
    # Note #3: for linking, we used to have -sALLOW_MEMORY_GROWTH=1, -sINITIAL_MEMORY=256MB, and -sMAXIMUM_MEMORY=2GB,
    #          but we now support threading and to allow memory growth might non-WASM code to run slowly. So, now, we
    #          use -sINITIAL_MEMORY=2GB and don't allow memory growth.

    set(DTS_FILE ${CMAKE_PROJECT_NAME}.d.ts)

    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
                           -O3
                           -matomics
                           -mbulk-memory
                           -pthread
                           -sSHARED_MEMORY=1
                           -DNDEBUG)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
                        -O2
                        -flto
                        -pthread
                        -sALLOW_TABLE_GROWTH
                        -sASSERTIONS=0
                        -sEXPORT_ES6=1
                        -sEXPORTED_FUNCTIONS=[_free,_malloc,_memset,_nlaSolverAddress,_wasmNlaSolve,_pow,_sqrt,_fabs,_exp,_exp2,_log,_log10,_ceil,_floor,_fmin,_fmax,_fmod,_sin,_cos,_tan,_sinh,_cosh,_tanh,_asin,_acos,_atan,_asinh,_acosh,_atanh]
                        -sEXPORTED_RUNTIME_METHODS=HEAPU8
                        -sINITIAL_MEMORY=2GB
                        -sMODULARIZE=1
                        -sSHARED_MEMORY=1
                        -sSTACK_SIZE=8MB
                        --bind
                        --emit-tsd ${DTS_FILE})

    # Package our JavaScript bindings, if possible.

    find_program(NODE_EXE NAMES node)
    find_program(NPM_EXE NAMES ${PREFERRED_NPM_NAMES} npm)

    if(NODE_EXE AND NPM_EXE)
        # Post-process the generated .d.ts to match our EM_ASM runtime patches.

        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${NODE_EXE} ${CMAKE_SOURCE_DIR}/src/bindings/javascript/postprocess.dts.mjs ${DTS_FILE})

        # Package our JavaScript bindings into a .tgz file that can be published to npm.

        set(TGZ_FILENAME ${REAL_CMAKE_PROJECT_NAME}-${PROJECT_VERSION}.tgz)
        set(TGZ_FILE ${CMAKE_BINARY_DIR}/../../../../${TGZ_FILENAME})

        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory dist
                           COMMAND ${CMAKE_COMMAND} -E copy ${DTS_FILE} dist
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.js dist
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.wasm dist
                           COMMAND ${NPM_EXE} pack
                           COMMAND ${CMAKE_COMMAND} -E copy opencor-${TGZ_FILENAME} ${TGZ_FILE})

        # Also package our .d.ts file as a standalone TypeScript type definitions package.

        set(TYPES_TGZ_FILENAME ${REAL_CMAKE_PROJECT_NAME}-types-${PROJECT_VERSION}.tgz)
        set(TYPES_TGZ_FILE ${CMAKE_BINARY_DIR}/../../../../${TYPES_TGZ_FILENAME})

        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory types/dist
                           COMMAND ${CMAKE_COMMAND} -E copy package.types.json types/package.json
                           COMMAND ${CMAKE_COMMAND} -E copy README.md types
                           COMMAND ${CMAKE_COMMAND} -E copy LICENSE.txt types
                           COMMAND ${CMAKE_COMMAND} -E copy ${DTS_FILE} types/dist
                           COMMAND ${CMAKE_COMMAND} -E chdir types ${NPM_EXE} pack
                           COMMAND ${CMAKE_COMMAND} -E copy types/opencor-${TYPES_TGZ_FILENAME} ${TYPES_TGZ_FILE})
    else()
        message(FATAL_ERROR "The node and/or npm tools could not be found, so libOpenCOR's JavaScript bindings could not be finalised and packaged.")
    endif()

    # Copy our generated files over so that they can be deployed.

    foreach(DEPLOY_DIR ${CMAKE_PROJECT_VERSION} latest)
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory wasm/${DEPLOY_DIR}
                           COMMAND ${CMAKE_COMMAND} -E copy ${DTS_FILE} wasm/${DEPLOY_DIR}
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.js wasm/${DEPLOY_DIR}
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.wasm wasm/${DEPLOY_DIR})
    endforeach()

    # Copy our generated files over so that they can be used by our test Web page.

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${DTS_FILE} ${JAVASCRIPT_BINDINGS_DIR}/${DTS_FILE}
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.js ${JAVASCRIPT_BINDINGS_DIR}
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.wasm ${JAVASCRIPT_BINDINGS_DIR})

    # Let people know how to run our test Web page.

    if(LIBOPENCOR_JAVASCRIPT_UNIT_TESTING)
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E echo "-- JavaScript test Web page has been copied to: ${JAVASCRIPT_BINDINGS_DIR}/index.html"
                           COMMAND ${CMAKE_COMMAND} -E echo "                          and can be run using: emrun --browser chrome ${JAVASCRIPT_BINDINGS_DIR}/index.html")
    endif()
else()
    # If we are building a static version of libOpenCOR then we want to extract the object files from our various
    # packages and merge them into libOpenCOR. Indeed, a static library doesn't include its dependencies (it "only"
    # references them). So, we need to merge those dependencies into libOpenCOR and, sadly, there is no (easy?) way to
    # do this using CMake, so we must do it ourselves. Now, one (big!) downside of this approach is that it blows up the
    # size of libOpenCOR since it also merges parts of our dependencies that we don't actually need. So much so that we
    # cannot generate a debug version of libOpenCOR on Windows since it would result in a static library that is more
    # than 4GB in size (and MSVC's linker cannot create static libraries that big). This is clearly far from ideal, but
    # this is the only way to have a static version of libOpenCOR that can be used without having to "manually" link
    # against all of its dependencies.

    if(NOT LIBOPENCOR_SHARED_LIBS)
        # Build the static archive extractor that will be used to extract the object files from our various packages. It
        # handles duplicate object names (e.g., two object files named X86.cpp.o within the same library) by prepending
        # a unique index to each output filename, which ar -x cannot do since the second extraction silently overwrites
        # the first, losing symbols.

        set(STATIC_ARCHIVE_EXTRACTOR ${CMAKE_BINARY_DIR}/staticarchiveextractor)

        message(STATUS "Building staticarchiveextractor")

        try_compile(STATIC_ARCHIVE_EXTRACTOR_EXE ${CMAKE_BINARY_DIR}
                    SOURCES ${CMAKE_SOURCE_DIR}/cmake/staticarchiveextractor.cpp
                    COPY_FILE ${STATIC_ARCHIVE_EXTRACTOR})

        if(STATIC_ARCHIVE_EXTRACTOR_EXE)
            message(STATUS "Building staticarchiveextractor - Success")
        else()
            message(STATUS "Building staticarchiveextractor - Failed")
            message(FATAL_ERROR "staticarchiveextractor could not be built.")
        endif()

        # Retrieve the path to the libraries of the packages that we use.
        # Note: we only look for them in the directories of those packages since PREBUILT_DIR may contain other packages
        #       (e.g., one that has since been renamed), which libraries we must not merge into our static library.

        set(PACKAGES_DIR ${CMAKE_BINARY_DIR}/packages)
        set(PACKAGES)

        foreach(AVAILABLE_PACKAGE ${AVAILABLE_PACKAGES})
            file(GLOB_RECURSE AVAILABLE_PACKAGE_LIBRARIES ${PREBUILT_DIR}/${AVAILABLE_PACKAGE}/*${CMAKE_STATIC_LIBRARY_SUFFIX})

            list(APPEND PACKAGES ${AVAILABLE_PACKAGE_LIBRARIES})
        endforeach()

        # Extract the object files from our various packages, handling duplicate object names.

        list(LENGTH PACKAGES PACKAGE_COUNT)

        set(PACKAGE_INDEX 0)
        set(PACKAGES_OBJECT_FILES)

        foreach(PACKAGE ${PACKAGES})
            math(EXPR PACKAGE_INDEX "${PACKAGE_INDEX} + 1")
            get_filename_component(PACKAGE_NAME ${PACKAGE} NAME_WE)

            set(PACKAGE_DIR ${PACKAGES_DIR}/${PACKAGE_NAME})

            file(MAKE_DIRECTORY ${PACKAGE_DIR})

            message(STATUS "[${PACKAGE_INDEX}/${PACKAGE_COUNT}] Extracting object files from ${PACKAGE_NAME}")

            execute_process(COMMAND ${STATIC_ARCHIVE_EXTRACTOR} ${PACKAGE}
                            WORKING_DIRECTORY ${PACKAGE_DIR})

            message(STATUS "[${PACKAGE_INDEX}/${PACKAGE_COUNT}] Extracting object files from ${PACKAGE_NAME} - done")

            file(GLOB_RECURSE PACKAGE_OBJECT_FILES ${PACKAGE_DIR}/*${CMAKE_CXX_OUTPUT_EXTENSION})

            list(APPEND PACKAGES_OBJECT_FILES ${PACKAGE_OBJECT_FILES})
        endforeach()
    endif()

    add_library(${CMAKE_PROJECT_NAME}
                ${API_HEADER_FILES}
                ${SOURCE_FILES}
                ${HEADER_FILES}
                ${PACKAGES_OBJECT_FILES})

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
                          CXX_VISIBILITY_PRESET hidden
                          DEBUG_POSTFIX d
                          PREFIX ""
                          VISIBILITY_INLINES_HIDDEN ON)
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/api>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>)
target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module>
                           $<INSTALL_INTERFACE:include/>
                           $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME_LC}/module>)

# Configure libOpenCOR.

configure_target(${CMAKE_PROJECT_NAME})

# Generate the export header file, or don't use any if we are building our JavaScript bindings.

if(EMSCRIPTEN)
    configure_file(noexports.h ${API_EXPORT_HEADER_FILE} COPYONLY)
else()
    include(GenerateExportHeader)

    generate_export_header(${CMAKE_PROJECT_NAME}
                           EXPORT_FILE_NAME ${API_EXPORT_HEADER_FILE}
                           BASE_NAME LIBOPENCOR)
endif()

# Copy over our export header file for unit testing.

configure_file(unittestingexport.h unittestingexport.h COPYONLY)

# Don't allow unresolved symbols.
# Note: MSVC doesn't allow unresolved symbols, so we only need to look for them if we are building using GNU or Clang.

if(BUILDING_USING_GNU)
    add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,--no-undefined)
elseif(BUILDING_USING_CLANG)
    # On macOS, only use the -Wl,--no-undefined option if we are not using Xcode 15.0 or later since otherwise the
    # linker is going to generate some warnings about -undefined error being deprecated.

    if(APPLE)
        # Get the Xcode version.
        # Note: CMake has XCODE_VERSION, but it's only available when using the Xcode generator. So, we need to get the
        #       Xcode version ourselves. And, depending on how Xcode was installed, we may need to get the Xcode version
        #       using softwareupdate.

        execute_process(COMMAND xcodebuild -version
                        OUTPUT_VARIABLE XCODE_VERSION
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        RESULT_VARIABLE RESULT
                        ERROR_QUIET)

        if(RESULT EQUAL 0)
            string(REPLACE "Xcode " "" XCODE_VERSION ${XCODE_VERSION})
        else()
            execute_process(COMMAND softwareupdate --history
                            COMMAND awk "/Command Line Tools for Xcode/ {print \$6}"
                            COMMAND tail -1
                            OUTPUT_VARIABLE XCODE_VERSION
                            OUTPUT_STRIP_TRAILING_WHITESPACE
                            ERROR_QUIET)
        endif()
    else()
        set(XCODE_VERSION 0)
    endif()

    if(XCODE_VERSION VERSION_LESS 15.0)
        add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,-undefined,error)
    endif()
endif()

# On Linux, in release mode (unless we are doing code coverage), put each of our functions and data in its own section,
# so that the linker can remove the ones that we don't use, be it when linking our shared library or our Python module
# (which embeds our static library). Our third-party libraries are built the same way (see THIRD_PARTY_C_FLAGS in
# cmake/packages.cmake).

if(NOT WIN32 AND NOT APPLE AND NOT EMSCRIPTEN AND RELEASE_MODE AND NOT LIBOPENCOR_CODE_COVERAGE)
    add_compiler_flags(${CMAKE_PROJECT_NAME} -ffunction-sections -fdata-sections)
endif()

# Only export our own symbols from our shared library and, in release mode (unless we are doing code coverage, so as not
# to affect our code coverage reports), have the linker remove the code that we don't use and discard our local symbols.
# Note #1: our third-party libraries (e.g., LLVM) are static libraries which symbols have default visibility. So,
#          without this, our shared library would export all of them, which would prevent the linker from removing any
#          of their code that we don't use and, on Linux, would allow another copy of LLVM in the same process (e.g.,
#          one from another Python module) to interpose its symbols with ours.
# Note #2: on Windows, a DLL only exports the symbols that are explicitly marked as such and, in release mode, MSVC's
#          linker removes unreferenced code by default, so there is nothing for us to do.
# Note #3: on macOS, there is no equivalent of --exclude-libs, so we only export the symbols that have "libOpenCOR" in
#          their (mangled) name. This is fine since our own code is built with hidden visibility (see
#          CXX_VISIBILITY_PRESET above), so the only symbols of ours that could be exported are those that we export on
#          purpose and they are all in the libOpenCOR namespace.

if(LIBOPENCOR_SHARED_LIBS AND NOT EMSCRIPTEN)
    if(APPLE)
        set(EXPORTED_SYMBOLS_FILE ${CMAKE_CURRENT_BINARY_DIR}/exportedsymbols.txt)

        file(WRITE ${EXPORTED_SYMBOLS_FILE} "*libOpenCOR*\n")

        add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,-exported_symbols_list,${EXPORTED_SYMBOLS_FILE})

        if(RELEASE_MODE AND NOT LIBOPENCOR_CODE_COVERAGE)
            add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,-dead_strip -Wl,-x)
        endif()
    elseif(NOT WIN32)
        add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,--exclude-libs,ALL)

        if(RELEASE_MODE AND NOT LIBOPENCOR_CODE_COVERAGE)
            add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,--gc-sections -Wl,-x)
        endif()
    endif()
endif()

# Get ready for code coverage.

if(LIBOPENCOR_CODE_COVERAGE)
    add_compiler_flags(${CMAKE_PROJECT_NAME} ${CODE_COVERAGE_COMPILER_FLAGS})
    add_linker_flags(${CMAKE_PROJECT_NAME} ${CODE_COVERAGE_LINKER_FLAGS})
endif()

# Build our different bindings.

add_subdirectory(bindings)

list(APPEND GIT_SOURCE_FILES
    ${JAVASCRIPT_SOURCE_FILES}
    ${PYTHON_SOURCE_FILES}
)

# Install libOpenCOR itself.

if(APPLE)
    # On macOS, we need to specify the COMPONENT option otherwise our package will be empty.
    # Note: we don't want to specify the COMPONENT option on Windows since NSIS will otherwise make it a selectable
    #       option which we don't want.

    set(LIBOPENCOR_COMPONENT_OPTION COMPONENT Runtime)
endif()

install(TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        ${LIBOPENCOR_COMPONENT_OPTION}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES ${API_HEADER_FILES}
        DESTINATION include/${CMAKE_PROJECT_NAME_LC})

install(FILES ${API_MODULE_FILE}
        DESTINATION include/${CMAKE_PROJECT_NAME_LC}/module)

# Install libOpenCOR's CMake files.

include(CMakePackageConfigHelpers)

if(WIN32)
    set(CMAKE_PACKAGE_DIR cmake/${CMAKE_PROJECT_NAME})
else()
    set(CMAKE_PACKAGE_DIR lib/cmake/${CMAKE_PROJECT_NAME})
endif()

if(NOT LIBOPENCOR_SHARED_LIBS)
    # Patch the generated libOpenCORTargets.cmake file by removing the INTERFACE_LINK_LIBRARIES line. Indeed, it lists
    # all the third-party libraries used by libOpenCOR. However, the way we build the static version of libOpenCOR (so
    # that it embeds all of its dependencies) means that we don't need and, in fact, don't want to list them. (Don't
    # want because otherwise CMake might complain that it cannot find libOpenCOR's "dependencies".)
    # Note: rather than patching libOpenCORTargets.cmake, we could use
    #         set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES INTERFACE_LINK_LIBRARIES "")
    #       but this will fail our Python bindings. So, we patch libOpenCORTargets.cmake instead.

    set(PATCH_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/patch.cmake)

    configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/patch.in.cmake ${PATCH_CMAKE_FILE} @ONLY)

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -P ${PATCH_CMAKE_FILE})
endif()

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
        FILE ${CMAKE_PROJECT_NAME}Targets.cmake
        DESTINATION ${CMAKE_PACKAGE_DIR})

set(CONFIG_CMAKE_IN_FILE ${PROJECT_SOURCE_DIR}/cmake/packaging/config.in.cmake)
set(CONFIG_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake)

configure_package_config_file(${CONFIG_CMAKE_IN_FILE} ${CONFIG_CMAKE_FILE}
                              INSTALL_DESTINATION ${CMAKE_PACKAGE_DIR})

set(CONFIG_VERSION_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake)

write_basic_package_version_file(${CONFIG_VERSION_CMAKE_FILE}
                                 COMPATIBILITY AnyNewerVersion)

install(FILES ${CONFIG_CMAKE_FILE}
              ${CONFIG_VERSION_CMAKE_FILE}
        DESTINATION ${CMAKE_PACKAGE_DIR})

# Uninstall libOpenCOR.

set(UNINSTALL_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/uninstall.in.cmake ${UNINSTALL_CMAKE_FILE} @ONLY)

add_target(uninstall
           COMMAND ${CMAKE_COMMAND} -P ${UNINSTALL_CMAKE_FILE}
           COMMENT "Uninstall the project...")

# Share some file information with the rest of our building system.

set(GIT_API_HEADER_FILES ${GIT_API_HEADER_FILES} PARENT_SCOPE)
set(GIT_API_MODULE_FILE ${GIT_API_MODULE_FILE} PARENT_SCOPE)
set(GIT_SOURCE_FILES ${GIT_SOURCE_FILES} PARENT_SCOPE)
set(GIT_HEADER_FILES ${GIT_HEADER_FILES} PARENT_SCOPE)
