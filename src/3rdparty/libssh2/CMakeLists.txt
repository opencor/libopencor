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
set(PACKAGE_VERSION 1.11.1)
set(PACKAGE_REPOSITORY libssh2)
set(RELEASE_TAG libssh2-1.11.1)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBSSH2)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 25f2d940a79f8fbe03cc0ae07062c673bcd2f606)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 0f735daadcb69c8e6433b97941cd2d92d8b8af21)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 c4518bc5c088a4a0fe7759a346e54a2c204645f8)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 a7e0984a3900512638807138b6c3bc0537f5e420)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             f87739f5ed57f9ff820e2f5df68d9983d0029d0e)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             9b15a45e6f10ec5cb8061d431a356b622e1968bd)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             19a1a3960b77815b93eeb53f3d04e8ae72dee35a)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             4b4603099b6e843a700edc19229119cc3378b79e)
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
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_STATIC_LIBS=ON
            -DBUILD_TESTING=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCRYPTO_BACKEND=OpenSSL
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
            -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DZLIB_LIBRARY=${ZLIB_LIBRARY_FILE}
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${PACKAGE}/libssh2-targets.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    foreach(LIBRARY zlib)
        string(REPLACE "${PREBUILT_DIR}/${LIBRARY}" "\${_IMPORT_PREFIX}/../${LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    endforeach()

    if(APPLE)
        string(REGEX REPLACE "/Applications/Xcode.*\.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/lib" "$<LINK_ONLY:" FILE_CONTENTS "${FILE_CONTENTS}")
        string(REPLACE ".tbd" ">" FILE_CONTENTS "${FILE_CONTENTS}")
    endif()

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
set(LIBSSH2_CMAKE_PACKAGE_NAME libssh2 CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBSSH2_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSSH2_LIBRARY libssh2::libssh2_static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSSH2_LIBRARY_FILE ${INSTALL_DIR}/lib/${PACKAGE_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
