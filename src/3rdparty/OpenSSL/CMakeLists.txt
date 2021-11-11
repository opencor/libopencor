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
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/OpenSSL)

if(WIN32)
    set(CRYPTO_STATIC_LIBRARY lib/libcrypto_static${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(SSL_STATIC_LIBRARY lib/libssl_static${CMAKE_STATIC_LIBRARY_SUFFIX})
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
            message(">>> Retrieve the OpenSSL package...")
        else()
            message(">>> Retrieve the OpenSSL package...")
        endif()
    elseif(APPLE)
        message(">>> Retrieve the OpenSSL package...")
    else()
        message(">>> Retrieve the OpenSSL package...")
    endif()
else()
    find_package(Perl REQUIRED QUIET)

    set(CONFIG_OPTIONS no-pic no-shared no-threads --prefix=${INSTALL_DIR})

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
