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

# Go through our API header files and generate a test file for each of them.

if(LIBOPENCOR_UNIT_TESTING)
    foreach(GIT_API_HEADER_FILE ${GIT_API_HEADER_FILES})
        get_filename_component(GIT_API_HEADER_NAME ${GIT_API_HEADER_FILE} NAME_WE)

        set(TEST_SOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/headerfiles/${GIT_API_HEADER_NAME}.cpp)

        configure_file(${CMAKE_CURRENT_LIST_DIR}/tests.cpp.in ${TEST_SOURCE_FILE})

        list(APPEND TEST_SOURCE_FILES ${TEST_SOURCE_FILE})
    endforeach()

    prepare_test(api_header_files_tests
                 ${TEST_SOURCE_FILES})
endif()

# Track our template for code formatting.

list(APPEND GIT_TESTS_SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/tests.cpp.in)

set(GIT_TESTS_SOURCE_FILES ${GIT_TESTS_SOURCE_FILES} PARENT_SCOPE)
