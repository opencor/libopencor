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

# Run our Python tests.

set(TEST_SOURCE_FILES
    version_tests.py
)

if(WIN32)
    set(RUN_PYTHON_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>\;%PATH%;PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python")
else()
    set(RUN_PYTHON_ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python")
endif()

foreach(TEST_SOURCE_FILE ${TEST_SOURCE_FILES})
    configure_file(${TEST_SOURCE_FILE} ${TEST_SOURCE_FILE} COPYONLY)

    get_filename_component(TEST_SOURCE_FILE_NAME ${TEST_SOURCE_FILE} NAME_WE)

    set(TEST_SOURCE_FILE_NAME python_${TEST_SOURCE_FILE_NAME})

    add_test(NAME ${TEST_SOURCE_FILE_NAME}
             COMMAND ${Python_EXECUTABLE} ${TEST_SOURCE_FILE})

    set_tests_properties(${TEST_SOURCE_FILE_NAME} PROPERTIES
        ENVIRONMENT "${RUN_PYTHON_ENVIRONMENT}"
    )
endforeach()

# Run our Python coverage tests.

if(LIBOPENCOR_PYTHON_COVERAGE)
    configure_file(runner.py runner.py)

    add_custom_target(python_coverage
                      COMMAND ${CMAKE_COMMAND} -E env "${RUN_PYTHON_ENVIRONMENT}" ${Python_EXECUTABLE} runner.py
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      DEPENDS ${PYTHON_BINDINGS_TARGET}
                      COMMENT "Running Python coverage tests...")

    add_custom_target(python_coverage_report
                      COMMAND ${CMAKE_COMMAND} -E env ${RUN_PYTHON_ENVIRONMENT} ${Python_EXECUTABLE} runner.py html
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      DEPENDS ${PYTHON_BINDINGS_TARGET}
                      COMMENT "Running Python coverage tests with HTML report...")
endif()
