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
set(PACKAGE_VERSION 8.20.0)
set(PACKAGE_REPOSITORY curl)
set(RELEASE_TAG curl-8_20_0)
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
                                 80e2348e1090c231eec6f4da92255407c33e67cd)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 073da3f3cabb3b0488729703fb65df21c8310a49)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 48a1a3ac31bf06d3c80fde0d23084e0595d79039)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 23b008d23fd6ff06bf86b8f76aa5aad8a4c91b12)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             0bf402a70a3e06a63fa829ce144c47e2bfddf92c)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             58f53a31867673b59ede4f3474e91a55ceff204d)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             45f3aac9d9b0ae5e61407a024fe8a6ba62a36ef0)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             01e0159b3198e11b040f00c8d48b9659f65bb10c)
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
            -DBUILD_LIBCURL_DOCS=OFF
            -DBUILD_TESTING=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCURL_DISABLE_LDAP=ON
            -DCURL_USE_LIBPSL=OFF
            -DCURL_USE_OPENSSL=ON
            -DENABLE_CURL_MANUAL=OFF
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

    string(REPLACE "CURL::libssh2" "${LIBSSH2_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "CURL::OpenSSL_Crypto" "${OPENSSL_LIBCRYPTO_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "CURL::OpenSSL_SSL" "${OPENSSL_LIBSSL_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "CURL::ZLIB" "${ZLIB_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "Threads::Threads" "" FILE_CONTENTS "${FILE_CONTENTS}")
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
