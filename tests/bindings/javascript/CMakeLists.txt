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

# (Configure and) copy over some test-related files.

configure_file(utils.js.in utils.js)
configure_file(res/package.json.in package.json)

configure_file(res/favicon.ico favicon.ico COPYONLY)
configure_file(res/index.html index.html COPYONLY)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/res/res" "${CMAKE_CURRENT_BINARY_DIR}/res")

# Copy our JavaScript tests over.

file(GLOB JAVASCRIPT_TESTS "*.js")

foreach(JAVASCRIPT_TEST ${JAVASCRIPT_TESTS})
    get_filename_component(JAVASCRIPT_TEST_FILE ${JAVASCRIPT_TEST} NAME)

    configure_file(${JAVASCRIPT_TEST_FILE} ${JAVASCRIPT_TEST_FILE} COPYONLY)
endforeach()

# Run our JavaScript tests.

add_target(javascript_unit_testing
           COMMAND ${NPM_EXE} install
           COMMAND ${NPM_EXE} test
           DEPENDS ${JAVASCRIPT_BINDINGS_TARGET}
           USES_TERMINAL
           COMMENT "Running JavaScript unit testing...")

# Run our JavaScript code coverage.

if(LIBOPENCOR_JAVASCRIPT_CODE_COVERAGE)
    add_target(javascript_code_coveraage
               COMMAND ${NPM_EXE} install
               COMMAND ${NPM_EXE} run coverage
               DEPENDS ${JAVASCRIPT_BINDINGS_TARGET}
               USES_TERMINAL
               COMMENT "Running JavaScript code coverage...")
endif()
