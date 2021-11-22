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
    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using MSVC on Windows.")
    endif()
elseif(APPLE)
    if(    NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
       AND NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using (Apple) Clang on macOS.")
    endif()
else()
    if(    NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"
       AND NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using GCC/G++ on Linux.")
    endif()
endif()

# Make sure that we are building libOpenCOR in 64-bit mode.
# Note: normally, we would check the value of CMAKE_SIZEOF_VOID_P, but in some
#       cases it may not be set (e.g., when generating an Xcode project file),
#       so we determine and retrieve that value ourselves.

try_run(ARCHITECTURE_RUN ARCHITECTURE_COMPILE
        ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/architecture.c
        RUN_OUTPUT_VARIABLE ARCHITECTURE)

if(NOT ARCHITECTURE_COMPILE)
    message(FATAL_ERROR "We could not determine your architecture. Please clean your ${CMAKE_PROJECT_NAME} environment and try again.")
elseif(NOT ${ARCHITECTURE} EQUAL 64)
    message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built in 64-bit mode.")
endif()

# Check whether we are dealing with a single or multiple configuration.

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# Look for various packages and programs.

find_package(Doxygen)

find_program(BUILDCACHE_EXE buildcache)

if(NOT BUILDCACHE_EXE)
    if(MSVC)
        find_program(CLCACHE_EXE clcache)
    else()
        find_program(CCACHE_EXE ccache)
    endif()
endif()

find_program(BLACK_EXE NAMES ${PREFERRED_BLACK_NAMES} black)
find_program(CLANG_FORMAT_EXE NAMES ${PREFERRED_CLANG_FORMAT_NAMES} clang-format)
find_program(CLANG_TIDY_EXE NAMES ${PREFERRED_CLANG_TIDY_NAMES} clang-tidy)
find_program(FIND_EXE NAMES ${PREFERRED_FIND_NAMES} find)
find_program(GCOV_EXE NAMES ${PREFERRED_GCOV_NAMES} gcov)
find_program(GCOVR_EXE NAMES ${PREFERRED_GCOVR_NAMES} gcovr)
find_program(GIT_EXE NAMES ${PRFERRED_GIT_NAMES} git)
find_program(LLVM_COV_EXE NAMES ${PREFERRED_LLVM_COV_NAMES} llvm-cov)
find_program(LLVM_PROFDATA_EXE NAMES ${PREFERRED_LLVM_PROFDATA_NAMES} llvm-profdata)
find_program(PATCH_EXE NAMES ${PREFERRED_PATCH_NAMES} patch)
find_program(PYTEST_EXE NAMES ${PREFERRED_PYTEST_NAMES} pytest)
find_program(SPHINX_EXE NAMES ${PREFERRED_SPHINX_NAMES} sphinx-build sphinx-build2)
find_program(VALGRIND_EXE NAMES ${PREFERRED_VALGRIND_NAMES} valgrind)

# Create some aliases.

if(DOXYGEN_EXECUTABLE)
    set(DOXYGEN_EXE ${DOXYGEN_EXECUTABLE})
endif()

if(PYTHON_EXECUTABLE)
    set(PYTHON_EXE ${PYTHON_EXECUTABLE})
endif()

# Check some compiler flags.

include(CheckCXXCompilerFlag)

set(CODE_COVERAGE_GCOV_COMPILER_FLAGS "--coverage")
set(CODE_COVERAGE_GCOV_LINKER_FLAGS "${CODE_COVERAGE_GCOV_COMPILER_FLAGS}")
set(CMAKE_REQUIRED_FLAGS ${CODE_COVERAGE_GCOV_COMPILER_FLAGS})

check_cxx_compiler_flag(${CODE_COVERAGE_GCOV_COMPILER_FLAGS} CODE_COVERAGE_GCOV_COMPILER_FLAGS_OK)

set(CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
set(CODE_COVERAGE_LLVM_COV_LINKER_FLAGS "-fprofile-instr-generate")
set(CMAKE_REQUIRED_FLAGS ${CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS})

check_cxx_compiler_flag(${CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS} CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS_OK)

# Determine what is available.

if(BUILDCACHE_EXE OR CLCACHE_EXE OR CCACHE_EXE)
    set(COMPILER_CACHING_AVAILABLE TRUE CACHE INTERNAL "Compiler caching is available.")
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
        set(FORMAT_CODE_AVAILABLE TRUE CACHE INTERNAL "Format code is available.")

        if(GIT_EXE)
            set(CHECK_CODE_FORMATTING_AVAILABLE TRUE CACHE INTERNAL "Check code formatting is available.")
        endif()
    endif()
endif()

if(BLACK_EXE)
    set(PYTHON_FORMATTING_AVAILABLE TRUE CACHE INTERNAL "Python formatting is available.")
endif()

if(CLANG_TIDY_EXE)
    set(CLANG_TIDY_MINIMUM_VERSION 12)

    execute_process(COMMAND ${CLANG_TIDY_EXE} --version
                    OUTPUT_VARIABLE CLANG_TIDY_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    string(REGEX REPLACE "^.*LLVM version ([.0-9]+).*$" "\\1" CLANG_TIDY_VERSION "${CLANG_TIDY_VERSION}")

    if(CLANG_TIDY_VERSION VERSION_LESS CLANG_TIDY_MINIMUM_VERSION)
        message(WARNING "Clang-Tidy ${CLANG_TIDY_VERSION} was found, but version ${CLANG_TIDY_MINIMUM_VERSION}+ is needed to perform static analysis.")
    else()
        set(CODE_ANALYSIS_AVAILABLE TRUE CACHE INTERNAL "Code analysis is available.")
    endif()
endif()

if(FIND_EXE AND GCOV_EXE AND GCOVR_EXE AND CODE_COVERAGE_GCOV_COMPILER_FLAGS_OK)
    set(CODE_COVERAGE_GCOV_AVAILABLE TRUE CACHE INTERNAL "Code coverage using gcov is available.")
endif()

if(FIND_EXE AND LLVM_COV_EXE AND LLVM_PROFDATA_EXE AND CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS_OK)
    set(CODE_COVERAGE_LLVM_COV_AVAILABLE TRUE CACHE INTERNAL "Code coverage using llvm-cov is available.")
endif()

if(DOXYGEN_EXE AND PATCH_EXE AND PYTHON_EXE AND SPHINX_EXE)
    set(DOXYGEN_MINIMUM_VERSION 1.9)

    execute_process(COMMAND ${DOXYGEN_EXE} --version
                    OUTPUT_VARIABLE DOXYGEN_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    if(DOXYGEN_VERSION VERSION_LESS DOXYGEN_MINIMUM_VERSION)
        message(WARNING "ClangFormat ${DOXYGEN_VERSION} was found, but version ${DOXYGEN_MINIMUM_VERSION}+ is needed to generate the documentation.")
    else()
        check_python_package(sphinx-copybutton PYTHON_SPHINX_COPY_BUTTON_AVAILABLE)
        check_python_package(sphinx-inline-tabs PYTHON_SPHINX_INLINE_TABS_AVAILABLE)

        if(PYTHON_SPHINX_COPY_BUTTON_AVAILABLE AND PYTHON_SPHINX_INLINE_TABS_AVAILABLE)
            set(DOCUMENTATION_AVAILABLE TRUE CACHE INTERNAL "Documentation is available.")
        endif()
    endif()
endif()

if(PYTHON_LIBRARIES)
    set(PYTHON_BINDINGS_AVAILABLE TRUE CACHE INTERNAL "Python bindings are available.")
endif()

if(PYTHON_BINDINGS_AVAILABLE AND PYTEST_EXE)
    set(PYTHON_UNIT_TESTING_AVAILABLE TRUE CACHE INTERNAL "Python unit testing is available.")
endif()

if(PYTHON_EXE AND PYTHON_UNIT_TESTING_AVAILABLE)
    check_python_package(pytest-html PYTHON_PYTEST_HTML_AVAILABLE)

    if(PYTHON_PYTEST_HTML_AVAILABLE)
        set(PYTHON_UNIT_TESTING_REPORT_AVAILABLE TRUE CACHE INTERNAL "Python unit testing report is available.")
    endif()
endif()

if(PYTHON_EXE AND VALGRIND_EXE)
    set(MEMORY_CHECKS_AVAILABLE TRUE CACHE INTERNAL "Memory checks are available.")
endif()

# Hide the CMake options that are not directly relevant to libOpenCOR.

if(MSVC)
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
                 FIND_EXE
                 GCOV_EXE
                 GCOVR_EXE
                 GIT_EXE
                 LLVM_COV_EXE
                 LLVM_PROFDATA_EXE
                 PATCH_EXE
                 PYTEST_EXE
                 SPHINX_EXE
                 VALGRIND_EXE)
