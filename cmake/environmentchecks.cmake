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

# Hide the CMake options that are not directly relevant to libOpenCOR.

if(WIN32)
    mark_as_advanced(CLCACHE_EXE)
    mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
else()
    mark_as_advanced(CCACHE_EXE)

    if(APPLE)
        mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
        mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
        mark_as_advanced(CMAKE_OSX_SYSROOT)
    endif()
endif()
