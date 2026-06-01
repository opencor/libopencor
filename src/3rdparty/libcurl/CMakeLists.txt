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
                                 fcd780049a9da4bc87d6066f8b7ed94a0669ecb7)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 55ea4f0047efb61808856f9d6d246e9385277336)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 30a4ad9b968829620dcb020b51b916d3c25fd1ba)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 dbaced2020520d73fcec616f6fd364454f98307a)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             0b10f48c0fb7ca822058284c641a0ad4032e1635)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             cc9d70b909e5f8171a4ea901c797d6b6f47ff5b2)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             2fe369235e2623b936ebe8a5c70e2cb84d329b5a)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8cdb9fbc1bd7b29a2cf35abb4717903a53ccf325)
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
