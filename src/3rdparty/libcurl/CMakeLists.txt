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

set(PACKAGE_NAME libcurl)
set(PACKAGE_VERSION 8.13.0)
set(PACKAGE_REPOSITORY curl)
set(RELEASE_TAG curl-8_13_0)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

if(WIN32 AND NOT RELEASE_MODE)
    set(DEBUG_TAG -d)
endif()

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBCURL)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 1a4dac12be56cc76bf6a8cca5164aae84b321b49)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 94794fa0d700f2a6956cc5fd8c078f1fbbd4383e)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 17c73519bb0d26e044c24f23564f9fbe9e2103f0)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 4019e7cb2ffd8b48b8bf64aa01af5089918bae32)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             7b919a6395c708515d13529f3b3fc1727aea5811)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             2b397985f476be4095515a0b7ce20549342f2c47)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             63e3713ca2b882bf8acbd5eea4baafac965e2d7f)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             c9d42bb0debb3ba4fbf6674b2c63bffd48cc088d)
        endif()
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            -DCURL_BROTLI=OFF
            -DCURL_ZSTD=OFF
            -DBUILD_CURL_EXE=OFF
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_TESTING=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCURL_USE_LIBPSL=OFF
            -DCURL_USE_OPENSSL=ON
            -DLIBSSH2_INCLUDE_DIR=${LIBSSH2_INCLUDE_DIR}
            -DLIBSSH2_LIBRARY=${LIBSSH2_LIBRARY_FILE}
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
            -DUSE_LIBIDN2=OFF
            -DUSE_NGHTTP2=OFF
            -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DZLIB_LIBRARY=${ZLIB_LIBRARY_FILE}
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/CURL/CURLConfig.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "find_dependency" "# find_dependency" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/CURL/CURLTargets.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${LIBSSH2_LIBRARY_FILE}" "${LIBSSH2_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "OpenSSL::Crypto" "${OPENSSL_LIBCRYPTO_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "OpenSSL::SSL" "${OPENSSL_LIBSSL_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "ZLIB::ZLIB" "${ZLIB_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")

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

set(LIBCURL_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBCURL_CMAKE_DIR ${INSTALL_DIR}/lib/cmake/CURL CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBCURL_CMAKE_PACKAGE_NAME CURL CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBCURL_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCURL_LIBRARY CURL::libcurl CACHE INTERNAL "${PACKAGE_NAME}'s library.")
