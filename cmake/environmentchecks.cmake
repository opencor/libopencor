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

# Make sure that we are using a supported compiler.

if(WIN32)
    if(    "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC"
       AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC"
       AND MSVC_TOOLSET_VERSION EQUAL 143)
        set(BUILDING_USING_MSVC TRUE)
    elseif(    "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"
           AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(BUILDING_USING_CLANG TRUE)
    else()
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using MSVC 2022 or Clang on Windows.")
    endif()
elseif(APPLE)
    if(   (    "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"
           AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       OR (    "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang"
           AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang"))
        set(BUILDING_USING_CLANG TRUE)
    else()
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using (Apple) Clang on macOS.")
    endif()
else()
    if(    "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"
       AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(BUILDING_USING_GNU TRUE)
    elseif(    "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"
           AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(BUILDING_USING_CLANG TRUE)
    else()
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using GNU or Clang on Linux.")
    endif()
endif()

# Make sure that we are building libOpenCOR in 64-bit mode.

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built in 64-bit mode.")
endif()

# Make sure that we are building libOpenCOR for the version of macOS that we are after.

if(APPLE AND NOT "${CMAKE_OSX_DEPLOYMENT_TARGET}" VERSION_GREATER_EQUAL "${MACOS_DEPLOYMENT_TARGET}")
    message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built for macOS ${MACOS_DEPLOYMENT_TARGET} and later.")
endif()

# Determine our default target architecture.

message(FATAL_ERROR "CMAKE_HOST_SYSTEM_PROCESSOR: >${CMAKE_HOST_SYSTEM_PROCESSOR}<")

if("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "AMD64"
   OR "${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
    set(DEFAULT_TARGET_ARCHITECTURE Intel)

    if(WIN32)
        set(LLVMCLANG_HOST_TRIPLE x86_64-pc-windows-msvc)
    elseif(APPLE)
        set(LLVMCLANG_HOST_TRIPLE x86_64-apple-darwin)
    else()
        set(LLVMCLANG_HOST_TRIPLE x86_64-unknown-linux-gnu)
    endif()
elseif("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "ARM64"
       OR "${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "arm64")
    set(DEFAULT_TARGET_ARCHITECTURE ARM)

    if(WIN32)
        set(LLVMCLANG_HOST_TRIPLE aarch64-pc-windows-msvc)
    elseif(APPLE)
        set(LLVMCLANG_HOST_TRIPLE aarch64-apple-darwin)
    else()
        set(LLVMCLANG_HOST_TRIPLE aarch64-unknown-linux-gnu)
    endif()
endif()

if("${DEFAULT_TARGET_ARCHITECTURE}" STREQUAL "")
    message(FATAL_ERROR "No supported target architecture could be determined for ${CMAKE_PROJECT_NAME}.")
endif()

# Check whether we are dealing with a single or multiple configuration.

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# Look for Doxygen.

find_package(Doxygen)

# Look for Python.

if (CMAKE_VERSION VERSION_LESS 3.18)
    set(NB_PYTHON_DEV_MODULE Development)
else()
    set(NB_PYTHON_DEV_MODULE Development.Module)
endif()

find_package(Python 3.9
             COMPONENTS Interpreter ${NB_PYTHON_DEV_MODULE}
             OPTIONAL_COMPONENTS Development.SABIModule)

add_subdirectory(extern/nanobind)

mark_as_advanced(NB_CREATE_INSTALL_RULES
                 NB_TEST
                 NB_TEST_SANITZE
                 NB_TEST_SHARED_BUILD
                 NB_TEST_STABLE_ABI
                 nanobind_DIR)

# Look for various programs.

find_program(BUILDCACHE_EXE buildcache)

if(NOT BUILDCACHE_EXE)
    if(BUILDING_USING_MSVC)
        find_program(CLCACHE_EXE clcache)
    else()
        find_program(CCACHE_EXE ccache)
    endif()
endif()

find_program(BLACK_EXE NAMES ${PREFERRED_BLACK_NAMES} black)
find_program(CLANG_FORMAT_EXE NAMES ${PREFERRED_CLANG_FORMAT_NAMES} clang-format)
find_program(CLANG_TIDY_EXE NAMES ${PREFERRED_CLANG_TIDY_NAMES} clang-tidy)
find_program(EMCC_EXE NAMES ${PREFERRED_EMCC_NAMES} emcc)
find_program(EMCMAKE_EXE NAMES ${PREFERRED_EMCMAKE_NAMES} emcmake)
find_program(EMCONFIGURE_EXE NAMES ${PREFERRED_EMCONFIGURE_NAMES} emconfigure)
find_program(FIND_EXE NAMES ${PREFERRED_FIND_NAMES} find)
find_program(GIT_EXE NAMES ${PRFERRED_GIT_NAMES} git)
find_program(LLVM_COV_EXE NAMES ${PREFERRED_LLVM_COV_NAMES} llvm-cov)
find_program(LLVM_PROFDATA_EXE NAMES ${PREFERRED_LLVM_PROFDATA_NAMES} llvm-profdata)
find_program(NODE_EXE NAMES ${PREFERRED_NODE_NAMES} node)
find_program(NPM_EXE NAMES ${PREFERRED_NPM_NAMES} npm)
find_program(PATCH_EXE NAMES ${PREFERRED_PATCH_NAMES} patch)
find_program(PRETTIER_EXE NAMES ${PREFERRED_PRETTIER_NAMES} prettier)
find_program(PYTEST_EXE NAMES ${PREFERRED_PYTEST_NAMES} pytest)
find_program(SPHINX_EXE NAMES ${PREFERRED_SPHINX_NAMES} sphinx-build sphinx-build2)

if(APPLE)
    find_program(LEAKS_EXE NAMES ${PREFERRED_LEAKS_NAMES} leaks)
elseif(NOT WIN32)
    find_program(VALGRIND_EXE NAMES ${PREFERRED_VALGRIND_NAMES} valgrind)
endif()

# Create some aliases.

if(DOXYGEN_EXECUTABLE)
    set(DOXYGEN_EXE ${DOXYGEN_EXECUTABLE})
endif()

if(Python_EXECUTABLE)
    set(PYTHON_EXE ${Python_EXECUTABLE})
endif()

# Check some compiler flags.

include(CheckCXXCompilerFlag)

set(ORIG_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})

set(CODE_COVERAGE_COMPILER_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
set(CODE_COVERAGE_LINKER_FLAGS "-fprofile-instr-generate")
set(CMAKE_REQUIRED_FLAGS ${CODE_COVERAGE_COMPILER_FLAGS})

check_cxx_compiler_flag(${CODE_COVERAGE_COMPILER_FLAGS} CODE_COVERAGE_COMPILER_FLAGS_OK)

set(CMAKE_REQUIRED_FLAGS ${ORIG_CMAKE_REQUIRED_FLAGS})

# Determine what is available.

if(BUILDCACHE_EXE OR CLCACHE_EXE OR CCACHE_EXE)
    set(COMPILER_CACHING_AVAILABLE TRUE)
else()
    if(BUILDING_USING_MSVC)
        set(COMPILER_CACHING_ERROR_MESSAGE "Compiler caching is requested but neither buildcache nor clcache could be found.")
    else()
        set(COMPILER_CACHING_ERROR_MESSAGE "Compiler caching is requested but neither buildcache nor ccache could be found.")
    endif()
endif()

if(CLANG_FORMAT_EXE)
    set(CLANG_FORMAT_MINIMUM_VERSION 12)

    execute_process(COMMAND ${CLANG_FORMAT_EXE} --version
                    OUTPUT_VARIABLE CLANG_FORMAT_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    string(REGEX REPLACE "^.*clang-format version ([.0-9]+).*$" "\\1" CLANG_FORMAT_VERSION "${CLANG_FORMAT_VERSION}")

    if(CLANG_FORMAT_VERSION VERSION_LESS CLANG_FORMAT_MINIMUM_VERSION)
        message(WARNING "ClangFormat ${CLANG_FORMAT_VERSION} was found, but version ${CLANG_FORMAT_MINIMUM_VERSION}+ is needed to check code formatting and/or format the code.")
    else()
        set(FORMAT_CODE_AVAILABLE TRUE)

        if(GIT_EXE)
            set(CHECK_CODE_FORMATTING_AVAILABLE TRUE)
        endif()
    endif()
endif()

if(BLACK_EXE)
    set(PYTHON_FORMATTING_AVAILABLE TRUE)
endif()

if(CLANG_TIDY_EXE)
    set(CLANG_TIDY_MINIMUM_VERSION 12)

    execute_process(COMMAND ${CLANG_TIDY_EXE} --version
                    OUTPUT_VARIABLE CLANG_TIDY_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    string(REGEX REPLACE "^.*LLVM version ([.0-9]+).*$" "\\1" CLANG_TIDY_VERSION "${CLANG_TIDY_VERSION}")

    if(CLANG_TIDY_VERSION VERSION_LESS CLANG_TIDY_MINIMUM_VERSION)
        set(CODE_ANALYSIS_ERROR_MESSAGE "Code analysis is requested and Clang-Tidy ${CLANG_TIDY_VERSION} was found, but version ${CLANG_TIDY_MINIMUM_VERSION}+ is needed.")
    else()
        set(CODE_ANALYSIS_AVAILABLE TRUE)
    endif()
else()
    set(CODE_ANALYSIS_ERROR_MESSAGE "Code analysis is requested but Clang-Tidy could not be found.")
endif()

if(EMCC_EXE AND EMCMAKE_EXE AND EMCONFIGURE_EXE)
    set(FORBIDDEN_EMCC_VERSION 3.1.48)

    execute_process(COMMAND ${EMCC_EXE} --version
                    OUTPUT_VARIABLE EMCC_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    string(REGEX REPLACE "^emcc .* ([0-9]+\.[0-9]+\.[0-9]+).*" "\\1" EMCC_VERSION "${EMCC_VERSION}")

    if(EMCC_VERSION VERSION_EQUAL FORBIDDEN_EMCC_VERSION)
        set(JAVASCRIPT_BINDINGS_ERROR_MESSAGE "JavaScript bindings are requested and Emscripten ${EMCC_VERSION} was found, but it is known to cause problems. Please use another version of Emscripten.")
    else()
        set(JAVASCRIPT_BINDINGS_AVAILABLE TRUE)
    endif()
else()
    set(JAVASCRIPT_BINDINGS_ERROR_MESSAGE "JavaScript bindings are requested but the emcc, emcmake and/or emconfigure tools could not be found.")
endif()

if(JAVASCRIPT_BINDINGS_AVAILABLE AND NODE_EXE AND NPM_EXE)
    set(JAVASCRIPT_UNIT_TESTING_AVAILABLE TRUE)
else()
    set(JAVASCRIPT_UNIT_TESTING_ERROR_MESSAGE "JavaScript unit testing is requested but the node and/or npm tools could not be found.")
endif()

if(FIND_EXE AND LLVM_COV_EXE AND LLVM_PROFDATA_EXE AND CODE_COVERAGE_COMPILER_FLAGS_OK)
    set(CODE_COVERAGE_AVAILABLE TRUE)
else()
    set(CODE_COVERAGE_ERROR_MESSAGE "Code coverage testing is requested but the find, llvm-cov and/or llvm-profdata tools could not be found, and/or the -fprofile-instr-generate and -fcoverage-mapping compiler/linker flags are not supported.")
endif()

if(DOXYGEN_EXE AND PATCH_EXE AND PYTHON_EXE AND SPHINX_EXE)
    set(EXPECTED_DOXYGEN_VERSION 1.9.3)

    if(NOT DOXYGEN_VERSION VERSION_EQUAL EXPECTED_DOXYGEN_VERSION)
        set(DOCUMENTATION_AVAILABLE_ERROR_MESSAGE "Documentation is requested and Doxygen ${DOXYGEN_VERSION} was found, but version ${EXPECTED_DOXYGEN_VERSION} is needed.")
    else()
        check_python_package(sphinx-copybutton PYTHON_SPHINX_COPY_BUTTON_AVAILABLE)
        check_python_package(sphinx-inline-tabs PYTHON_SPHINX_INLINE_TABS_AVAILABLE)

        if(PYTHON_SPHINX_COPY_BUTTON_AVAILABLE AND PYTHON_SPHINX_INLINE_TABS_AVAILABLE)
            set(DOCUMENTATION_AVAILABLE TRUE)
        else()
            set(DOCUMENTATION_AVAILABLE_ERROR_MESSAGE "Documentation is requested but the sphinx-copybutton and/or sphinx-inline-tabs Python packages could not be found.")
        endif()
    endif()
else()
    set(DOCUMENTATION_AVAILABLE_ERROR_MESSAGE "Documentation is requested but Doxygen, the patch tool, Python and/or Sphinx could not be found.")
endif()

if(PRETTIER_EXE)
    set(JAVASCRIPT_FORMATTING_AVAILABLE TRUE)
endif()

if(TARGET Python::Module AND TARGET Python::Interpreter)
    set(PYTHON_BINDINGS_AVAILABLE TRUE)
else()
    set(PYTHON_BINDINGS_ERROR_MESSAGE "Python bindings are requested but Python libraries could not be found.")
endif()

if(PYTHON_BINDINGS_AVAILABLE AND PYTEST_EXE)
    set(PYTHON_UNIT_TESTING_AVAILABLE TRUE)
else()
    set(PYTHON_UNIT_TESTING_ERROR_MESSAGE "Python unit testing is requested but the pytest tool could not be found.")
endif()

if(PYTHON_UNIT_TESTING_AVAILABLE AND PYTHON_EXE)
    check_python_package(pytest-html PYTHON_PYTEST_HTML_AVAILABLE)

    if(PYTHON_PYTEST_HTML_AVAILABLE)
        set(PYTHON_UNIT_TESTING_REPORT_AVAILABLE TRUE)
    endif()
endif()

if(   (APPLE AND PYTHON_EXE AND LEAKS_EXE)
   OR (NOT APPLE AND NOT WIN32 AND PYTHON_EXE AND VALGRIND_EXE))
    set(MEMORY_CHECKS_AVAILABLE TRUE)
else()
    if(APPLE)
        set(MEMORY_CHECKS_ERROR_MESSAGE "Memory checks are requested but Python and/or the leaks tool could not be found.")
    else()
        set(MEMORY_CHECKS_ERROR_MESSAGE "Memory checks are requested but Python and/or Valgrind could not be found.")
    endif()
endif()

# Hide the CMake options that are not directly relevant to libOpenCOR.

if(BUILDING_USING_MSVC)
    mark_as_advanced(CLCACHE_EXE
                     CMAKE_CONFIGURATION_TYPES)
else()
    mark_as_advanced(CCACHE_EXE)

    if(APPLE)
        mark_as_advanced(CMAKE_OSX_ARCHITECTURES
                         CMAKE_OSX_DEPLOYMENT_TARGET
                         CMAKE_OSX_SYSROOT)
    endif()
endif()

mark_as_advanced(BLACK_EXE
                 BUILDCACHE_EXE
                 CLANG_FORMAT_EXE
                 CLANG_TIDY_EXE
                 EMCC_EXE
                 EMCMAKE_EXE
                 EMCONFIGURE_EXE
                 FIND_EXE
                 GIT_EXE
                 LEAKS_EXE
                 LLVM_COV_EXE
                 LLVM_PROFDATA_EXE
                 NODE_EXE
                 NPM_EXE
                 PATCH_EXE
                 PRETTIER_EXE
                 PYTEST_EXE
                 SPHINX_EXE
                 VALGRIND_EXE)
