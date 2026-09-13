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

# Apply the libOpenCOR patches to the LLVM+Clang source code, if needed.
# Note: the patches are applied by libOpenCOR's CMakeLists.txt when building LLVM+Clang from source. They are also
#       meant to be applied (and committed) to the opencor/llvm-project repository so that our prebuilt packages
#       include them.

if(NOT DEFINED LLVMCLANG_SOURCE_DIR)
    message(FATAL_ERROR "LLVMCLANG_SOURCE_DIR must be set to the LLVM+Clang source directory.")
endif()

if(NOT DEFINED LLVMCLANG_PATCHES_DIR)
    message(FATAL_ERROR "LLVMCLANG_PATCHES_DIR must be set to the directory that contains the libOpenCOR patches to be applied to LLVM+Clang.")
endif()

if(NOT EXISTS "${LLVMCLANG_PATCHES_DIR}")
    message(FATAL_ERROR "The directory that contains the libOpenCOR patches to be applied to LLVM+Clang could not be found (${LLVMCLANG_PATCHES_DIR}).")
endif()

if(NOT EXISTS "${LLVMCLANG_SOURCE_DIR}/clang/lib/CMakeLists.txt")
    message(FATAL_ERROR "The LLVM+Clang source directory could not be found (${LLVMCLANG_SOURCE_DIR}).")
endif()

find_program(GIT_EXECUTABLE NAMES git)

if(NOT GIT_EXECUTABLE)
    message(FATAL_ERROR "git could not be found, so the LLVM+Clang patches could not be applied.")
endif()

# Make sure that the source directory is a git repository, otherwise git apply would silently skip the patches (it
# only applies them if the current or a parent directory is a git repository, which is not something we can rely on,
# e.g. if libOpenCOR's source is a tarball rather than a clone).

execute_process(COMMAND ${GIT_EXECUTABLE} init -q
                WORKING_DIRECTORY "${LLVMCLANG_SOURCE_DIR}"
                RESULT_VARIABLE RESULT
                ERROR_VARIABLE ERROR)

if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR "git init failed, so the LLVM+Clang patches could not be applied (${ERROR}).")
endif()

# Determine the libOpenCOR patches to be applied to LLVM+Clang.

file(GLOB_RECURSE LLVMCLANG_PATCH_FILES "${LLVMCLANG_PATCHES_DIR}/*.patch")

foreach(LLVMCLANG_PATCH_FILE IN LISTS LLVMCLANG_PATCH_FILES)
    get_filename_component(LLVMCLANG_PATCH_NAME "${LLVMCLANG_PATCH_FILE}" NAME)

    # Normalise the line endings of the patch file and of the files to be patched, if needed.
    # Note: on Windows, git checks out text files (such as our patch files) with CRLF line endings by default, while
    #       the LLVM+Clang source files (as extracted from their archive) have LF line endings. git apply cannot
    #       apply a patch with CRLF line endings to files with LF line endings (and vice versa), so we make sure
    #       that both the patch file and the files to be patched use LF line endings. Also, we don't want git to
    #       convert the line endings of the patched files (e.g. on Windows), so we disable any automatic line ending
    #       conversion and ask git to ignore any remaining whitespace differences.

    file(READ "${LLVMCLANG_PATCH_FILE}" LLVMCLANG_PATCH_CONTENT)
    string(REPLACE "\r\n" "\n" LLVMCLANG_PATCH_CONTENT "${LLVMCLANG_PATCH_CONTENT}")

    set(LLVMCLANG_NORMALISED_PATCH_FILE "${LLVMCLANG_SOURCE_DIR}/.llvmclang-patch")

    file(WRITE "${LLVMCLANG_NORMALISED_PATCH_FILE}" "${LLVMCLANG_PATCH_CONTENT}")

    # Normalise the line endings of the files to be patched. They are the files listed in the patch file.

    string(REGEX MATCHALL "diff --git a/[^ ]+ b/[^ ]+"
           LLVMCLANG_PATCHED_FILES "${LLVMCLANG_PATCH_CONTENT}")

    foreach(LLVMCLANG_PATCHED_FILE ${LLVMCLANG_PATCHED_FILES})
        string(REPLACE "diff --git a/" "" LLVMCLANG_PATCHED_FILE "${LLVMCLANG_PATCHED_FILE}")
        string(REGEX REPLACE " b/.*" "" LLVMCLANG_PATCHED_FILE "${LLVMCLANG_PATCHED_FILE}")

        if(EXISTS "${LLVMCLANG_SOURCE_DIR}/${LLVMCLANG_PATCHED_FILE}")
            file(READ "${LLVMCLANG_SOURCE_DIR}/${LLVMCLANG_PATCHED_FILE}" LLVMCLANG_PATCHED_FILE_CONTENT)
            string(REPLACE "\r\n" "\n" LLVMCLANG_PATCHED_FILE_CONTENT "${LLVMCLANG_PATCHED_FILE_CONTENT}")
            file(WRITE "${LLVMCLANG_SOURCE_DIR}/${LLVMCLANG_PATCHED_FILE}" "${LLVMCLANG_PATCHED_FILE_CONTENT}")
        endif()
    endforeach()

    # Check whether the patch has already been applied (e.g. it may have been committed to the opencor/llvm-project
    # repository that we download from). Indeed, if the patch can be reverse-applied, then it has already been applied.

    execute_process(COMMAND ${GIT_EXECUTABLE} -c core.autocrlf=input apply --reverse --check --whitespace=nowarn --ignore-space-change "${LLVMCLANG_NORMALISED_PATCH_FILE}"
                    WORKING_DIRECTORY "${LLVMCLANG_SOURCE_DIR}"
                    RESULT_VARIABLE RESULT
                    ERROR_VARIABLE ERROR)

    if(RESULT EQUAL 0)
        message(STATUS "Applying the LLVM+Clang patch ${LLVMCLANG_PATCH_NAME} - already applied")

        file(REMOVE "${LLVMCLANG_NORMALISED_PATCH_FILE}")

        continue()
    endif()

    execute_process(COMMAND ${GIT_EXECUTABLE} -c core.autocrlf=input apply --whitespace=nowarn --ignore-space-change "${LLVMCLANG_NORMALISED_PATCH_FILE}"
                    WORKING_DIRECTORY "${LLVMCLANG_SOURCE_DIR}"
                    RESULT_VARIABLE RESULT
                    ERROR_VARIABLE ERROR)

    file(REMOVE "${LLVMCLANG_NORMALISED_PATCH_FILE}")

    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "The LLVM+Clang patch ${LLVMCLANG_PATCH_NAME} could not be applied (${ERROR}).")
    endif()

    message(STATUS "Applying the LLVM+Clang patch ${LLVMCLANG_PATCH_NAME} - Success")
endforeach()
