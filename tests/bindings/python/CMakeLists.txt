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
    set(RUN_PYTHON_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>\;%PATH%;PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python")
else()
    set(RUN_PYTHON_ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/src/bindings/python")
endif()

add_custom_target(python_unit_testing
                  COMMAND ${RUN_PYTHON_ENVIRONMENT} ${PYTEST_EXE}
                  USES_TERMINAL
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "Running Python unit testing...")
