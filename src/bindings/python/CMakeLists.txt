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

    pybind11_add_module(${PYTHON_BINDINGS_TARGET}
                        ${PYTHON_SOURCE_FILES})

    set_target_properties(${PYTHON_BINDINGS_TARGET} PROPERTIES
                          OUTPUT_NAME ${CMAKE_PROJECT_NAME_LC}/module)

    target_compile_definitions(${PYTHON_BINDINGS_TARGET} PRIVATE
                               PROJECT_VERSION=${PROJECT_VERSION})

    target_include_directories(${PYTHON_BINDINGS_TARGET} PRIVATE
                               $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/api/${CMAKE_PROJECT_NAME_LC}/module>)

    target_link_libraries(${PYTHON_BINDINGS_TARGET} PRIVATE
                          $<TARGET_NAME:${CMAKE_PROJECT_NAME}>)

    if($ENV{GITHUB_ACTIONS})
        set(GITHUB_ACTIONS ON)
    else()
        set(GITHUB_ACTIONS OFF)
    endif()

    if(GITHUB_ACTIONS AND WIN32 AND SKBUILD)
        # Note: this is to account for the fact that the `pip install libOpenCOR` step in our CI doesn't, on GitHub
        #       Actions' Windows runner, generate the library in the correct place while it used to be fine before (see
        #       https://github.com/actions/runner-images/issues/10004). Normally, we would expect the library to be
        #       created in _skbuild/win-amd64-3.12/cmake-build/src/bindings/python/Release/libopencor but for some
        #       reasons it is created in _skbuild/win-amd64-3.12/cmake-build/src/bindings/python/Release. So, we need to
        #       copy it to the correct place otherwise its installation will fail (besides the fact that we want to copy
        #       it to our test directory so that we can test things properly).

        add_custom_command(TARGET ${PYTHON_BINDINGS_TARGET} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${PYTHON_BINDINGS_TARGET}>/../$<TARGET_FILE_NAME:${PYTHON_BINDINGS_TARGET}>
                                                            $<TARGET_FILE:${PYTHON_BINDINGS_TARGET}>)
    endif()

    add_custom_command(TARGET ${PYTHON_BINDINGS_TARGET} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PYTHON_BINDINGS_TARGET}>
                                                        ${PYTHON_BINDINGS_DIR}/${CMAKE_PROJECT_NAME_LC}/$<TARGET_FILE_NAME:${PYTHON_BINDINGS_TARGET}>)

    add_subdirectory(${CMAKE_PROJECT_NAME_LC})

    # Install our Python bindings.

    install(TARGETS ${PYTHON_BINDINGS_TARGET} DESTINATION .)
endif()
