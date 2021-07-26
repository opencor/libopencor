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
find_program(GIT_EXE NAMES ${PRFERRED_GIT_NAMES} git)

if(CLANG_FORMAT_EXE)
    # Note: Git is needed so that we can `git diff` the result of ClangFormat on
    #       our codebase.

    set(CLANG_FORMAT_MINIMUM_VERSION 12)

    execute_process(COMMAND ${CLANG_FORMAT_EXE} -version
                    OUTPUT_VARIABLE CLANG_FORMAT_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)

    string(REGEX REPLACE "^.*clang-format version ([.0-9]+).*$" "\\1" CLANG_FORMAT_VERSION "${CLANG_FORMAT_VERSION}")

    if(CLANG_FORMAT_VERSION VERSION_LESS CLANG_FORMAT_MINIMUM_VERSION)
        message(WARNING "ClangFormat ${CLANG_FORMAT_VERSION} was found, but version ${CLANG_FORMAT_MINIMUM_VERSION}+ is needed to use ClangFormat.")
    else()
        set(CLANG_FORMAT_AVAILABLE TRUE CACHE INTERNAL "Executable required to format the codebase.")

        if(GIT_EXE)
            set(CLANG_FORMAT_TESTING_AVAILABLE TRUE CACHE INTERNAL "Executables required to run the ClangFormat test.")
        endif()
    endif()
endif()

if(CLANG_TIDY_EXE)
    set(CLANG_TIDY_AVAILABLE TRUE CACHE INTERNAL "Executable required to perform static analysis.")
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
    GIT_EXE
)
