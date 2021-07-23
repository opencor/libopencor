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

set(TEST_SOURCE_FILES
    version_tests.py
)

if(WIN32)
    set(RUN_PYTHON_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>\;%PATH%;PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python/$<CONFIG>")
else()
    set(RUN_PYTHON_ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python")
endif()

foreach(TEST_SOURCE_FILE ${TEST_SOURCE_FILES})
    configure_file(${TEST_SOURCE_FILE} ${TEST_SOURCE_FILE} COPYONLY)

    get_filename_component(TEST_SOURCE_FILE_NAME ${TEST_SOURCE_FILE} NAME_WE)

    set(TEST_SOURCE_FILE_NAME python_${TEST_SOURCE_FILE_NAME})

    add_test(NAME ${TEST_SOURCE_FILE_NAME}
             COMMAND ${Python_EXECUTABLE} ${TEST_SOURCE_FILE})

    set_tests_properties(${TEST_SOURCE_FILE_NAME} PROPERTIES ENVIRONMENT
        "${RUN_PYTHON_ENVIRONMENT}"
    )

    # set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    #     ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SOURCE_FILE}
    #     ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SOURCE_FILE}c
    # )
endforeach()

# set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
#     ${CMAKE_CURRENT_BINARY_DIR}/__pycache__
# )
