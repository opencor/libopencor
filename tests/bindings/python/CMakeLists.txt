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

if(WIN32)
    list(APPEND COMMANDS COMMAND SET PATH=$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>\;%PATH%)
    list(APPEND COMMANDS COMMAND SET PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python)
else()
    list(APPEND COMMANDS COMMAND export PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python)
endif()

set(TEMPORARY_FILES __pycache__ default.profraw)

add_target(python_unit_testing
           ${COMMANDS}
           COMMAND ${PYTEST_EXE}
           COMMAND ${CMAKE_COMMAND} -E rm -fr ${TEMPORARY_FILES}
           DEPENDS ${PYTHON_BINDINGS_TARGET}
           USES_TERMINAL
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
           COMMENT "Running Python unit testing...")

if(PYTHON_UNIT_TESTING_REPORT_AVAILABLE)
    add_target(python_unit_testing_report
               ${COMMANDS}
               COMMAND ${PYTEST_EXE} --html=${CMAKE_CURRENT_BINARY_DIR}/report.html
               COMMAND ${CMAKE_COMMAND} -E rm -fr ${TEMPORARY_FILES}
               DEPENDS ${PYTHON_BINDINGS_TARGET}
               USES_TERMINAL
               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
               COMMENT "Running Python unit testing report...")
endif()
