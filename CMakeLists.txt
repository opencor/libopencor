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

# Rebrand the CMake options that are relevant to libOpenCOR.

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

# Hide the CMake options that are not directly relevant to libOpenCOR.

if(WINDOWS)
    mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
elseif(APPLE)
    mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
    mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
    mark_as_advanced(CMAKE_OSX_SYSROOT)
endif()

# Build libOpenCOR.

add_subdirectory(src)
