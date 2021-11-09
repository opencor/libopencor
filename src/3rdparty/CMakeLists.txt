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

# Check whether we are using the compiler that was used to generate prebuilt
# versions of our third-party libraries.

set(VALID_PREBUILT_COMPILER FALSE)

if(WIN32)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(VALID_PREBUILT_COMPILER TRUE)
    endif()
elseif(APPLE)
    if(   "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
       OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        set(VALID_PREBUILT_COMPILER TRUE)
    endif()
else()
    if(   "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"
       OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       set(VALID_PREBUILT_COMPILER TRUE)
    endif()
endif()

# Determine whether we are building in release mode.
# Note: this only makes sense if we are not in a multi-configuration mode, i.e.
#       IS_MULTI_CONFIG is equal to FALSE, which is likely to be the case when
#       building a third-party library.

if("${LIBOPENCOR_BUILD_TYPE}" STREQUAL "Debug")
    set(RELEASE_MODE FALSE)
else()
    set(RELEASE_MODE TRUE)
endif()

# Determine the platform on which we are.

if(WIN32)
    if(RELEASE_MODE)
        set(TARGET_PLATFORM windows.release)
    else()
        set(TARGET_PLATFORM windows.debug)
    endif()
elseif(APPLE)
    set(TARGET_PLATFORM macos)
else()
    set(TARGET_PLATFORM linux)
endif()

# Build our different third-party libraries as external projects and statically
# link them to libOpenCOR.

add_subdirectory(libCellML)
