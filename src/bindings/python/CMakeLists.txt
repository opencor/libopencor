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

# Prevent some warnings about an inconsistent DLL linkage.

if(WIN32)
    add_definitions(-D${CMAKE_PROJECT_NAME}_EXPORTS)
endif()

# Build our Python bindings.

set(PYTHON_BINDINGS_TARGET ${CMAKE_PROJECT_NAME}_Python CACHE INTERNAL "Python bindings target.")

pybind11_add_module(${PYTHON_BINDINGS_TARGET}
                    python.cpp)

target_link_libraries(${PYTHON_BINDINGS_TARGET}
                      ${CMAKE_PROJECT_NAME})

set_target_properties(${PYTHON_BINDINGS_TARGET} PROPERTIES
                      OUTPUT_NAME ${CMAKE_PROJECT_NAME_LC}/${CMAKE_PROJECT_NAME_LC})

# Install our Python bindings.

install(TARGETS ${PYTHON_BINDINGS_TARGET} DESTINATION .)
