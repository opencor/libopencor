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

# BINDINGS_PYTHON ==> LIBOPENCOR_BINDINGS_PYTHON.

if(Python_Development_FOUND)
    set(DOCSTRING "Buid Python wrappers.")
    set(LIBOPENCOR_BINDINGS_PYTHON ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED BINDINGS_PYTHON)
    if(Python_Development_FOUND)
        set(LIBOPENCOR_BINDINGS_PYTHON ${BINDINGS_PYTHON} CACHE BOOL "${DOCSTRING}" FORCE)
    else()
        set(IGNORE_ME ${BINDINGS_PYTHON})

        message(WARNING "Python bindings are requested but Python development files could not be found.")
    endif()

    unset(BINDINGS_PYTHON CACHE)
endif()

# BUILD_SHARED_LIBS ==> LIBOPENCOR_BUILD_SHARED_LIBS.

set(DOCSTRING "Build shared libraries.")
set(LIBOPENCOR_BUILD_SHARED_LIBS ON CACHE BOOL "${DOCSTRING}")

if(DEFINED BUILD_SHARED_LIBS)
    set(LIBOPENCOR_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

set(BUILD_SHARED_LIBS ${LIBOPENCOR_BUILD_SHARED_LIBS} CACHE INTERNAL "Internalise BUILD_SHARED_LIBS, manipulate via LIBOPENCOR_BUILD_SHARED_LIBS." FORCE)

# [CMAKE_]BUILD_TYPE ==> LIBOPENCOR_BUILD_TYPE.

set(DOCSTRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ...")

if(DEFINED BUILD_TYPE)
    set(LIBOPENCOR_BUILD_TYPE "${BUILD_TYPE}" CACHE STRING "${DOCSTRING}" FORCE)

    unset(BUILD_TYPE CACHE)
else()
    set(LIBOPENCOR_BUILD_TYPE "Debug" CACHE STRING "${DOCSTRING}")
endif()

set(CMAKE_BUILD_TYPE ${LIBOPENCOR_BUILD_TYPE} CACHE INTERNAL "Internalise CMAKE_BUILD_TYPE, manipulate via LIBOPENCOR_BUILD_TYPE." FORCE)

# [CMAKE_]INSTALL_PREFIX ==> LIBOPENCOR_INSTALL_PREFIX.

set(DOCSTRING "Install path prefix, prepended onto install directories.")

if(DEFINED INSTALL_PREFIX)
    set(LIBOPENCOR_INSTALL_PREFIX ${INSTALL_PREFIX} CACHE PATH "${DOCSTRING}" FORCE)

    unset(INSTALL_PREFIX CACHE)
else()
    set(LIBOPENCOR_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "${DOCSTRING}")
endif()

file(TO_CMAKE_PATH ${LIBOPENCOR_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_PATH)

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX_PATH} CACHE INTERNAL "Internalise CMAKE_INSTALL_PREFIX, manipulate via LIBOPENCOR_INSTALL_PREFIX." FORCE)

# CODE_ANALYSIS ==> LIBOPENCOR_CODE_ANALYSIS.

if(CLANG_TIDY_AVAILABLE)
    set(DOCSTRING "Enable code analysis.")
    set(LIBOPENCOR_CODE_ANALYSIS OFF CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED CODE_ANALYSIS)
    if(CLANG_TIDY_AVAILABLE)
        set(LIBOPENCOR_CODE_ANALYSIS ${CODE_ANALYSIS} CACHE BOOL "${DOCSTRING}" FORCE)
    else()
        message(WARNING "Code analysis is requested but clang-tidy could not be found.")
    endif()

    unset(CODE_ANALYSIS CACHE)
endif()

# COMPILER_CACHING ==> LIBOPENCOR_COMPILER_CACHING.

if(CLCACHE_AVAILABLE OR CCACHE_AVAILABLE)
    set(DOCSTRING "Enable compiler caching.")
    set(LIBOPENCOR_COMPILER_CACHING ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED COMPILER_CACHING)
    if(CLCACHE_AVAILABLE OR CCACHE_AVAILABLE)
        set(LIBOPENCOR_COMPILER_CACHING ${COMPILER_CACHING} CACHE BOOL "${DOCSTRING}" FORCE)
    else()
        if(WIN32)
            set(COMPILER_CACHING_TOOL clcache)
        else()
            set(COMPILER_CACHING_TOOL ccache)
        endif()

        message(WARNING "Compiler caching is requested but ${COMPILER_CACHING_TOOL} could not be found.")
    endif()

    unset(COMPILER_CACHING CACHE)
endif()

# COVERAGE ==> LIBOPENCOR_COVERAGE

set(DOCSTRING "Enable coverage testing.")

if(COVERAGE_TESTING_AVAILABLE)
    set(LIBOPENCOR_COVERAGE ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED COVERAGE)
    if(COVERAGE_TESTING_AVAILABLE)
        set(LIBOPENCOR_COVERAGE "${COVERAGE}" CACHE BOOL "${DOCSTRING}" FORCE)
    else()
        message(WARNING "Coverage testing is requested but gcov or its requirements could not be found.")
    endif()

    unset(COVERAGE CACHE)
endif()

# MEMORY_CHECKS ==> LIBOPENCOR_MEMORY_CHECKS

set(DOCSTRING "Enable memory checks.")

if(VALGRIND_AVAILABLE)
    set(LIBOPENCOR_MEMORY_CHECKS ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED MEMORY_CHECKS)
    if(MEMORY_CHECKS)
        if(VALGRIND_AVAILABLE)
            set(LIBOPENCOR_MEMORY_CHECKS "${MEMORY_CHECKS}" CACHE BOOL "${DOCSTRING}" FORCE)
        else()
            message(WARNING "Memory checks are requested but Valgrind could not be found.")
        endif()
    endif()

    unset(MEMORY_CHECKS CACHE)
endif()

# UNIT_TESTING ==> LIBOPENCOR_UNIT_TESTING.

set(DOCSTRING "Enable unit testing.")
set(LIBOPENCOR_UNIT_TESTING ON CACHE BOOL "${DOCSTRING}")

if(DEFINED UNIT_TESTING)
    set(LIBOPENCOR_UNIT_TESTING ${UNIT_TESTING} CACHE BOOL "${DOCSTRING}" FORCE)

    unset(UNIT_TESTING CACHE)
endif()

# WARNINGS_AS_ERRORS ==> LIBOPENCOR_WARNINGS_AS_ERRORS.

set(DOCSTRING "Treat warnings as errors.")
set(LIBOPENCOR_WARNINGS_AS_ERRORS ON CACHE BOOL "${DOCSTRING}")

if(DEFINED WARNINGS_AS_ERRORS)
    set(LIBOPENCOR_WARNINGS_AS_ERRORS ${WARNINGS_AS_ERRORS} CACHE BOOL "${DOCSTRING}" FORCE)

    unset(WARNINGS_AS_ERRORS CACHE)
endif()

# Make sure that the configuration is sound.

if(LIBOPENCOR_MEMORY_CHECKS)
    if("${LIBOPENCOR_BUILD_TYPE}" STREQUAL "Release")
        message(SEND_ERROR "Configuration confusion: memory checks have been requested but so has a release build.")
    endif()

    if(NOT LIBOPENCOR_UNIT_TESTING)
        message(SEND_ERROR "Configuration confusion: memory checks have been requested but not unit testing.")
    endif()
endif()

# Update our submodules.

find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE RESULT
                OUTPUT_QUIET
                ERROR_QUIET)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Git submodules could not be updated.")
endif()

# Enable clcache/ccache.

if(LIBOPENCOR_COMPILER_CACHING)
    if(CLCACHE_EXE)
        set(CLCACHEWRAPPER ${CMAKE_CURRENT_BINARY_DIR}/clcachewrapper)

        execute_process(COMMAND ${CMAKE_C_COMPILER} /O2 /Fe${CLCACHEWRAPPER} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clcachewrapper.c
                        RESULT_VARIABLE RESULT OUTPUT_QUIET ERROR_QUIET)

        if(RESULT EQUAL 0)
            set(CMAKE_CXX_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
        endif()
    else()
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
    endif()
endif()

# Enable Python bindings.

if(LIBOPENCOR_BINDINGS_PYTHON)
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
    add_subdirectory(tests)

    mark_as_advanced(
        BUILD_GMOCK
        INSTALL_GTEST
    )
endif()

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

    set(CHECK_CODE_FORMATTING_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/checkcodeformatting.cmake)

    configure_file(cmake/checkcodeformatting.in.cmake
                   ${CHECK_CODE_FORMATTING_SCRIPT} @ONLY)

    if(CLANG_FORMAT_TESTING_AVAILABLE)
        add_custom_target(check_code_formatting
            COMMAND ${CMAKE_COMMAND} -P ${CHECK_CODE_FORMATTING_SCRIPT}
        )
    endif()

    add_custom_target(format_code
        COMMAND ${CLANG_FORMAT_EXE} -i ${GIT_FILES}
        COMMENT "Formatting the code..."
    )
endif()
