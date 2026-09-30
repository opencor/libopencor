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
set(PACKAGE_VERSION 8.21.0)
set(PACKAGE_REPOSITORY curl)
set(RELEASE_TAG curl-8_21_0)
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
                                 f0c14f3fd186399a73825a4f6eaf839908978e44)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 17c9c0eddcb2352a62f4fd7d10cee22d665d9cbf)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 86ce1716ab57751f2f05f12f83e61879d1104c2e)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 8f6061699d1b4d7ae235104cd4281e4c17b6c9cd)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             e6acd53caf05a5375cae0fa502ce3b31395f9f32)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             50a548ddf494b307d8abb12bd1ae3471a6b81062)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             7342e5e9eae7a40b6c82e96948d775a0847c8336)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             36cc3f5101c5474c8f1b39a61d3443fc7c1c4032)
        endif()
    endif()
else()
    # Build our package.
    # Note: libOpenCOR only uses libcurl to download files over HTTP(S) (following redirects, if needed), so we only
    #       build its HTTP(S) support and disable the features that we don't use (e.g., cookies, HSTS, DoH, MIME, and
    #       most authentication methods). We don't need zlib either since we never ask for compressed content. However,
    #       we keep proxy support (and Basic authentication) since libcurl honours the http_proxy, https_proxy, and
    #       no_proxy environment variables, which some users may rely on.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            -DCURL_BROTLI=OFF
            -DCURL_ZSTD=OFF
            -DBUILD_CURL_EXE=OFF
            -DBUILD_EXAMPLES=OFF
            -DBUILD_LIBCURL_DOCS=OFF
            -DBUILD_MISC_DOCS=OFF
            -DBUILD_TESTING=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCURL_DISABLE_ALTSVC=ON
            -DCURL_DISABLE_AWS=ON
            -DCURL_DISABLE_BEARER_AUTH=ON
            -DCURL_DISABLE_BINDLOCAL=ON
            -DCURL_DISABLE_COOKIES=ON
            -DCURL_DISABLE_DIGEST_AUTH=ON
            -DCURL_DISABLE_DOH=ON
            -DCURL_DISABLE_FORM_API=ON
            -DCURL_DISABLE_GETOPTIONS=ON
            -DCURL_DISABLE_HEADERS_API=ON
            -DCURL_DISABLE_HSTS=ON
            -DCURL_DISABLE_KERBEROS_AUTH=ON
            -DCURL_DISABLE_LDAP=ON
            -DCURL_DISABLE_MIME=ON
            -DCURL_DISABLE_NEGOTIATE_AUTH=ON
            -DCURL_DISABLE_NETRC=ON
            -DCURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG=ON
            -DCURL_DISABLE_PARSEDATE=ON
            -DCURL_DISABLE_PROGRESS_METER=ON
            -DCURL_DISABLE_SHA512_256=ON
            -DCURL_DISABLE_SRP=ON
            -DCURL_USE_LIBPSL=OFF
            -DCURL_USE_LIBSSH2=OFF
            -DCURL_USE_OPENSSL=ON
            -DCURL_ZLIB=OFF
            -DENABLE_CURL_MANUAL=OFF
            -DENABLE_UNIX_SOCKETS=OFF
            -DHTTP_ONLY=ON
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
            -DUSE_LIBIDN2=OFF
            -DUSE_NGHTTP2=OFF
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/CURL/CURLConfig.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "find_dependency" "# find_dependency" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/CURL/CURLTargets.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "OpenSSL::Crypto" "${OPENSSL_LIBCRYPTO_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "OpenSSL::SSL" "${OPENSSL_LIBSSL_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "Threads::Threads" "" FILE_CONTENTS "${FILE_CONTENTS}")

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
