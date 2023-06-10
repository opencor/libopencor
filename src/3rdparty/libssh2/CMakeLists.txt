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

set(PACKAGE_NAME libssh2)
set(PACKAGE_VERSION 1.10.0)
set(PACKAGE_REPOSITORY libssh2)
set(RELEASE_TAG libssh2-1.10.0)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBSSH2 OR LIBOPENCOR_JAVASCRIPT_BINDINGS)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 66a45d6d04addac8c38cceb46abf1ec236235f04)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 467f84667ba3cabbb0b496f6b4cbdbe286779917)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 d7d0639a87c7cab2c140d197e8a469786b6b0b6d)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 9448c2d7345bed06dffd5d8c78e17158eca417e6)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             555726b2220a89d3de8c405297d2898311ff415e)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             52a3363adaeeccf94116e689af1ed541bfa14aaa)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             748feccb81c12663f28023b171925ee255ed3acc)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             19e3ffa73e95969fa13beb7f8a37fecf4d595b6b)
        endif()
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            -DBUILD_EXAMPLES=OFF
            -DBUILD_TESTING=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_LIBCRYPTO_LIBRARY_FILE}
            -DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}
            -DOPENSSL_SSL_LIBRARY=${OPENSSL_LIBSSL_LIBRARY_FILE}
    )

    # Patch some of our CMake configuration files.

    if(RELEASE_MODE)
        set(PACKAGE_MODE release)
    else()
        set(PACKAGE_MODE debug)
    endif()

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${PACKAGE}/Libssh2Config-${PACKAGE_MODE}.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${OPENSSL_LIBCRYPTO_LIBRARY_FILE}" "${OPENSSL_LIBCRYPTO_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "${OPENSSL_LIBSSL_LIBRARY_FILE}" "${OPENSSL_LIBSSL_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib/cmake ${STATIC_LIBRARIES})
endif()

# Make our package accessible.

set(LIBSSH2_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBSSH2_CMAKE_DIR ${INSTALL_DIR}/lib/cmake/libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBSSH2_CMAKE_PACKAGE_NAME Libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBSSH2_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSSH2_LIBRARY Libssh2::libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSSH2_LIBRARY_FILE ${INSTALL_DIR}/lib/${PACKAGE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
