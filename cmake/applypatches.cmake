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

# Apply our patches to the source code of a third-party library.
# Note #1: the patches are applied by build_package() when building a third-party library from source (see
#          patch_package_args() in packages.cmake), which is also how our prebuilt packages are built.
# Note #2: ExternalProject downloads and extracts the source code afresh whenever our patches or this script change
#          (see patch_package_args() in packages.cmake), so we always patch a pristine copy of it and never need to
#          revert anything.
# Note #3: our patches have LF line endings, whatever the platform (see .gitattributes) and so do (normally) the source
#          files that they patch. Still, we make sure that Git never writes CRLF line endings into the patched files
#          (core.autocrlf=input overrides Git for Windows' default, i.e. core.autocrlf=true), and we ask Git to ignore
#          whitespace differences (including CR characters), so that a patch still applies should either side have CRLF
#          line endings.

if(NOT DEFINED PACKAGE_NAME)
    message(FATAL_ERROR "PACKAGE_NAME must be set to the name of the third-party library to be patched.")
endif()

if(NOT DEFINED SOURCE_DIR OR NOT EXISTS "${SOURCE_DIR}")
    message(FATAL_ERROR "SOURCE_DIR must be set to the ${PACKAGE_NAME} source directory.")
endif()

if(NOT DEFINED PATCHES_DIR OR NOT EXISTS "${PATCHES_DIR}")
    message(FATAL_ERROR "PATCHES_DIR must be set to the directory that contains the libOpenCOR patches to be applied to ${PACKAGE_NAME}.")
endif()

find_program(GIT_EXECUTABLE NAMES git)

if(NOT GIT_EXECUTABLE)
    message(FATAL_ERROR "Git could not be found, so the ${PACKAGE_NAME} patches could not be applied.")
endif()

# Make sure that the source directory is a Git repository.
# Note: indeed, otherwise, git apply would silently skip the patches since it only applies them if the current or a
#       parent directory is a Git repository, which is not something we can rely on (e.g., if libOpenCOR's source is a
#       tarball rather than a clone).

execute_process(COMMAND ${GIT_EXECUTABLE} init -q
                WORKING_DIRECTORY "${SOURCE_DIR}"
                RESULT_VARIABLE RESULT
                ERROR_VARIABLE ERROR)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "git init failed, so the ${PACKAGE_NAME} patches could not be applied (${ERROR}).")
endif()

# Apply our patches, unless they have already been applied (i.e. they can be reverse-applied).

set(GIT_APPLY ${GIT_EXECUTABLE} -c core.autocrlf=input apply --whitespace=nowarn --ignore-space-change)

file(GLOB_RECURSE PATCH_FILES "${PATCHES_DIR}/*.patch")

foreach(PATCH_FILE IN LISTS PATCH_FILES)
    get_filename_component(PATCH_NAME "${PATCH_FILE}" NAME)

    message(STATUS "Applying the ${PACKAGE_NAME} patch ${PATCH_NAME}")

    execute_process(COMMAND ${GIT_APPLY} --reverse --check "${PATCH_FILE}"
                    WORKING_DIRECTORY "${SOURCE_DIR}"
                    RESULT_VARIABLE RESULT
                    OUTPUT_QUIET
                    ERROR_QUIET)

    if(RESULT EQUAL 0)
        message(STATUS "Applying the ${PACKAGE_NAME} patch ${PATCH_NAME} - Already applied")
    else()
        execute_process(COMMAND ${GIT_APPLY} "${PATCH_FILE}"
                        WORKING_DIRECTORY "${SOURCE_DIR}"
                        RESULT_VARIABLE RESULT
                        ERROR_VARIABLE ERROR)

        if(NOT RESULT EQUAL 0)
            message(FATAL_ERROR "The ${PACKAGE_NAME} patch ${PATCH_NAME} could not be applied (${ERROR}).")
        endif()

        message(STATUS "Applying the ${PACKAGE_NAME} patch ${PATCH_NAME} - Success")
    endif()
endforeach()
