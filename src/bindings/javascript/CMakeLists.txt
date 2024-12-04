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

# JavaScript source files.
# Note: this is always set so that we can those files to GIT_SOURCE_FILES whether or not we are building our JavaScript
#       bindings.

set(JAVASCRIPT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
    CACHE INTERNAL "JavaScript source files."
)

# Only build our JavaScript bindings if needed.

if(EMSCRIPTEN OR LIBOPENCOR_JAVASCRIPT_BINDINGS)
    if(EMSCRIPTEN)
        # Add some sources to our WebAssembly version of libOpenCOR so we can access its internals.

        if(NOT ONLY_BUILD_THIRD_PARTY_LIBRARIES)
            target_sources(${CMAKE_PROJECT_NAME} PRIVATE
                           ${JAVASCRIPT_SOURCE_FILES})

            target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
                                       $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/api/${CMAKE_PROJECT_NAME_LC}/module>)
        endif()

        # Use the variables that we passed to CMake but that may or may not end up being used.
        # Note: this prevents us from getting a warning about unused variables.

        set(DUMMY
            ${BUILDCACHE_EXE}
            ${CCACHE_EXE}
            ${CLCACHE_EXE}
            ${EMCMAKE_EXE}
            ${EMCONFIGURE_EXE}
            ${LIBOPENCOR_JAVASCRIPT_UNIT_TESTING}
            ${REAL_CMAKE_PROJECT_NAME}
        )

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package.json.in ${CMAKE_CURRENT_BINARY_DIR}/../../package.json @ONLY)
    else()
        # Build our JavaScript bindings.

        set(JAVASCRIPT_BINDINGS_TARGET ${CMAKE_PROJECT_NAME}_JavaScript CACHE INTERNAL "JavaScript bindings target.")

        add_custom_target(${JAVASCRIPT_BINDINGS_TARGET}
                          COMMAND ${EMCMAKE_EXE} ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S ${CMAKE_SOURCE_DIR} -B build
                                                                  -DBUILDCACHE_EXE=${BUILDCACHE_EXE}
                                                                  -DCCACHE_EXE=${CCACHE_EXE}
                                                                  -DCLCACHE_EXE=${CLCACHE_EXE}
                                                                  -DEMCMAKE_EXE=${EMCMAKE_EXE}
                                                                  -DEMCONFIGURE_EXE=${EMCONFIGURE_EXE}
                                                                  -DLIBOPENCOR_BUILD_TYPE=${LIBOPENCOR_BUILD_TYPE}
                                                                  -DLIBOPENCOR_COMPILER_CACHING=${LIBOPENCOR_COMPILER_CACHING}
                                                                  -DLIBOPENCOR_JAVASCRIPT_UNIT_TESTING=${LIBOPENCOR_JAVASCRIPT_UNIT_TESTING}
                                                                  -DLIBOPENCOR_PREBUILT_LIBCELLML=${LIBOPENCOR_PREBUILT_LIBCELLML}
                                                                  -DLIBOPENCOR_PREBUILT_LIBCOMBINE=${LIBOPENCOR_PREBUILT_LIBCOMBINE}
                                                                  -DLIBOPENCOR_PREBUILT_LIBNUML=${LIBOPENCOR_PREBUILT_LIBNUML}
                                                                  -DLIBOPENCOR_PREBUILT_LIBSBML=${LIBOPENCOR_PREBUILT_LIBSBML}
                                                                  -DLIBOPENCOR_PREBUILT_LIBSEDML=${LIBOPENCOR_PREBUILT_LIBSEDML}
                                                                  -DLIBOPENCOR_PREBUILT_LIBXML2=${LIBOPENCOR_PREBUILT_LIBXML2}
                                                                  -DLIBOPENCOR_PREBUILT_SUNDIALS=${LIBOPENCOR_PREBUILT_SUNDIALS}
                                                                  -DLIBOPENCOR_PREBUILT_ZIPPER=${LIBOPENCOR_PREBUILT_ZIPPER}
                                                                  -DLIBOPENCOR_PREBUILT_ZLIB=${LIBOPENCOR_PREBUILT_ZLIB}
                                                                  -DONLY_BUILD_THIRD_PARTY_LIBRARIES=${ONLY_BUILD_THIRD_PARTY_LIBRARIES}
                                                                  -DREAL_CMAKE_PROJECT_NAME=${CMAKE_PROJECT_NAME}
                          COMMAND ${CMAKE_COMMAND} --build build)

        add_dependencies(${CMAKE_PROJECT_NAME} ${JAVASCRIPT_BINDINGS_TARGET})
    endif()
endif()
