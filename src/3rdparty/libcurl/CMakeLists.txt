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
set(PACKAGE_VERSION 7.80.0)
set(PACKAGE_REPOSITORY curl)
set(GIT_TAG curl-7_80_0)
set(RELEASE_TAG curl-7_80_0)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

if(WIN32 AND NOT RELEASE_MODE)
    set(DEBUG_TAG -d)
endif()

set(LIBCURL_LIBRARY lib/libcurl${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${LIBCURL_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBCURL)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             b50dc97d2554b6d684de735e10ad922c8ea9f1f4
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES b48d851d698330786b0f2c5c38db15b4ab4d6523)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             a251be70c7334009c49a12ddbabc85b0c2a5d14c
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 067338ce85a7c64ef7c1489b854ea16c3d120b86)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         880b4d6e06772ef79831b2f2e20bae3eaf2aba13
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES ee52e27f3b1f2de57034437ae1dc8fb91e5f108e)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         7c12dbc93648b8630ee1a2abfb668caa7592b7c4
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES cb35dc824ca1645eee67926f3182304c171d3fc3)
    endif()
else()
    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DBUILD_CURL_EXE=OFF
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_TESTING=OFF
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCMAKE_USE_OPENSSL=ON
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${LIBCURL_LIBRARY}
        DEPENDS
            OpenSSL
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(LIBCURL_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBCURL_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCURL_LIBRARIES
    ${INSTALL_DIR}/${LIBCURL_LIBRARY}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
