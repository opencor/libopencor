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

# Build our JavaScript bindings.

set(JAVASCRIPT_BINDINGS_TARGET JavaScript_${CMAKE_PROJECT_NAME} CACHE INTERNAL "JavaScript bindings target.")

add_custom_target(${JAVASCRIPT_BINDINGS_TARGET}
                  COMMAND echo "Building JavaScript bindings..."
                  DEPENDS ${CMAKE_PROJECT_NAME}
                  USES_TERMINAL
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMENT "Building JavaScript bindings...")

# Install our JavaScript bindings.

# install(TARGETS ${JAVASCRIPT_BINDINGS_TARGET} DESTINATION .)
