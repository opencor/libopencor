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

set(PACKAGE_NAME libxml2)
set(PACKAGE_VERSION 2.12.7)
set(PACKAGE_REPOSITORY libxml2)
set(RELEASE_TAG v2.12.7)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBXML2)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         038dd6ea7813556a8416c8e99cddfbc9b9f7593d)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 21c56302ad4673f5f831fb42534604cd15dd2a77)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 1810b86eea9ef55e54488c5568f1811a43756064)
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 e798ee267745202622399e322feefeee596e7958)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 5b487688d5e66cb2db562fee17d4a5da2d38bc8e)
            endif()
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             a5996ff52ed5e952cd02a2493723404161d3e8cc)
        endif()
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBXML2_WITH_DEBUG=OFF
            -DLIBXML2_WITH_ICONV=OFF
            -DLIBXML2_WITH_LEGACY=OFF
            -DLIBXML2_WITH_LZMA=OFF
            -DLIBXML2_WITH_MODULES=OFF
            -DLIBXML2_WITH_PROGRAMS=OFF
            -DLIBXML2_WITH_PYTHON=OFF
            -DLIBXML2_WITH_TESTS=OFF
            -DLIBXML2_WITH_ZLIB=OFF
    )

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include ${STATIC_LIBRARIES})
endif()

# Make our package accessible.

if(WIN32)
    set(LIBRARY_SUFFIX s)

    if(NOT RELEASE_MODE)
        set(LIBRARY_SUFFIX "${LIBRARY_SUFFIX}d")
    endif()
endif()

set(LIBXML2_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBXML2_INCLUDE_DIR ${INSTALL_DIR}/include/libxml2 CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBXML2_LIBRARY libxml2::libxml2${LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBXML2_LIBRARY_FILE ${INSTALL_DIR}/lib/libxml2${LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBXML2_DEFINITIONS LIBXML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
