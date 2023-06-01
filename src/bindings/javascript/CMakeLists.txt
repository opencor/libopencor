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

if(BUILDING_JAVASCRIPT_BINDINGS)
    # Add some sources to our WASM version of libOpenCOR so we can access its internals.

    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
                   ${CMAKE_CURRENT_SOURCE_DIR}/file.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp)

    # Use the variables that we passed to CMake but that may or may not end up being used.
    # Note: this prevents us from getting a warning about unused variables.

    set(DUMMY
        ${BUILDCACHE_EXE}
        ${CCACHE_EXE}
        ${CLCACHE_EXE}
        ${EMCMAKE_EXE}
        ${EMCONFIGURE_EXE}
        ${NATIVE_PREBUILT_DIR}
    )
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
                                                              -DLIBOPENCOR_PREBUILT_LIBCELLML=${LIBOPENCOR_PREBUILT_LIBCELLML}
                                                              -DLIBOPENCOR_PREBUILT_LIBCOMBINE=${LIBOPENCOR_PREBUILT_LIBCOMBINE}
                                                              -DLIBOPENCOR_PREBUILT_LIBCURL=${LIBOPENCOR_PREBUILT_LIBCURL}
                                                              -DLIBOPENCOR_PREBUILT_LIBNUML=${LIBOPENCOR_PREBUILT_LIBNUML}
                                                              -DLIBOPENCOR_PREBUILT_LIBSBML=${LIBOPENCOR_PREBUILT_LIBSBML}
                                                              -DLIBOPENCOR_PREBUILT_LIBSEDML=${LIBOPENCOR_PREBUILT_LIBSEDML}
                                                              -DLIBOPENCOR_PREBUILT_LIBSSH2=${LIBOPENCOR_PREBUILT_LIBSSH2}
                                                              -DLIBOPENCOR_PREBUILT_LIBXML2=${LIBOPENCOR_PREBUILT_LIBXML2}
                                                              -DLIBOPENCOR_PREBUILT_LLVMCLANG=${LIBOPENCOR_PREBUILT_LLVMCLANG}
                                                              -DLIBOPENCOR_PREBUILT_OPENSSL=${LIBOPENCOR_PREBUILT_OPENSSL}
                                                              -DLIBOPENCOR_PREBUILT_SUNDIALS=${LIBOPENCOR_PREBUILT_SUNDIALS}
                                                              -DLIBOPENCOR_PREBUILT_ZIPPER=${LIBOPENCOR_PREBUILT_ZIPPER}
                                                              -DLIBOPENCOR_PREBUILT_ZLIB=${LIBOPENCOR_PREBUILT_ZLIB}
                                                              -DNATIVE_PREBUILT_DIR=${PREBUILT_DIR}
                      COMMAND ${CMAKE_COMMAND} --build build)

    add_dependencies(${CMAKE_PROJECT_NAME} ${JAVASCRIPT_BINDINGS_TARGET})
endif()
