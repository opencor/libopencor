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
                   ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp)
else()
    # Build our JavaScript bindings.

    set(JAVASCRIPT_BINDINGS_TARGET ${CMAKE_PROJECT_NAME}_JavaScript CACHE INTERNAL "JavaScript bindings target.")

    if(   NOT LIBOPENCOR_PREBUILT_LIBCELLML
       OR NOT LIBOPENCOR_PREBUILT_LIBCOMBINE
       OR NOT LIBOPENCOR_PREBUILT_LIBNUML
       OR NOT LIBOPENCOR_PREBUILT_LIBSBML
       OR NOT LIBOPENCOR_PREBUILT_LIBSSH2
       OR NOT LIBOPENCOR_PREBUILT_LIBXML2
       OR NOT LIBOPENCOR_PREBUILT_OPENSSL
       OR NOT LIBOPENCOR_PREBUILT_SUNDIALS
       OR NOT LIBOPENCOR_PREBUILT_ZIPPER
       OR NOT LIBOPENCOR_PREBUILT_ZLIB)
        set(DEMCMAKE_EXE -DEMCMAKE_EXE=${EMCMAKE_EXE})
    endif()

    if(NOT LIBOPENCOR_PREBUILT_OPENSSL)
        set(DEMCONFIGURE_EXE -DEMCONFIGURE_EXE=${EMCONFIGURE_EXE})
    endif()

    add_custom_target(${JAVASCRIPT_BINDINGS_TARGET}
                      COMMAND ${EMCMAKE_EXE} ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S ${CMAKE_SOURCE_DIR} -B build
                                                              ${DEMCMAKE_EXE}
                                                              ${DEMCONFIGURE_EXE}
                                                              -DLIBOPENCOR_PREBUILT_LIBCELLML=${LIBOPENCOR_PREBUILT_LIBCELLML}
                                                              -DLIBOPENCOR_PREBUILT_LIBCOMBINE=${LIBOPENCOR_PREBUILT_LIBCOMBINE}
                                                              -DLIBOPENCOR_PREBUILT_LIBNUML=${LIBOPENCOR_PREBUILT_LIBNUML}
                                                              -DLIBOPENCOR_PREBUILT_LIBSBML=${LIBOPENCOR_PREBUILT_LIBSBML}
                                                              -DLIBOPENCOR_PREBUILT_LIBSSH2=${LIBOPENCOR_PREBUILT_LIBSSH2}
                                                              -DLIBOPENCOR_PREBUILT_LIBXML2=${LIBOPENCOR_PREBUILT_LIBXML2}
                                                              -DLIBOPENCOR_PREBUILT_OPENSSL=${LIBOPENCOR_PREBUILT_OPENSSL}
                                                              -DLIBOPENCOR_PREBUILT_SUNDIALS=${LIBOPENCOR_PREBUILT_SUNDIALS}
                                                              -DLIBOPENCOR_PREBUILT_ZIPPER=${LIBOPENCOR_PREBUILT_ZIPPER}
                                                              -DLIBOPENCOR_PREBUILT_ZLIB=${LIBOPENCOR_PREBUILT_ZLIB}
                      COMMAND ${CMAKE_COMMAND} --build build
                      WORKING_DIRECTORY ${DOCUMENTATION_BUILD_DIR})

    add_dependencies(${CMAKE_PROJECT_NAME} ${JAVASCRIPT_BINDINGS_TARGET})
endif()
