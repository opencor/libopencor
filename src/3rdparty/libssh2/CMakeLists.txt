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
                             6e10dd871c36b6e1ba386ad94c96f12c559c5b31)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             b7237f10ac0814b5ac9404677735d7543553fe07)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         009db278ff067ac04a4432f03580a449b43fc5f2)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         fd0c2b3d7238d7fa891aff992c1e7a5883d09252)
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DBUILD_EXAMPLES=OFF
            -DBUILD_TESTING=OFF
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
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
set(LIBSSH2_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBSSH2_CMAKE_PACKAGE_NAME Libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBSSH2_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSSH2_LIBRARY Libssh2::libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSSH2_LIBRARY_FILE ${INSTALL_DIR}/lib/${PACKAGE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
