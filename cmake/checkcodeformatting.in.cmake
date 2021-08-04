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

# Ignore differences in line endings on Windows.
# Note: indeed, Windows uses CRLF line endings while our code uses LF line
#       endings (i.e. UNIX style).

if(WIN32)
    set(GIT_DIFF_OPTION_FOR_WINDOWS --ignore-space-at-eol)
endif()

# Check whether our Git files are badly formatted.

set(GIT_FILES @GIT_FILES@)
set(BADLY_FORMATTED_FILES)

foreach(GIT_FILE ${GIT_FILES})
    execute_process(COMMAND "@CLANG_FORMAT_EXE@" "${GIT_FILE}"
                    COMMAND "@GIT_EXE@" diff --no-index ${GIT_DIFF_OPTION_FOR_WINDOWS} -- "${GIT_FILE}" -
                    RESULT_VARIABLE RESULT
                    OUTPUT_QUIET ERROR_QUIET)

    if(NOT RESULT EQUAL 0)
        list(APPEND BADLY_FORMATTED_FILES ${GIT_FILE})
    endif()
endforeach()

# Output the summary of our checks.

message(STATUS "Summary:")

list(LENGTH GIT_FILES GIT_FILES_COUNT)
list(LENGTH BADLY_FORMATTED_FILES BADLY_FORMATTED_FILES_COUNT)

math(EXPR PROPERLY_FORMATTED_FILES_PERCENTAGE "100*(${GIT_FILES_COUNT}-${BADLY_FORMATTED_FILES_COUNT})/${GIT_FILES_COUNT}")

message("     ${PROPERLY_FORMATTED_FILES_PERCENTAGE}% files properly formatted, ${BADLY_FORMATTED_FILES_COUNT} files badly formatted out of ${GIT_FILES_COUNT}.")

if(BADLY_FORMATTED_FILES)
    message("")
    message("     The badly formatted files are:")

    foreach(BADLY_FORMATTED_FILE ${BADLY_FORMATTED_FILES})
        string(REPLACE "@PROJECT_SOURCE_DIR@/" "" BADLY_FORMATTED_FILE "${BADLY_FORMATTED_FILE}")

        message("      - ${BADLY_FORMATTED_FILE}")
    endforeach()

    message("")
    message(FATAL_ERROR "Please correct the code's formatting by running the 'format_code' target.")
endif()
