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

if(WIN32)
    find_program(CLCACHE_EXE clcache)

    if(CLCACHE_EXE)
        set(CLCACHE_AVAILABLE TRUE CACHE INTERNAL "Executable required to cache compilations.")
    endif()
else()
    find_program(CCACHE_EXE ccache)

    if(CCACHE_EXE)
        set(CCACHE_AVAILABLE TRUE CACHE INTERNAL "Executable required to cache compilations.")
    endif()
endif()

find_program(CLANG_FORMAT_EXE NAMES ${PREFERRED_CLANG_FORMAT_NAMES} clang-format)
find_program(CLANG_TIDY_EXE NAMES ${PREFERRED_CLANG_TIDY_NAMES} clang-tidy)
find_program(FIND_EXE NAMES ${PREFERRED_FIND_NAMES} find)
find_program(GCOV_EXE NAMES ${PREFERRED_GCOV_NAMES} gcov)
find_program(GCOVR_EXE NAMES ${PREFERRED_GCOVR_NAMES} gcovr)
find_program(GIT_EXE NAMES ${PRFERRED_GIT_NAMES} git)
find_program(LLVM_COV_EXE NAMES ${PREFERRED_LLVM_COV_NAMES} llvm-cov)
find_program(LLVM_PROFDATA_EXE NAMES ${PREFERRED_LLVM_PROFDATA_NAMES} llvm-profdata)
find_program(PYTEST_EXE NAMES ${PREFERRED_PYTEST_NAMES} pytest)
find_program(VALGRIND_EXE NAMES ${PREFERRED_VALGRIND_NAMES} valgrind)

# Check some compiler flags

include(CheckCXXCompilerFlag)

set(COVERAGE_GCOV_COMPILER_FLAGS "--coverage")
set(COVERAGE_GCOV_LINKER_FLAGS "${COVERAGE_GCOV_COMPILER_FLAGS}")
set(CMAKE_REQUIRED_FLAGS ${COVERAGE_GCOV_COMPILER_FLAGS})

check_cxx_compiler_flag(${COVERAGE_GCOV_COMPILER_FLAGS} COVERAGE_GCOV_COMPILER_FLAGS_OK)

set(COVERAGE_LLVM_COV_COMPILER_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
set(COVERAGE_LLVM_COV_LINKER_FLAGS "-fprofile-instr-generate")
set(CMAKE_REQUIRED_FLAGS ${COVERAGE_LLVM_COV_COMPILER_FLAGS})

check_cxx_compiler_flag(${COVERAGE_LLVM_COV_COMPILER_FLAGS} COVERAGE_LLVM_COV_COMPILER_FLAGS_OK)

# Determine what is available.

if(CLANG_FORMAT_EXE)
    set(CLANG_FORMAT_MINIMUM_VERSION 12)

    execute_process(COMMAND ${CLANG_FORMAT_EXE} --version
                    OUTPUT_VARIABLE CLANG_FORMAT_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    string(REGEX REPLACE "^.*clang-format version ([.0-9]+).*$" "\\1" CLANG_FORMAT_VERSION "${CLANG_FORMAT_VERSION}")

    if(CLANG_FORMAT_VERSION VERSION_LESS CLANG_FORMAT_MINIMUM_VERSION)
        message(WARNING "ClangFormat ${CLANG_FORMAT_VERSION} was found, but version ${CLANG_FORMAT_MINIMUM_VERSION}+ is needed to format the code and/or run the ClangFormat test.")
    else()
        set(CLANG_FORMAT_AVAILABLE TRUE CACHE INTERNAL "Executable required to format the code.")

        if(GIT_EXE)
            set(CLANG_FORMAT_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to run the ClangFormat test.")
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

if(FIND_EXE AND GCOV_EXE AND GCOVR_EXE AND COVERAGE_GCOV_COMPILER_FLAGS_OK)
    set(COVERAGE_GCOV_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to run coverage testing using gcov.")
endif()

if(FIND_EXE AND LLVM_COV_EXE AND LLVM_PROFDATA_EXE AND COVERAGE_LLVM_COV_COMPILER_FLAGS_OK)
    set(COVERAGE_LLVM_COV_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to run coverage testing using llvm-cov.")
endif()

if(PYTEST_EXE)
    set(PYTHON_UNIT_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executable and package required to run Python unit testing.")
endif()

if(VALGRIND_EXE)
    set(VALGRIND_AVAILABLE TRUE CACHE INTERNAL "Executable required to run memory checks.")
endif()

# Hide the CMake options that are not directly relevant to libOpenCOR.

if(WIN32)
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
    CLANG_FORMAT_EXE
    CLANG_TIDY_EXE
    FIND_EXE
    GCOV_EXE
    GCOVR_EXE
    GIT_EXE
    LLVM_COV_EXE
    LLVM_PROFDATA_EXE
    PYTEST_EXE
    VALGRIND_EXE
)
