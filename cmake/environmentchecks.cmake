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
    else()
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built using MSVC 2022 on Windows.")
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
# Note: normally, we would check the value of CMAKE_SIZEOF_VOID_P, but in some cases it may not be set (e.g., when
#       generating an Xcode project file), so we determine and retrieve that value ourselves.

try_run(ARCHITECTURE_RUN ARCHITECTURE_COMPILE
        ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/architecture.c
        RUN_OUTPUT_VARIABLE ARCHITECTURE)

if(NOT ARCHITECTURE_COMPILE)
    message(FATAL_ERROR "We could not determine your architecture. Please clean your ${CMAKE_PROJECT_NAME} environment and try again.")
elseif(NOT ${ARCHITECTURE} EQUAL 64)
    message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built in 64-bit mode.")
endif()

# Make sure that we are building libOpenCOR for the version of macOS that we are after.

if(APPLE AND NOT "${CMAKE_OSX_DEPLOYMENT_TARGET}" VERSION_GREATER_EQUAL "${MACOS_DEPLOYMENT_TARGET}")
    message(FATAL_ERROR "${CMAKE_PROJECT_NAME} can only be built for macOS ${MACOS_DEPLOYMENT_TARGET} and later.")
endif()

# Determine our default target architecture.
# Note: on Windows, the target architecture depends on the version of MSVC we are using (i.e. x64 or arm64), so we look
#       for /x64/ and /arm64/ in CMAKE_CXX_COMPILER rather than just rely on the value of CMAKE_SYSTEM_PROCESSOR.

if(WIN32)
    string(FIND "${CMAKE_CXX_COMPILER}" "/x64/" INDEX)

    if(NOT INDEX EQUAL -1)
        set(DEFAULT_TARGET_ARCHITECTURE Intel)
    else()
        string(FIND "${CMAKE_CXX_COMPILER}" "/arm64/" INDEX)

        if(NOT INDEX EQUAL -1)
            set(DEFAULT_TARGET_ARCHITECTURE ARM)
        endif()
    endif()
elseif(APPLE)
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(DEFAULT_TARGET_ARCHITECTURE Intel)
    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
        set(DEFAULT_TARGET_ARCHITECTURE ARM)
    endif()
else()
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(DEFAULT_TARGET_ARCHITECTURE Intel)
    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
        set(DEFAULT_TARGET_ARCHITECTURE ARM)
    endif()
endif()

if("${DEFAULT_TARGET_ARCHITECTURE}" STREQUAL "")
    message(FATAL_ERROR "No supported target architecture could be determined for ${CMAKE_PROJECT_NAME}.")
endif()

# On Windows and Linux, our actual target architecture is always our default target architecture.

if(NOT APPLE)
    set(LIBOPENCOR_TARGET_ARCHITECTURE ${DEFAULT_TARGET_ARCHITECTURE})
endif()

# Check whether we are dealing with a single or multiple configuration.

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# Look for various packages and programs.
# Note: when it comes to finding Python, we try to use the registry after the environment variables on Windows and to
#       find frameworks after standard libraries or headers on macOS. (On GitHub Actions, it helps finding the correct
#       version of Python on Windows.)

if("${Python_FIND_REGISTRY}" STREQUAL "")
    set(Python_FIND_REGISTRY "LAST")
endif()

if("${Python_FIND_FRAMEWORK}" STREQUAL "")
    set(Python_FIND_FRAMEWORK "LAST")
endif()

find_package(Doxygen)
find_package(Python ${PREFERRED_PYTHON_VERSION} COMPONENTS Interpreter Development)

if(NOT WIN32 AND NOT APPLE)
    # Some third-party libraries get built with threads support (and so do our tests, though this can be disabled by
    # setting GTEST_DISABLE_PTHREADS to ON), so make sure that it's present since libOpenCOR will need to be linked
    # against pthreads as a result.

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

if(EMCMAKE_EXE AND EMCONFIGURE_EXE)
    set(JAVASCRIPT_BINDINGS_AVAILABLE TRUE)
else()
    set(JAVASCRIPT_BINDINGS_ERROR_MESSAGE "JavaScript bindings are requested but the emcmake and/or emconfigure tools could not be found.")
endif()

if(JAVASCRIPT_BINDINGS_AVAILABLE AND NODE_EXE AND NPM_EXE)
    set(JAVASCRIPT_UNIT_TESTING_AVAILABLE TRUE)
else()
    set(JAVASCRIPT_UNIT_TESTING_ERROR_MESSAGE "JavaScript unit testing is requested but the emcmake, emconfigure, node, and/or npm tools could not be found.")
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
                 EMCMAKE_EXE
                 EMCONFIGURE_EXE
                 FIND_EXE
                 GIT_EXE
                 LLVM_COV_EXE
                 LLVM_PROFDATA_EXE
                 NODE_EXE
                 NPM_EXE
                 PATCH_EXE
                 PRETTIER_EXE
                 PYTEST_EXE
                 SPHINX_EXE
                 VALGRIND_EXE)
