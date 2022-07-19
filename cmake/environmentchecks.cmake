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
       AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(BUILDING_USING_MSVC TRUE)
    else()
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using MSVC on Windows.")
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

# Make sure that we are building libOpenCOR for macOS 10.15 and later.

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET VERSION_GREATER_EQUAL 10.15)
    message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built for macOS 10.15 and later.")
endif()

# Check whether we are dealing with a single or multiple configuration.

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# Look for various packages and programs.
# Note: when it comes to finding Python, we try to use the registry after the
#       environment variables on Windows and to find frameworks after standard
#       libraries or headers on macOS. (On GitHub Actions, it helps finding the
#       correct version of Python on Windows.)

if(NOT DEFINED Python_FIND_REGISTRY)
    set(Python_FIND_REGISTRY "LAST")
endif()

if(NOT DEFINED Python_FIND_FRAMEWORK)
    set(Python_FIND_FRAMEWORK "LAST")
endif()

find_package(Doxygen)
find_package(Python ${PREFERRED_PYTHON_VERSION} COMPONENTS Interpreter Development)

if(NOT WIN32 AND NOT APPLE)
    # Some third-party libraries get built with threads support (and so do our
    # tests, though this can be disabled by setting GTEST_DISABLE_PTHREADS to
    # ON), so make sure that it's present since libOpenCOR will need to be
    # linked against pthreads as a result.

    find_package(Threads REQUIRED)
endif()

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

if(Python_EXECUTABLE)
    set(PYTHON_EXE ${Python_EXECUTABLE})
elseif(Python3_EXECUTABLE)
    set(PYTHON_EXE ${Python3_EXECUTABLE})
endif()

if(Python_LIBRARIES)
    set(PYTHON_LIBRARIES ${Python_LIBRARIES})
elseif(Python3_LIBRARY)
    set(PYTHON_LIBRARIES ${Python3_LIBRARY})
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

if(FIND_EXE AND GCOV_EXE AND GCOVR_EXE AND CODE_COVERAGE_GCOV_COMPILER_FLAGS_OK)
    set(CODE_COVERAGE_GCOV_AVAILABLE TRUE)
else()
    set(CODE_COVERAGE_GCOV_ERROR_MESSAGE "Code coverage testing using gcov is requested but the find, gcov and/or gcovr tools could not be found, and/or the --coverage compiler/linker flag is not supported.")
endif()

if(FIND_EXE AND LLVM_COV_EXE AND LLVM_PROFDATA_EXE AND CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS_OK)
    set(CODE_COVERAGE_LLVM_COV_AVAILABLE TRUE)
else()
    set(CODE_COVERAGE_LLVM_COV_ERROR_MESSAGE "Code coverage testing using llvm-cov is requested but the find, llvm-cov and/or llvm-profdata tools could not be found, and/or the -fprofile-instr-generate -fcoverage-mapping compiler/linker flags are not supported.")
endif()

if(DOXYGEN_EXE AND PATCH_EXE AND PYTHON_EXE AND SPHINX_EXE)
    set(DOXYGEN_MINIMUM_VERSION 1.9)

    execute_process(COMMAND ${DOXYGEN_EXE} --version
                    OUTPUT_VARIABLE DOXYGEN_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    if(DOXYGEN_VERSION VERSION_LESS DOXYGEN_MINIMUM_VERSION)
        set(DOCUMENTATION_AVAILABLE_ERROR_MESSAGE "Documentation is requested and Doxygen ${DOXYGEN_VERSION} was found, but version ${DOXYGEN_MINIMUM_VERSION}+ is needed.")
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

if(PYTHON_LIBRARIES)
    set(PYTHON_BINDINGS_AVAILABLE TRUE)
else()
    set(PYTHON_BINDINGS_ERROR_MESSAGE "Python bindings are requested but Python libraries could not be found.")
endif()

if(PYTHON_BINDINGS_AVAILABLE AND PYTEST_EXE)
    set(PYTHON_UNIT_TESTING_AVAILABLE TRUE)
else()
    set(PYTHON_UNIT_TESTING_ERROR_MESSAGE "Python unit testing is requested but Python libraries and/or the pytest tool could not be found.")
endif()

if(PYTHON_UNIT_TESTING_AVAILABLE AND PYTHON_EXE)
    check_python_package(pytest-html PYTHON_PYTEST_HTML_AVAILABLE)

    if(PYTHON_PYTEST_HTML_AVAILABLE)
        set(PYTHON_UNIT_TESTING_REPORT_AVAILABLE TRUE)
    endif()
endif()

if(PYTHON_EXE AND VALGRIND_EXE)
    set(MEMORY_CHECKS_AVAILABLE TRUE)
else()
    set(MEMORY_CHECKS_ERROR_MESSAGE "Memory checks are requested but Python and/or Valgrind could not be found.")
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
