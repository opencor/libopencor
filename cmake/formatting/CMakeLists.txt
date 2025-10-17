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

# Add some custom targets to test the formatting of our code and to format it.
# Note: we don't format API_EXPORT_HEADER_FILE since it's generated.

if(FORMAT_CODE_AVAILABLE)
    set(GIT_FILES
        ${GIT_API_HEADER_FILES}
        ${GIT_API_MODULE_FILE}
        ${GIT_SOURCE_FILES}
        ${GIT_HEADER_FILES}
        ${GIT_TESTS_SOURCE_FILES}
        ${GIT_TESTS_HEADER_FILES}
    )

    set(CODE_FORMATTING_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/codeformatting.cmake)

    configure_file(codeformatting.in.cmake ${CODE_FORMATTING_CMAKE_FILE} @ONLY)

    if(CHECK_CODE_FORMATTING_AVAILABLE)
        add_target(check_code_formatting
                   COMMAND ${CMAKE_COMMAND} -P ${CODE_FORMATTING_CMAKE_FILE}
                   COMMENT "Checking code formatting...")
    endif()

    add_target(format_code
               COMMAND ${CLANG_FORMAT_EXE} -i ${GIT_FILES}
               COMMENT "Formatting the code...")
endif()

# Add some custom targets to test the formatting of our JavaScript code and to format it.

if(JAVASCRIPT_FORMATTING_AVAILABLE)
    add_target(javascript_check_code_formatting
               COMMAND ${BIOME_EXE} format
               WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
               COMMENT "Checking JavaScript code formatting...")

    add_target(javascript_format_code
               COMMAND ${BIOME_EXE} format --write
               WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
               COMMENT "Formatting the JavaScript code...")
endif()

# Add some custom targets to test the formatting of our Python code and to format it.

if(PYTHON_FORMATTING_AVAILABLE)
    add_target(python_check_code_formatting
               COMMAND ${BLACK_EXE} --check .
               WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
               COMMENT "Checking Python code formatting...")

    add_target(python_format_code
               COMMAND ${BLACK_EXE} .
               WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
               COMMENT "Formatting the Python code...")
endif()
