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

set(PACKAGE_NAME OpenSSL)
set(PACKAGE_VERSION 1.1.1l)
set(PACKAGE_REPOSITORY openssl)
set(GIT_TAG OpenSSL_1_1_1l)
set(RELEASE_TAG OpenSSL_1_1_1l-libopencor)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

if(WIN32)
    set(CRYPTO_STATIC_LIBRARY lib/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(SSL_STATIC_LIBRARY lib/libssl${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(CRYPTO_STATIC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}crypto${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(SSL_STATIC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}ssl${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(SHA1_FILES
    ${CRYPTO_STATIC_LIBRARY}
    ${SSL_STATIC_LIBRARY}
)

# Either retrieve or build OpenSSL.

if(LIBOPENCOR_PREBUILT_OPENSSL)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             2396d2043778d84fd6d9370da0e069255aa74ee7
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 16ea3b7ba3fc198efd4457c70f0b7ff9983fe90b
                                         00855e72eeb37584b3c396192941a938fd19b063)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             2f2bdd45270d39e72aad5fc8ac15f52dcb91abf7
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES b5267fa182f655a35d6ee4d7b4e536515672e00f
                                         2ff863091fc65738f50e604d2c00244e3acf9313)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         dc863a1aa7b310c6ab27d1b740f7d50838e85086
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES ee1b0d58ed6dd8be5c7014e0d4517dd376458428
                                     4da4b3295ff7e8c47be5b66c1b1814e0db1fd617)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         9dc97c29174509c42636858631ef5a7908c87553
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 592914a5e4315aa37b7500c940f736bc9d6c6a2a
                                     537926b7ce87bcf2a84a4dee0155c11df21c5050)
    endif()
else()
    find_package(Perl REQUIRED QUIET)

    set(CONFIG_OPTIONS no-pic no-shared no-tests no-threads --prefix=${INSTALL_DIR})

    if(WIN32)
        list(APPEND CONFIG_OPTIONS no-asm)

        if(RELEASE_MODE)
            set(PLATFORM VC-WIN64A)
        else()
            set(PLATFORM debug-VC-WIN64A)
        endif()
    else()
        if(APPLE)
            set(PLATFORM darwin64-x86_64-cc)

            list(APPEND CONFIG_OPTIONS "-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
        else()
            set(PLATFORM linux-x86_64)

            list(APPEND CONFIG_OPTIONS "-Wa,--noexecstack")
        endif()
    endif()

    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CONFIGURE_COMMAND
            ${PERL_EXECUTABLE} <SOURCE_DIR>/Configure ${PLATFORM} ${CONFIG_OPTIONS}
        BUILD_COMMAND
            ${MAKE_NMAKE_COMMAND}
        INSTALL_COMMAND
            ${MAKE_NMAKE_COMMAND} install_sw
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${CRYPTO_STATIC_LIBRARY}
            ${INSTALL_DIR}/${SSL_STATIC_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let other third-party libraries find and use OpenSSL.

set(OPENSSL_ROOT_DIR ${INSTALL_DIR} PARENT_SCOPE)
