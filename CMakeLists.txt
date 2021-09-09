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

cmake_minimum_required(VERSION 3.15)

project(libOpenCOR
    VERSION 0.1.0
)

# Enable C++11 and later.

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check the environment we are dealing with.

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(common)
include(environmentchecks)

# Rebrand the CMake options that are relevant to libOpenCOR.

# BUILD_SHARED_LIBS ==> LIBOPENCOR_BUILD_SHARED_LIBS.

set(DOCSTRING "Build shared libraries.")

if(DEFINED BUILD_SHARED_LIBS)
    set(LIBOPENCOR_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE BOOL "${DOCSTRING}" FORCE)
else()
    set(LIBOPENCOR_BUILD_SHARED_LIBS ON CACHE BOOL "${DOCSTRING}")
endif()

set(BUILD_SHARED_LIBS ${LIBOPENCOR_BUILD_SHARED_LIBS} CACHE INTERNAL "Internalise BUILD_SHARED_LIBS, manipulate via LIBOPENCOR_BUILD_SHARED_LIBS." FORCE)

# [CMAKE_]BUILD_TYPE ==> LIBOPENCOR_BUILD_TYPE.

set(DOCSTRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ...")

if(DEFINED BUILD_TYPE)
    set(LIBOPENCOR_BUILD_TYPE "${BUILD_TYPE}" CACHE STRING "${DOCSTRING}" FORCE)
else()
    set(LIBOPENCOR_BUILD_TYPE "Debug" CACHE STRING "${DOCSTRING}")
endif()

set(CMAKE_BUILD_TYPE ${LIBOPENCOR_BUILD_TYPE} CACHE INTERNAL "Internalise CMAKE_BUILD_TYPE, manipulate via LIBOPENCOR_BUILD_TYPE." FORCE)

# CODE_ANALYSIS ==> LIBOPENCOR_CODE_ANALYSIS.

if(CLANG_TIDY_AVAILABLE)
    set(DOCSTRING "Enable code analysis.")
    set(LIBOPENCOR_CODE_ANALYSIS OFF CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED CODE_ANALYSIS AND CLANG_TIDY_AVAILABLE)
    set(LIBOPENCOR_CODE_ANALYSIS "${CODE_ANALYSIS}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(CODE_ANALYSIS)
    message(SEND_ERROR "Code analysis is requested but clang-tidy could not be found.")
endif()

unset(CODE_ANALYSIS CACHE)

# COMPILER_CACHING ==> LIBOPENCOR_COMPILER_CACHING.

if(COMPILER_CACHING_AVAILABLE)
    set(DOCSTRING "Enable compiler caching.")
    set(LIBOPENCOR_COMPILER_CACHING ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED COMPILER_CACHING AND COMPILER_CACHING_AVAILABLE)
    set(LIBOPENCOR_COMPILER_CACHING ${COMPILER_CACHING} CACHE BOOL "${DOCSTRING}" FORCE)
elseif(COMPILER_CACHING)
    if(MSVC)
        set(CLCACHE ", clcache")
    endif()

    message(SEND_ERROR "Compiler caching is requested but buildcache${CLCACHE} or ccache could not be found.")
endif()

unset(COMPILER_CACHING CACHE)

# CODE_COVERAGE_GCOV ==> LIBOPENCOR_CODE_COVERAGE_GCOV.

if(CODE_COVERAGE_GCOV_TESTING_AVAILABLE)
    set(DOCSTRING "Enable code coverage testing using gcov.")
    set(LIBOPENCOR_CODE_COVERAGE_GCOV ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED CODE_COVERAGE_GCOV AND CODE_COVERAGE_GCOV_TESTING_AVAILABLE)
    set(LIBOPENCOR_CODE_COVERAGE_GCOV "${CODE_COVERAGE_GCOV}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(CODE_COVERAGE_GCOV)
    message(SEND_ERROR "Code coverage testing using gcov is requested but gcov or its requirements could not be found.")
endif()

unset(CODE_COVERAGE_GCOV CACHE)

# CODE_COVERAGE_LLVM_COV ==> LIBOPENCOR_CODE_COVERAGE_LLVM_COV.

if(CODE_COVERAGE_LLVM_COV_TESTING_AVAILABLE)
    set(DOCSTRING "Enable code coverage testing using llvm-cov.")
    set(LIBOPENCOR_CODE_COVERAGE_LLVM_COV ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED CODE_COVERAGE_LLVM_COV AND CODE_COVERAGE_LLVM_COV_TESTING_AVAILABLE)
    set(LIBOPENCOR_CODE_COVERAGE_LLVM_COV "${CODE_COVERAGE_LLVM_COV}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(CODE_COVERAGE_LLVM_COV)
    message(SEND_ERROR "Code coverage testing using llvm-cov is requested but llvm-cov or its requirements could not be found.")
endif()

unset(CODE_COVERAGE_LLVM_COV CACHE)

# [CMAKE_]INSTALL_PREFIX ==> LIBOPENCOR_INSTALL_PREFIX.

set(DOCSTRING "Install path prefix, prepended onto install directories.")

if(DEFINED INSTALL_PREFIX)
    set(LIBOPENCOR_INSTALL_PREFIX ${INSTALL_PREFIX} CACHE PATH "${DOCSTRING}" FORCE)
else()
    set(LIBOPENCOR_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "${DOCSTRING}")
endif()

file(TO_CMAKE_PATH ${LIBOPENCOR_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_PATH)

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX_PATH} CACHE INTERNAL "Internalise CMAKE_INSTALL_PREFIX, manipulate via LIBOPENCOR_INSTALL_PREFIX." FORCE)

# MEMORY_CHECKS ==> LIBOPENCOR_MEMORY_CHECKS.

if(VALGRIND_AVAILABLE)
    set(DOCSTRING "Enable memory checks.")
    set(LIBOPENCOR_MEMORY_CHECKS ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED MEMORY_CHECKS AND VALGRIND_AVAILABLE)
    set(LIBOPENCOR_MEMORY_CHECKS "${MEMORY_CHECKS}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(MEMORY_CHECKS)
    message(SEND_ERROR "Memory checks are requested but Valgrind could not be found.")
endif()

unset(MEMORY_CHECKS CACHE)

# PYTHON_BINDINGS ==> LIBOPENCOR_PYTHON_BINDINGS.

if(Python_Development_FOUND)
    set(DOCSTRING "Buid Python wrappers.")
    set(LIBOPENCOR_PYTHON_BINDINGS ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED PYTHON_BINDINGS AND Python_Development_FOUND)
    set(LIBOPENCOR_PYTHON_BINDINGS ${PYTHON_BINDINGS} CACHE BOOL "${DOCSTRING}" FORCE)
elseif(PYTHON_BINDINGS)
    message(SEND_ERROR "Python bindings are requested but Python development files could not be found.")
endif()

unset(PYTHON_BINDINGS CACHE)

# PYTHON_UNIT_TESTING ==> LIBOPENCOR_PYTHON_UNIT_TESTING.

if(PYTHON_UNIT_TESTING_AVAILABLE)
    set(DOCSTRING "Enable Python unit testing.")
    set(LIBOPENCOR_PYTHON_UNIT_TESTING ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED PYTHON_UNIT_TESTING AND PYTHON_UNIT_TESTING_AVAILABLE)
    set(LIBOPENCOR_PYTHON_UNIT_TESTING "${PYTHON_UNIT_TESTING}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(PYTHON_UNIT_TESTING)
    message(SEND_ERROR "Python unit testing is requested but Python and/or the Python pytest package could not be found.")
endif()

unset(PYTHON_UNIT_TESTING CACHE)

# UNIT_TESTING ==> LIBOPENCOR_UNIT_TESTING.

set(DOCSTRING "Enable unit testing.")
set(LIBOPENCOR_UNIT_TESTING ON CACHE BOOL "${DOCSTRING}")

if(DEFINED UNIT_TESTING)
    set(LIBOPENCOR_UNIT_TESTING ${UNIT_TESTING} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(UNIT_TESTING CACHE)

# WARNINGS_AS_ERRORS ==> LIBOPENCOR_WARNINGS_AS_ERRORS.

set(DOCSTRING "Treat warnings as errors.")
set(LIBOPENCOR_WARNINGS_AS_ERRORS ON CACHE BOOL "${DOCSTRING}")

if(DEFINED WARNINGS_AS_ERRORS)
    set(LIBOPENCOR_WARNINGS_AS_ERRORS ${WARNINGS_AS_ERRORS} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(WARNINGS_AS_ERRORS CACHE)

# Make sure that the configuration is sound.

if(LIBOPENCOR_CODE_COVERAGE_GCOV)
    if(NOT LIBOPENCOR_BUILD_SHARED_LIBS)
        message(SEND_ERROR "Configuration confusion: code coverage using gcov is requested which means that a static library cannot be requested.")
    endif()

    if(NOT LIBOPENCOR_UNIT_TESTING)
        message(SEND_ERROR "Configuration confusion: code coverage using gcov is requested which means that unit testing should also be requested.")
    endif()
endif()

if(LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
    if(NOT LIBOPENCOR_BUILD_SHARED_LIBS)
        message(SEND_ERROR "Configuration confusion: code coverage using llvm-cov is requested which means that a static library cannot be requested.")
    endif()

    if(NOT LIBOPENCOR_UNIT_TESTING)
        message(SEND_ERROR "Configuration confusion: code coverage using llvm-cov is requested which means that unit testing should also be requested.")
    endif()
endif()

if(LIBOPENCOR_MEMORY_CHECKS)
    if("${LIBOPENCOR_BUILD_TYPE}" STREQUAL "Release")
        message(SEND_ERROR "Configuration confusion: memory checks are requested which means that a release build cannot also be requested.")
    endif()

    if(NOT LIBOPENCOR_UNIT_TESTING)
        message(SEND_ERROR "Configuration confusion: memory checks are requested which means that unit testing should also be requested.")
    endif()
endif()

if(LIBOPENCOR_PYTHON_UNIT_TESTING)
    if(NOT LIBOPENCOR_PYTHON_BINDINGS)
        message(SEND_ERROR "Configuration confusion: Python unit testing is requested which means that Python bindings should also be requested.")
    endif()
endif()

# Initialise our submodules.

find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                RESULT_VARIABLE RESULT
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_QUIET ERROR_QUIET)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Git submodules could not be initialised.")
endif()

# Update our submodules.

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --recursive --remote
                RESULT_VARIABLE RESULT
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_QUIET ERROR_QUIET)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Git submodules could not be updated.")
endif()

# Enable compiler caching.

if(LIBOPENCOR_COMPILER_CACHING)
    if(BUILDCACHE_EXE)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${BUILDCACHE_EXE})
    elseif(CLCACHE_EXE)
        set(CLCACHEWRAPPER ${CMAKE_CURRENT_BINARY_DIR}/clcachewrapper)

        execute_process(COMMAND ${CMAKE_C_COMPILER} /O2 /Fe${CLCACHEWRAPPER} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clcachewrapper.c
                        RESULT_VARIABLE RESULT
                        OUTPUT_QUIET ERROR_QUIET)

        if(RESULT EQUAL 0)
            set(CMAKE_CXX_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
        endif()
    else()
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
    endif()
endif()

# On Windows, if we are using MSVC then replace some compiler flags.

if(MSVC)
    # If we are treating warnings as errors then remove the /W3 option since it
    # will eventually be replaced with the /W4 and /WX options (see
    # treat_warnings_as_errors()).

    if(LIBCELLML_TREAT_WARNINGS_AS_ERRORS)
        replace_compiler_flag("/W3" "")
    endif()

    # Replace the /Zi option (i.e. enable debugging information), if present,
    # with the /Z7 option (i.e. enable old-style debugging information)
    # otherwise caching (be it with buildcache or clcache) won't work.

    if(BUILDCACHE_EXE OR CLCACHE_EXE)
        replace_compiler_flag("/Zi" "/Z7")
    endif()
endif()

# Enable Python bindings.

if(LIBOPENCOR_PYTHON_BINDINGS)
    add_subdirectory(extern/pybind11)

    mark_as_advanced(
        PYBIND11_FINDPYTHON
        PYBIND11_INSTALL
        PYBIND11_NOPYTHON
        PYBIND11_TEST
    )
endif()

# Build libOpenCOR.

add_subdirectory(src)

# Enable unit testing.

if(LIBOPENCOR_UNIT_TESTING)
    enable_testing()

    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    add_subdirectory(extern/googletest)

    mark_as_advanced(
        BUILD_GMOCK
        INSTALL_GTEST
    )
endif()

# Run our tests.

add_subdirectory(tests)

# Add some custom targets to test the formatting of our code and to format it.
# Note: we don't format API_EXPORT_HEADER_FILE since it's generated.

if(CLANG_FORMAT_AVAILABLE)
    set(GIT_FILES
        ${GIT_API_HEADER_FILES}
        ${GIT_API_MODULE_FILE}
        ${GIT_SOURCE_FILES}
        ${GIT_HEADER_FILES}
        ${GIT_TESTS_SOURCE_FILES}
        ${GIT_TESTS_HEADER_FILES}
    )

    set(CODE_FORMATTING_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/codeformatting.cmake)

    configure_file(cmake/codeformatting.in.cmake
                   ${CODE_FORMATTING_SCRIPT} @ONLY)

    if(CLANG_FORMAT_TESTING_AVAILABLE)
        add_target(code_formatting
                   COMMAND ${CMAKE_COMMAND} -P ${CODE_FORMATTING_SCRIPT}
                   COMMENT "Checking code formatting...")
    endif()

    add_target(format_code
               COMMAND ${CLANG_FORMAT_EXE} -i ${GIT_FILES}
               COMMENT "Formatting the code...")
endif()

# Summary of our configuration.

message(STATUS "Configuration summary:")

if(DEFINED LIBOPENCOR_BUILD_SHARED_LIBS)
    message(STATUS " - LIBOPENCOR_BUILD_SHARED_LIBS:      ${LIBOPENCOR_BUILD_SHARED_LIBS}")
endif()

if(DEFINED LIBOPENCOR_BUILD_TYPE)
    message(STATUS " - LIBOPENCOR_BUILD_TYPE:             ${LIBOPENCOR_BUILD_TYPE}")
endif()

if(DEFINED LIBOPENCOR_CODE_ANALYSIS)
    message(STATUS " - LIBOPENCOR_CODE_ANALYSIS:          ${LIBOPENCOR_CODE_ANALYSIS}")
endif()

if(DEFINED LIBOPENCOR_CODE_COVERAGE_GCOV)
    message(STATUS " - LIBOPENCOR_CODE_COVERAGE_GCOV:     ${LIBOPENCOR_CODE_COVERAGE_GCOV}")
endif()

if(DEFINED LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
    message(STATUS " - LIBOPENCOR_CODE_COVERAGE_LLVM_COV: ${LIBOPENCOR_CODE_COVERAGE_LLVM_COV}")
endif()

if(DEFINED LIBOPENCOR_COMPILER_CACHING)
    message(STATUS " - LIBOPENCOR_COMPILER_CACHING:       ${LIBOPENCOR_COMPILER_CACHING}")
endif()

if(DEFINED LIBOPENCOR_INSTALL_PREFIX)
    message(STATUS " - LIBOPENCOR_INSTALL_PREFIX:         ${LIBOPENCOR_INSTALL_PREFIX}")
endif()

if(DEFINED LIBOPENCOR_MEMORY_CHECKS)
    message(STATUS " - LIBOPENCOR_MEMORY_CHECKS:          ${LIBOPENCOR_MEMORY_CHECKS}")
endif()

if(DEFINED LIBOPENCOR_PYTHON_BINDINGS)
    message(STATUS " - LIBOPENCOR_PYTHON_BINDINGS:        ${LIBOPENCOR_PYTHON_BINDINGS}")
endif()

if(DEFINED LIBOPENCOR_PYTHON_UNIT_TESTING)
    message(STATUS " - LIBOPENCOR_PYTHON_UNIT_TESTING:    ${LIBOPENCOR_PYTHON_UNIT_TESTING}")
endif()

if(DEFINED LIBOPENCOR_UNIT_TESTING)
    message(STATUS " - LIBOPENCOR_UNIT_TESTING:           ${LIBOPENCOR_UNIT_TESTING}")
endif()

if(DEFINED LIBOPENCOR_WARNINGS_AS_ERRORS)
    message(STATUS " - LIBOPENCOR_WARNINGS_AS_ERRORS:     ${LIBOPENCOR_WARNINGS_AS_ERRORS}")
endif()

# Summary of available targets.

message(STATUS "Available targets:")

list(SORT TARGETS)

foreach(TARGET ${TARGETS})
    message(STATUS " - ${TARGET}")
endforeach()
