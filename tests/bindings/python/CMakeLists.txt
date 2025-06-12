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

# Configure our utilities.

configure_file(utils.py.in ${PYTHON_BINDINGS_DIR}/utils.py)

# Copy our Python files over.

file(GLOB PYTHON_FILES "*.py")

foreach(PYTHON_FILE ${PYTHON_FILES})
    get_filename_component(PYTHON_FILENAME ${PYTHON_FILE} NAME)

    configure_file(${PYTHON_FILENAME} ${PYTHON_BINDINGS_DIR}/${PYTHON_FILENAME} COPYONLY)
endforeach()

# Run our Python tests.

if(WIN32)
    list(APPEND COMMANDS COMMAND SET PATH=$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>\;%PATH%)
    list(APPEND COMMANDS COMMAND SET PYTHONPATH=${PYTHON_BINDINGS_DIR})
else()
    list(APPEND COMMANDS COMMAND export PYTHONPATH=${PYTHON_BINDINGS_DIR})
endif()

add_target(python_unit_testing
           ${COMMANDS}
           COMMAND ${PYTEST_EXE}
           DEPENDS ${PYTHON_BINDINGS_TARGET}
           USES_TERMINAL
           WORKING_DIRECTORY ${PYTHON_BINDINGS_DIR}
           COMMENT "Running Python unit testing...")

if(PYTHON_UNIT_TESTING_REPORT_AVAILABLE)
    add_target(python_unit_testing_report
               ${COMMANDS}
               COMMAND ${PYTEST_EXE} --html=${CMAKE_CURRENT_BINARY_DIR}/report.html
               DEPENDS ${PYTHON_BINDINGS_TARGET}
               USES_TERMINAL
               WORKING_DIRECTORY ${PYTHON_BINDINGS_DIR}
               COMMENT "Running Python unit testing report...")
endif()
