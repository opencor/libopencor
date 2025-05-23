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

# Python source files.
# Note: this is always set so that we can those files to GIT_SOURCE_FILES whether or not we are building our Python
#       bindings.

set(PYTHON_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
    CACHE INTERNAL "Python source files."
)

# Only build our Python bindings if needed.

if(LIBOPENCOR_PYTHON_BINDINGS)
    # Make it possible to unit test our Python bindings.

    set(PYTHON_BINDINGS_DIR ${CMAKE_BINARY_DIR}/tests/bindings/python CACHE INTERNAL "Python bindings directory.")

    # Build our Python bindings.

    set(PYTHON_BINDINGS_TARGET ${CMAKE_PROJECT_NAME}_Python CACHE INTERNAL "Python bindings target.")

    nanobind_add_module(${PYTHON_BINDINGS_TARGET}
                        LTO
                        NOMINSIZE
                        NB_STATIC
                        STABLE_ABI
                        ${PYTHON_SOURCE_FILES})

    set_target_properties(${PYTHON_BINDINGS_TARGET} PROPERTIES
                          OUTPUT_NAME ${CMAKE_PROJECT_NAME_LC}/module)

    target_compile_definitions(${PYTHON_BINDINGS_TARGET} PRIVATE
                               PROJECT_VERSION=${PROJECT_VERSION})

    target_link_libraries(${PYTHON_BINDINGS_TARGET} PRIVATE
                          $<TARGET_NAME:${CMAKE_PROJECT_NAME}>)

    add_custom_command(TARGET ${PYTHON_BINDINGS_TARGET} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PYTHON_BINDINGS_TARGET}>
                                                        ${PYTHON_BINDINGS_DIR}/${CMAKE_PROJECT_NAME_LC}/$<TARGET_FILE_NAME:${PYTHON_BINDINGS_TARGET}>)

    add_subdirectory(${CMAKE_PROJECT_NAME_LC})

    # Install our Python bindings.

    install(TARGETS ${PYTHON_BINDINGS_TARGET} DESTINATION .)
endif()
