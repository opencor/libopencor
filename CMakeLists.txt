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

cmake_minimum_required(VERSION 3.14)

project(libOpenCOR
    VERSION 0.0.0
    LANGUAGES CXX
)

# Check the environment we are dealing with.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(environmentchecks)

# Rebrand the CMake options that are relevant to libOpenCOR.

# BINDINGS_PYTHON ==> LIBOPENCOR_BINDINGS_PYTHON.

set(DOCSTRING "Buid Python wrappers.")

if(Python_Development_FOUND)
    set(LIBOPENCOR_BINDINGS_PYTHON ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED BINDINGS_PYTHON)
    if(Python_Development_FOUND)
        set(LIBOPENCOR_BINDINGS_PYTHON "${BINDINGS_PYTHON}" CACHE BOOL "${DOCSTRING}" FORCE)
    else()
        set(IGNORE_ME ${BINDINGS_PYTHON})

        message(WARNING "Python bindings were requested but development files for Python could not be found.")
    endif()

    unset(BINDINGS_PYTHON CACHE)
endif()

# CMAKE_BUILD_TYPE ==> LIBOPENCOR_BUILD_TYPE.

set(DOCSTRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")

if(DEFINED BUILD_TYPE)
    set(LIBOPENCOR_BUILD_TYPE ${BUILD_TYPE} CACHE STRING "${DOCSTRING}" FORCE)

    unset(BUILD_TYPE CACHE)
else()
    set(LIBOPENCOR_BUILD_TYPE "Debug" CACHE STRING "${DOCSTRING}")
endif()

set(CMAKE_BUILD_TYPE ${LIBOPENCOR_BUILD_TYPE} CACHE INTERNAL "Internalise CMAKE_BUILD_TYPE, manipulate via LIBOPENCOR_BUILD_TYPE" FORCE)

# CMAKE_INSTALL_PREFIX ==> LIBOPENCOR_INSTALL_PREFIX.

set(DOCSTRING "Install path prefix, prepended onto install directories.")

if(DEFINED INSTALL_PREFIX)
    set(LIBOPENCOR_INSTALL_PREFIX ${INSTALL_PREFIX} CACHE PATH "${DOCSTRING}" FORCE)

    unset(INSTALL_PREFIX CACHE)
else()
    set(LIBOPENCOR_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "${DOCSTRING}")
endif()

file(TO_CMAKE_PATH ${LIBOPENCOR_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_PATH)

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX_PATH} CACHE INTERNAL "Internalise CMAKE_INSTALL_PREFIX, manipulate via LIBOPENCOR_INSTALL_PREFIX.")

# BUILD_SHARED ==> LIBOPENCOR_BUILD_SHARED.

set(DOCSTRING "Build shared libraries.")

set(LIBOPENCOR_BUILD_SHARED ON CACHE BOOL "${DOCSTRING}")

if(DEFINED BUILD_SHARED)
    set(LIBOPENCOR_BUILD_SHARED ${BUILD_SHARED} CACHE BOOL "${DOCSTRING}" FORCE)

    unset(BUILD_SHARED CACHE)
endif()

set(BUILD_SHARED_LIBS ${LIBOPENCOR_BUILD_SHARED} CACHE INTERNAL "Internalise BUILD_SHARED_LIBS, manipulate via LIBOPENCOR_BUILD_SHARED" FORCE)

# COMPILER_CACHE ==> LIBOPENCOR_COMPILER_CACHE.

set(DOCSTRING "Enable compiler cache.")

if((WIN32 AND CLCACHE_AVAILABLE) OR (NOT WIN32 AND CCACHE_AVAILABLE))
    set(LIBOPENCOR_COMPILER_CACHE ON CACHE BOOL "${DOCSTRING}")
endif()

if(DEFINED COMPILER_CACHE)
    if((WIN32 AND CLCACHE_AVAILABLE) OR (NOT WIN32 AND CCACHE_AVAILABLE))
        set(LIBOPENCOR_COMPILER_CACHE "${COMPILER_CACHE}" CACHE BOOL "${DOCSTRING}" FORCE)
    else()
        if(WIN32)
            set(COMPILER_CACHE_TOOL clcache)
        else()
            set(COMPILER_CACHE_TOOL ccache)
        endif()

        message(WARNING "Compiler cache is requested but ${COMPILER_CACHE_TOOL} could not be found.")
    endif()

    unset(COMPILER_CACHE CACHE)
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

if(LIBOPENCOR_COMPILER_CACHE)
    if(WIN32)
        if(CLCACHE_EXE)
            set(CLCACHEWRAPPER ${CMAKE_CURRENT_BINARY_DIR}/clcachewrapper)

            execute_process(COMMAND ${CMAKE_C_COMPILER} /O2 /Fe${CLCACHEWRAPPER} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clcachewrapper.c
                            RESULT_VARIABLE RESULT OUTPUT_QUIET ERROR_QUIET)

            if(RESULT EQUAL 0)
                set(CMAKE_CXX_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
            endif()
        endif()
    else()
        if(CCACHE_EXE)
            set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
        endif()
    endif()
endif()

# Enable Python bindings through pybind11.

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
