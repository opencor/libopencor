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

# Look for various packages and programs.

find_package(Python ${PREFERRED_PYTHON_VERSION} COMPONENTS Interpreter Development)

find_program(BUILDCACHE_EXE buildcache)

if(NOT BUILDCACHE_EXE)
    if(MSVC)
        find_program(CLCACHE_EXE clcache)
    else()
        find_program(CCACHE_EXE ccache)
    endif()
endif()

find_program(CLANG_FORMAT_EXE NAMES ${PREFERRED_CLANG_FORMAT_NAMES} clang-format
             PATHS /usr/local/opt/llvm/bin)
find_program(CLANG_TIDY_EXE NAMES ${PREFERRED_CLANG_TIDY_NAMES} clang-tidy
             PATHS /usr/local/opt/llvm/bin)
find_program(EMCMAKE_EXE NAMES ${PREFERRED_EMCMAKE_NAMES} emcmake)
find_program(FIND_EXE NAMES ${PREFERRED_FIND_NAMES} find)
find_program(GCOV_EXE NAMES ${PREFERRED_GCOV_NAMES} gcov)
find_program(GCOVR_EXE NAMES ${PREFERRED_GCOVR_NAMES} gcovr)
find_program(GIT_EXE NAMES ${PRFERRED_GIT_NAMES} git)
find_program(LLVM_COV_EXE NAMES ${PREFERRED_LLVM_COV_NAMES} llvm-cov
             PATHS /usr/local/opt/llvm/bin)
find_program(LLVM_PROFDATA_EXE NAMES ${PREFERRED_LLVM_PROFDATA_NAMES} llvm-profdata
             PATHS /usr/local/opt/llvm/bin)
find_program(NODE_EXE NAMES ${PREFERRED_NODE_NAMES} node)
find_program(NPM_EXE NAMES ${PREFERRED_NPM_NAMES} npm)
find_program(PYTEST_EXE NAMES ${PREFERRED_PYTEST_NAMES} pytest)
find_program(VALGRIND_EXE NAMES ${PREFERRED_VALGRIND_NAMES} valgrind)

# Check some compiler flags

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
    set(COMPILER_CACHING_AVAILABLE TRUE CACHE INTERNAL "Executable required to cache compilations.")
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
        set(CLANG_FORMAT_AVAILABLE TRUE CACHE INTERNAL "Executable required to format the code.")

        if(GIT_EXE)
            set(CLANG_FORMAT_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to check code formatting.")
        endif()
    endif()
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
        set(CLANG_TIDY_AVAILABLE TRUE CACHE INTERNAL "Executable required to perform static analysis.")
    endif()
endif()

if(FIND_EXE AND GCOV_EXE AND GCOVR_EXE AND CODE_COVERAGE_GCOV_COMPILER_FLAGS_OK)
    set(CODE_COVERAGE_GCOV_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to run code coverage testing using gcov.")
endif()

if(FIND_EXE AND LLVM_COV_EXE AND LLVM_PROFDATA_EXE AND CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS_OK)
    set(CODE_COVERAGE_LLVM_COV_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to run code coverage testing using llvm-cov.")
endif()

if(EMCMAKE_EXE)
    set(JAVASCRIPT_BINDINGS_AVAILABLE TRUE CACHE INTERNAL "Executable required to build JavaScript bindings.")
endif()

if(JAVASCRIPT_BINDINGS_AVAILABLE AND NODE_EXE AND NPM_EXE)
    set(JAVASCRIPT_UNIT_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to run JavaScript unit testing.")
endif()

if(Python_Development_FOUND)
    set(PYTHON_BINDINGS_AVAILABLE TRUE CACHE INTERNAL "Executable required to build Python bindings.")
endif()

if(PYTHON_BINDINGS_AVAILABLE AND PYTEST_EXE)
    set(PYTHON_UNIT_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executable required to run Python unit testing.")
endif()

if(Python_EXECUTABLE AND PYTHON_UNIT_TESTING_AVAILABLE)
    message(STATUS "Performing Test HAS_PYTHON_PYTEST_HTML")

    execute_process(COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_python_packages.py pytest-html
                    RESULT_VARIABLE RESULT
                    OUTPUT_QUIET ERROR_QUIET)

    if(RESULT EQUAL 0)
        set(PYTHON_UNIT_TESTING_REPORT_AVAILABLE TRUE CACHE INTERNAL "Executable and package required to run Python unit testing.")

        message(STATUS "Performing Test HAS_PYTHON_PYTEST_HTML - Success")
    else()
        message(STATUS "Performing Test HAS_PYTHON_PYTEST_HTML - Failed")
    endif()
endif()

if(VALGRIND_EXE)
    set(VALGRIND_AVAILABLE TRUE CACHE INTERNAL "Executable required to run memory checks.")
endif()

# Hide the CMake options that are not directly relevant to libOpenCOR.

if(MSVC)
    mark_as_advanced(
        CLCACHE_EXE
        CMAKE_CONFIGURATION_TYPES
    )
else()
    mark_as_advanced(CCACHE_EXE)

    if(APPLE)
        mark_as_advanced(
            CMAKE_OSX_ARCHITECTURES
            CMAKE_OSX_DEPLOYMENT_TARGET
            CMAKE_OSX_SYSROOT
        )
    endif()
endif()

mark_as_advanced(
    BUILDCACHE_EXE
    CLANG_FORMAT_EXE
    CLANG_TIDY_EXE
    EMCMAKE_EXE
    FIND_EXE
    GCOV_EXE
    GCOVR_EXE
    GIT_EXE
    LLVM_COV_EXE
    LLVM_PROFDATA_EXE
    NODE_EXE
    NPM_EXE
    PYTEST_EXE
    VALGRIND_EXE
)
