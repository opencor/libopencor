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

set(JAVASCRIPT_BINDINGS_TARGET ${CMAKE_PROJECT_NAME}_JavaScript CACHE INTERNAL "JavaScript bindings target.")

if(EMSCRIPTEN)
    # Build our JavaScript bindings.

    add_executable(${JAVASCRIPT_BINDINGS_TARGET}
        javascript.cpp
    )

    target_link_libraries(${JAVASCRIPT_BINDINGS_TARGET} PUBLIC ${CMAKE_PROJECT_NAME})

    set_target_properties(${JAVASCRIPT_BINDINGS_TARGET} PROPERTIES
        LINK_FLAGS "--bind"
        OUTPUT_NAME libopencor
    )
else()
    # Build our JavaScript bindings using Emscripten.

    set(JAVASCRIPT_BINDINGS_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)

    add_custom_target(${JAVASCRIPT_BINDINGS_TARGET}
                      COMMAND ${CMAKE_COMMAND} -E rm -fr ${JAVASCRIPT_BINDINGS_BUILD_DIR}
                      COMMAND mkdir ${JAVASCRIPT_BINDINGS_BUILD_DIR}
                      COMMAND cd ${JAVASCRIPT_BINDINGS_BUILD_DIR}
                      COMMAND ${EMCMAKE_EXE} ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ${CMAKE_SOURCE_DIR}
                      COMMAND ${CMAKE_COMMAND} --build .
                      DEPENDS ${CMAKE_PROJECT_NAME}
                      USES_TERMINAL
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      COMMENT "Building our JavaScript bindings using Emscripten...")

    # Install our JavaScript bindings.

    set(JAVASCRIPT_BINDINGS_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    string(REPLACE "${CMAKE_SOURCE_DIR}" "" JAVASCRIPT_BINDINGS_BINARY_DIR "${JAVASCRIPT_BINDINGS_BINARY_DIR}")

    install(PROGRAMS ${JAVASCRIPT_BINDINGS_BUILD_DIR}/${JAVASCRIPT_BINDINGS_BINARY_DIR}/libopencor.js
                     ${JAVASCRIPT_BINDINGS_BUILD_DIR}/${JAVASCRIPT_BINDINGS_BINARY_DIR}/libopencor.wasm
            DESTINATION .)
endif()
