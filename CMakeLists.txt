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
        VERSION 0.1.0)

string(TOLOWER "${CMAKE_PROJECT_NAME}" CMAKE_PROJECT_NAME_LC)

# Initialise our submodules.

find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                RESULT_VARIABLE RESULT
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_QUIET ERROR_QUIET)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Git submodules could not be retrieved.")
endif()

# Enable C++11.

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# On macOS, deploy on macOS 10.13 and later.

if(APPLE AND "${CMAKE_OSX_DEPLOYMENT_TARGET}" STREQUAL "")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
endif()

# Some common CMake functions/macros.

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(common)

# Enable Python bindings.
# Note: we get pybind11 to look for Python for us since it seems to be doing a
#       better job of it when building libOpenCOR Python using GitHub Actions.
#       Indeed, on GHA Windows, Python 3.7.9 is the official version, yet CMake
#       finds and uses Python 3.10.0, which causes problems when we then want to
#       test our Python bindings since we use the official version of Python.

add_subdirectory(extern/pybind11)

mark_as_advanced(PYBIND11_FINDPYTHON
                 PYBIND11_INSTALL
                 PYBIND11_INTERNALS_VERSION
                 PYBIND11_NOPYTHON
                 PYBIND11_PYTHON_VERSION
                 PYBIND11_TEST)

# Check the environment we are dealing with.

include(environmentchecks)

# Initialise/reset our available targets.

set(TARGETS "install" CACHE INTERNAL "Useful targets.")

# [CMAKE_]BUILD_TYPE ==> LIBOPENCOR_BUILD_TYPE.

if(NOT IS_MULTI_CONFIG)
    set(DOCSTRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ...")

    if(DEFINED BUILD_TYPE)
        set(LIBOPENCOR_BUILD_TYPE "${BUILD_TYPE}" CACHE STRING "${DOCSTRING}" FORCE)
    else()
        set(LIBOPENCOR_BUILD_TYPE "Release" CACHE STRING "${DOCSTRING}")
    endif()

    set(CMAKE_BUILD_TYPE ${LIBOPENCOR_BUILD_TYPE} CACHE INTERNAL "Internalise CMAKE_BUILD_TYPE, manipulate via LIBOPENCOR_BUILD_TYPE." FORCE)
endif()

unset(BUILD_TYPE CACHE)

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

# CODE_COVERAGE_GCOV ==> LIBOPENCOR_CODE_COVERAGE_GCOV.

if(CODE_COVERAGE_GCOV_TESTING_AVAILABLE)
    set(DOCSTRING "Enable code coverage testing using gcov.")
    set(LIBOPENCOR_CODE_COVERAGE_GCOV OFF CACHE BOOL "${DOCSTRING}")
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
    set(LIBOPENCOR_CODE_COVERAGE_LLVM_COV OFF CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED CODE_COVERAGE_LLVM_COV AND CODE_COVERAGE_LLVM_COV_TESTING_AVAILABLE)
    set(LIBOPENCOR_CODE_COVERAGE_LLVM_COV "${CODE_COVERAGE_LLVM_COV}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(CODE_COVERAGE_LLVM_COV)
    message(SEND_ERROR "Code coverage testing using llvm-cov is requested but llvm-cov or its requirements could not be found.")
endif()

unset(CODE_COVERAGE_LLVM_COV CACHE)

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

# DOCUMENTATION ==> LIBOPENCOR_DOCUMENTATION.

if(DOCUMENTATION_AVAILABLE)
    set(DOCSTRING "Generate the documentation.")
    set(LIBOPENCOR_DOCUMENTATION OFF CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED DOCUMENTATION AND DOCUMENTATION_AVAILABLE)
    set(LIBOPENCOR_DOCUMENTATION ${DOCUMENTATION} CACHE BOOL "${DOCSTRING}" FORCE)
elseif(DOCUMENTATION)
    message(SEND_ERROR "Documentation is requested but Doxygen, patch, Python, some Python packages and/or Sphinx could not be found.")
endif()

unset(DOCUMENTATION CACHE)

# [CMAKE_]INSTALL_PREFIX ==> LIBOPENCOR_INSTALL_PREFIX.

set(DOCSTRING "Install path prefix, prepended onto install directories.")

if(DEFINED INSTALL_PREFIX)
    set(LIBOPENCOR_INSTALL_PREFIX ${INSTALL_PREFIX} CACHE PATH "${DOCSTRING}" FORCE)
else()
    set(LIBOPENCOR_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "${DOCSTRING}")
endif()

file(TO_CMAKE_PATH ${LIBOPENCOR_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_PATH)

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX_PATH} CACHE INTERNAL "Internalise CMAKE_INSTALL_PREFIX, manipulate via LIBOPENCOR_INSTALL_PREFIX.")

unset(INSTALL_PREFIX CACHE)

# MEMORY_CHECKS ==> LIBOPENCOR_MEMORY_CHECKS.

if(VALGRIND_AVAILABLE)
    set(DOCSTRING "Enable memory checks.")
    set(LIBOPENCOR_MEMORY_CHECKS OFF CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED MEMORY_CHECKS AND VALGRIND_AVAILABLE)
    set(LIBOPENCOR_MEMORY_CHECKS "${MEMORY_CHECKS}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(MEMORY_CHECKS)
    message(SEND_ERROR "Memory checks are requested but Valgrind could not be found.")
endif()

unset(MEMORY_CHECKS CACHE)

# PREBUILT_LIBCELLML ==> LIBOPENCOR_PREBUILT_LIBCELLML.

set(DOCSTRING "Use the prebuilt version of libCellML.")
set(LIBOPENCOR_PREBUILT_LIBCELLML ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_LIBCELLML)
    set(LIBOPENCOR_PREBUILT_LIBCELLML ${PREBUILT_LIBCELLML} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_LIBCELLML CACHE)

# PREBUILT_LIBCURL ==> LIBOPENCOR_PREBUILT_LIBCURL.

set(DOCSTRING "Use the prebuilt version of libcurl.")
set(LIBOPENCOR_PREBUILT_LIBCURL ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_LIBCURL)
    set(LIBOPENCOR_PREBUILT_LIBCURL ${PREBUILT_LIBCURL} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_LIBCURL CACHE)

# PREBUILT_LIBNUML ==> LIBOPENCOR_PREBUILT_LIBNUML.

set(DOCSTRING "Use the prebuilt version of libNuML.")
set(LIBOPENCOR_PREBUILT_LIBNUML ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_LIBNUML)
    set(LIBOPENCOR_PREBUILT_LIBNUML ${PREBUILT_LIBNUML} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_LIBNUML CACHE)

# PREBUILT_LIBSBML ==> LIBOPENCOR_PREBUILT_LIBSBML.

set(DOCSTRING "Use the prebuilt version of libSBML.")
set(LIBOPENCOR_PREBUILT_LIBSBML ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_LIBSBML)
    set(LIBOPENCOR_PREBUILT_LIBSBML ${PREBUILT_LIBSBML} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_LIBSBML CACHE)

# PREBUILT_LIBSEDML ==> LIBOPENCOR_PREBUILT_LIBSEDML.

set(DOCSTRING "Use the prebuilt version of libSEDML.")
set(LIBOPENCOR_PREBUILT_LIBSEDML ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_LIBSEDML)
    set(LIBOPENCOR_PREBUILT_LIBSEDML ${PREBUILT_LIBSEDML} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_LIBSEDML CACHE)

# PREBUILT_LIBXML2 ==> LIBOPENCOR_PREBUILT_LIBXML2.

set(DOCSTRING "Use the prebuilt version of libxml2.")
set(LIBOPENCOR_PREBUILT_LIBXML2 ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_LIBXML2)
    set(LIBOPENCOR_PREBUILT_LIBXML2 ${PREBUILT_LIBXML2} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_LIBXML2 CACHE)

# PREBUILT_OPENSSL ==> LIBOPENCOR_PREBUILT_OPENSSL.

set(DOCSTRING "Use the prebuilt version of OpenSSL.")
set(LIBOPENCOR_PREBUILT_OPENSSL ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_OPENSSL)
    set(LIBOPENCOR_PREBUILT_OPENSSL ${PREBUILT_OPENSSL} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_OPENSSL CACHE)

# PREBUILT_ZLIB ==> LIBOPENCOR_PREBUILT_ZLIB.

set(DOCSTRING "Use the prebuilt version of zlib.")
set(LIBOPENCOR_PREBUILT_ZLIB ON CACHE BOOL "${DOCSTRING}")

if(DEFINED PREBUILT_ZLIB)
    set(LIBOPENCOR_PREBUILT_ZLIB ${PREBUILT_ZLIB} CACHE BOOL "${DOCSTRING}" FORCE)
endif()

unset(PREBUILT_ZLIB CACHE)

# PYTHON_BINDINGS ==> LIBOPENCOR_PYTHON_BINDINGS.

if(PYTHON_BINDINGS_AVAILABLE)
    set(DOCSTRING "Buid Python bindings.")
    set(LIBOPENCOR_PYTHON_BINDINGS OFF CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED PYTHON_BINDINGS AND PYTHON_BINDINGS_AVAILABLE)
    set(LIBOPENCOR_PYTHON_BINDINGS ${PYTHON_BINDINGS} CACHE BOOL "${DOCSTRING}" FORCE)
elseif(PYTHON_BINDINGS)
    message(SEND_ERROR "Python bindings are requested but Python could not be found.")
endif()

unset(PYTHON_BINDINGS CACHE)

# PYTHON_UNIT_TESTING ==> LIBOPENCOR_PYTHON_UNIT_TESTING.

if(PYTHON_UNIT_TESTING_AVAILABLE)
    set(DOCSTRING "Enable Python unit testing.")
    set(LIBOPENCOR_PYTHON_UNIT_TESTING OFF CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED PYTHON_UNIT_TESTING AND PYTHON_UNIT_TESTING_AVAILABLE)
    set(LIBOPENCOR_PYTHON_UNIT_TESTING "${PYTHON_UNIT_TESTING}" CACHE BOOL "${DOCSTRING}" FORCE)
elseif(PYTHON_UNIT_TESTING)
    message(SEND_ERROR "Python unit testing is requested but Python and/or the Python pytest package could not be found.")
endif()

unset(PYTHON_UNIT_TESTING CACHE)

# [BUILD_]SHARED_LIBS ==> LIBOPENCOR_SHARED_LIBS.

set(DOCSTRING "Build shared libraries.")

if(DEFINED SHARED_LIBS)
    set(LIBOPENCOR_SHARED_LIBS ${SHARED_LIBS} CACHE BOOL "${DOCSTRING}" FORCE)
else()
    set(LIBOPENCOR_SHARED_LIBS ON CACHE BOOL "${DOCSTRING}")
endif()

set(BUILD_SHARED_LIBS ${LIBOPENCOR_SHARED_LIBS} CACHE INTERNAL "Internalise BUILD_SHARED_LIBS, manipulate via LIBOPENCOR_SHARED_LIBS.")

unset(SHARED_LIBS CACHE)

# UNIT_TESTING ==> LIBOPENCOR_UNIT_TESTING.

set(DOCSTRING "Enable unit testing.")
set(LIBOPENCOR_UNIT_TESTING OFF CACHE BOOL "${DOCSTRING}")

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
    if(NOT LIBOPENCOR_SHARED_LIBS)
        message(SEND_ERROR "Configuration confusion: code coverage using gcov is requested which means that a static library cannot be requested.")
    endif()

    if(NOT LIBOPENCOR_UNIT_TESTING)
        message(SEND_ERROR "Configuration confusion: code coverage using gcov is requested which means that unit testing should also be requested.")
    endif()
endif()

if(LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
    if(NOT LIBOPENCOR_SHARED_LIBS)
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

# Build libOpenCOR.

add_subdirectory(src)

# Enable unit testing.

if(LIBOPENCOR_UNIT_TESTING)
    enable_testing()

    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    add_subdirectory(extern/googletest)

    mark_as_advanced(BUILD_GMOCK
                     INSTALL_GTEST)
endif()

# Run our tests.

add_subdirectory(tests)

# Generate our documentation.

add_subdirectory(doc)

# Check our code formatting and format our code.

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake/formatting)

# Package libOpenCOR.

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cmake/packaging)

# Summary of our configuration.

message(STATUS "Configuration summary:")

if(DEFINED LIBOPENCOR_BUILD_TYPE)
    message(STATUS " - LIBOPENCOR_BUILD_TYPE:              ${LIBOPENCOR_BUILD_TYPE}")
endif()

if(DEFINED LIBOPENCOR_CODE_ANALYSIS)
    message(STATUS " - LIBOPENCOR_CODE_ANALYSIS:           ${LIBOPENCOR_CODE_ANALYSIS}")
endif()

if(DEFINED LIBOPENCOR_CODE_COVERAGE_GCOV)
    message(STATUS " - LIBOPENCOR_CODE_COVERAGE_GCOV:      ${LIBOPENCOR_CODE_COVERAGE_GCOV}")
endif()

if(DEFINED LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
    message(STATUS " - LIBOPENCOR_CODE_COVERAGE_LLVM_COV:  ${LIBOPENCOR_CODE_COVERAGE_LLVM_COV}")
endif()

if(DEFINED LIBOPENCOR_COMPILER_CACHING)
    message(STATUS " - LIBOPENCOR_COMPILER_CACHING:        ${LIBOPENCOR_COMPILER_CACHING}")
endif()

if(DEFINED LIBOPENCOR_DOCUMENTATION)
    message(STATUS " - LIBOPENCOR_DOCUMENTATION:           ${LIBOPENCOR_DOCUMENTATION}")
endif()

if(DEFINED LIBOPENCOR_INSTALL_PREFIX)
    message(STATUS " - LIBOPENCOR_INSTALL_PREFIX:          ${LIBOPENCOR_INSTALL_PREFIX}")
endif()

if(DEFINED LIBOPENCOR_MEMORY_CHECKS)
    message(STATUS " - LIBOPENCOR_MEMORY_CHECKS:           ${LIBOPENCOR_MEMORY_CHECKS}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_LIBCELLML)
    message(STATUS " - LIBOPENCOR_PREBUILT_LIBCELLML:      ${LIBOPENCOR_PREBUILT_LIBCELLML}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_LIBCURL)
    message(STATUS " - LIBOPENCOR_PREBUILT_LIBCURL:        ${LIBOPENCOR_PREBUILT_LIBCURL}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_LIBNUML)
    message(STATUS " - LIBOPENCOR_PREBUILT_LIBNUML:        ${LIBOPENCOR_PREBUILT_LIBNUML}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_LIBSBML)
    message(STATUS " - LIBOPENCOR_PREBUILT_LIBSBML:        ${LIBOPENCOR_PREBUILT_LIBSBML}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_LIBSEDML)
    message(STATUS " - LIBOPENCOR_PREBUILT_LIBSEDML:       ${LIBOPENCOR_PREBUILT_LIBSEDML}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_LIBXML2)
    message(STATUS " - LIBOPENCOR_PREBUILT_LIBXML2:        ${LIBOPENCOR_PREBUILT_LIBXML2}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_OPENSSL)
    message(STATUS " - LIBOPENCOR_PREBUILT_OPENSSL:        ${LIBOPENCOR_PREBUILT_OPENSSL}")
endif()

if(DEFINED LIBOPENCOR_PREBUILT_ZLIB)
    message(STATUS " - LIBOPENCOR_PREBUILT_ZLIB:           ${LIBOPENCOR_PREBUILT_ZLIB}")
endif()

if(DEFINED LIBOPENCOR_PYTHON_BINDINGS)
    message(STATUS " - LIBOPENCOR_PYTHON_BINDINGS:         ${LIBOPENCOR_PYTHON_BINDINGS}")
endif()

if(DEFINED LIBOPENCOR_PYTHON_UNIT_TESTING)
    message(STATUS " - LIBOPENCOR_PYTHON_UNIT_TESTING:     ${LIBOPENCOR_PYTHON_UNIT_TESTING}")
endif()

if(DEFINED LIBOPENCOR_SHARED_LIBS)
    message(STATUS " - LIBOPENCOR_SHARED_LIBS:             ${LIBOPENCOR_SHARED_LIBS}")
endif()

if(DEFINED LIBOPENCOR_UNIT_TESTING)
    message(STATUS " - LIBOPENCOR_UNIT_TESTING:            ${LIBOPENCOR_UNIT_TESTING}")
endif()

if(DEFINED LIBOPENCOR_WARNINGS_AS_ERRORS)
    message(STATUS " - LIBOPENCOR_WARNINGS_AS_ERRORS:      ${LIBOPENCOR_WARNINGS_AS_ERRORS}")
endif()

# Summary of available targets.

message(STATUS "Useful targets:")

foreach(TARGET ${TARGETS})
    message(STATUS " - ${TARGET}")
endforeach()
