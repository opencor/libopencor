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
                                 63dc0994e64621bcb2f5b4aabf28606340ade99b)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 372fe0ff3c9ae2af3015b65b75a4955f81b99f41)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 9f769ee4d69ffe4d643dfdc1740d1d5a16462778)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 c5044b65f97a74720b3580a75a61edd780982d31)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             457032b44cd2d305333c89e353c907caceaaf249)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             baeed8362a8b565d7b925bb227690fa5b7afd9c1)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             179f6fc8af9b65e100924aeb093ae3367a0f9580)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             81628ad13ec645ad5cad1b130325599f29ffaecc)
        endif()
    endif()
else()
    # Build our package.
    # Note #1: libOpenCOR only uses libcurl to download files over HTTP(S) (following redirects, if needed), so we only
    #          build its HTTP(S) support and disable the features that we don't use (e.g., cookies, HSTS, DoH, MIME, and
    #          most authentication methods). We don't need zlib either since we never ask for compressed content.
    #          However, we keep proxy support (and Basic authentication) since libcurl honours the http_proxy,
    #          https_proxy, and no_proxy environment variables, which some users may rely on.
    # Note #2: we prepend our OpenSSL include directory to CMAKE_C_FLAGS so that it comes before any other include
    #          directory. Otherwise, on macOS (Intel), libcurl gets compiled against Homebrew's OpenSSL headers (in
    #          /usr/local/include), which are configured differently from our OpenSSL (e.g., with engine and OCSP
    #          support), and linking against our OpenSSL libraries then fails.

    if(THIRD_PARTY_C_FLAGS)
        set(LIBCURL_C_FLAGS ${THIRD_PARTY_C_FLAGS})
    else()
        set(LIBCURL_C_FLAGS ${CMAKE_C_FLAGS_INIT})
    endif()

    string(STRIP "-I${OPENSSL_INCLUDE_DIR} ${LIBCURL_C_FLAGS}" LIBCURL_C_FLAGS)

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
            -DCMAKE_C_FLAGS=${LIBCURL_C_FLAGS}
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

    string(REPLACE "CURL::OpenSSL_Crypto" "${OPENSSL_LIBCRYPTO_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "CURL::OpenSSL_SSL" "${OPENSSL_LIBSSL_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
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
