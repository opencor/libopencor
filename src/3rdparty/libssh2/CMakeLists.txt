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
set(GIT_TAG libssh2-1.10.0)
set(RELEASE_TAG libssh2-1.10.0)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBSSH2)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             c1d65dcaabb155bd64f14a73f3e3db999460fef2)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             866a43d6c6b6100b5c7d4382c0622c1d74e59882)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         6cc26820b2898c76827aa0914001d2d4945de3d4)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         109b96e388f8ebe968418ed9bdfd850a8dcce6cb)
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
    )

    # Patch some of our CMake configuration files since they reference absolute
    # paths.

    if(RELEASE_MODE)
        set(PACKAGE_MODE release)
    else()
        set(PACKAGE_MODE debug)
    endif()

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${PACKAGE}/Libssh2Config-${PACKAGE_MODE}.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${PACKAGE_NAME}" "OpenSSL" OPENSSL_INSTALL_DIR "${INSTALL_DIR}")
    string(REPLACE
           "${OPENSSL_INSTALL_DIR}/lib/libssl${CMAKE_STATIC_LIBRARY_SUFFIX};${OPENSSL_INSTALL_DIR}/lib/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX}"
           "OpenSSL::libcrypto;OpenSSL::libssl"
           FILE_CONTENTS "${FILE_CONTENTS}")

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
set(LIBSSH2_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBSSH2_CMAKE_PACKAGE_NAME Libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBSSH2_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSSH2_LIBRARY Libssh2::libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSSH2_LIBRARY_FILE ${INSTALL_DIR}/lib/${PACKAGE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
