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
set(PACKAGE_VERSION 3.0.14)
set(PACKAGE_REPOSITORY openssl)
set(RELEASE_TAG openssl-3.0.14)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_OPENSSL)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 c46f9ca66d99d452698f9309927d63219972a089)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 b8b2de7684995a8c02c272e06031b70513793d6b)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 b51022bad09ebc69618eb03f5c80a590564eb2c5)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 369d4639814f3e76357e190e55dc0cb106dc535f)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             61e6dfe3d85f4ce899e07d7c8f998221190f4fb9)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             71f49e49ce8e2612ca0ee1792be3863807f47e86)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             d5eaa89b18a7e7adce8b641488a6958827fba261)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             af12c894dc1767cae060d925893e777ecf1e439a)
        endif()
    endif()
else()
    # Build our package.

    find_package(Perl REQUIRED QUIET)

    set(CONFIG_OPTIONS no-shared no-tests no-threads --prefix=${INSTALL_DIR})

    if(WIN32)
        list(APPEND CONFIG_OPTIONS no-asm)

        if(RELEASE_MODE)
            if(INTEL_MODE)
                set(PLATFORM VC-WIN64A)
            else()
                set(PLATFORM VC-WIN64-ARM)
            endif()
        else()
            if(INTEL_MODE)
                set(PLATFORM debug-VC-WIN64A)
            else()
                set(PLATFORM debug-VC-WIN64-ARM)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            set(PLATFORM darwin64-x86_64-cc)
        else()
            set(PLATFORM darwin64-arm64-cc)
        endif()

        list(APPEND CONFIG_OPTIONS "-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
    else()
        if(INTEL_MODE)
            set(PLATFORM linux-x86_64)
        else()
            set(PLATFORM linux-aarch64)
        endif()

        list(APPEND CONFIG_OPTIONS "-Wa,--noexecstack")
    endif()

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CONFIGURE_COMMAND
            ${PERL_EXECUTABLE} <SOURCE_DIR>/Configure ${PLATFORM} ${CONFIG_OPTIONS}
        ${PATCH_COMMANDS}
        BUILD_COMMAND
            ${MAKE_NMAKE_COMMAND}
        INSTALL_COMMAND
            ${MAKE_NMAKE_COMMAND} install_sw
    )

    # On Linux, we need to rename the library folder from lib64 to lib, but only when building for the Intel
    # architecture.

    if(NOT WIN32 AND NOT APPLE AND INTEL_MODE)
        file(RENAME "${INSTALL_DIR}/lib64" "${INSTALL_DIR}/lib")
    endif()

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include ${STATIC_LIBRARIES})
endif()

# Make our package accessible.
# Note: the order in which we list the OpenSSL libraries in OPENSSL_LIBRARIES should not matter, but as it happens it
#       does when building our Python bindings on Linux. Indeed, if libcrypto is listed before libssl then our Python
#       module will report that SRP_Calc_A is undefined.

set(OPENSSL_ROOT_DIR ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(OPENSSL_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(OPENSSL_LIBRARIES
    OpenSSL::libssl
    OpenSSL::libcrypto
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
set(OPENSSL_LIBCRYPTO_LIBRARY OpenSSL::libcrypto CACHE INTERNAL "${PACKAGE_NAME}'s libcrypto library.")
set(OPENSSL_LIBSSL_LIBRARY OpenSSL::libssl CACHE INTERNAL "${PACKAGE_NAME}'s libssl library.")
set(OPENSSL_LIBCRYPTO_LIBRARY_FILE ${INSTALL_DIR}/lib/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s libcrypto library.")
set(OPENSSL_LIBSSL_LIBRARY_FILE ${INSTALL_DIR}/lib/libssl${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s libssl library.")
