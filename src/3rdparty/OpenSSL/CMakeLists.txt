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
set(PACKAGE_VERSION 3.5.0)
set(PACKAGE_REPOSITORY openssl)
set(RELEASE_TAG openssl-3.5.0)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_OPENSSL)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 ac4440ae3dba3415322fc833bef87b8b4d1b082b)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 cd571c829ce744dbfc82bb1998f4d566b289cf93)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 ed9489d5ed226ed79ac2d1042a4a22bcdc899a8c)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 dec6518c275c9f72aeeea068328ac86f0b87fb82)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             67f37a19f38abf06220619074ee880ca63b4a804)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8c5d0afbbd2dfe2120502b728e8e5e9ff8f69ecc)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             934a9c39cd923b2466f83066952b17295b8dadf5)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             cc6a2de4b58b0b717e0684534c17ba5c90e6e7ab)
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

    if(WIN32 AND NOT INTEL_MODE)
        #---GRY--- THE BELOW SHOULD BE REMOVED ONCE THE OpenSSL ISSUE ON WoA HAS BEEN FIXED.

        # There is currently an issue with building OpenSSL on Windows on ARM, which results in libcurl segfaulting
        # whenever we try to use it. A temporary fix is to build OpenSSL without optimisation. We can do this by
        # setting the CFLAGS environment variable. By default, the flags passed to the C compiler are
        # "/W3 /wd4090 /nologo /O2", so here we set CFLAGS to "/W3 /wd4090 /nologo". See
        # https://github.com/openssl/openssl/issues/26239 and
        # https://developercommunity.visualstudio.com/t/Openssl-crashes-on-arm64-when-built-with/10896571 for more
        # details.

        string(REPLACE " " "\\ " ESCAPED_CMAKE_COMMAND ${CMAKE_COMMAND})
        string(REPLACE " " "\\ " ESCAPED_CFLAGS "/W3 /wd4090 /nologo")

        set(PRE_CONFIGURE_COMMAND ${ESCAPED_CMAKE_COMMAND} -E env CFLAGS=${ESCAPED_CFLAGS})
    endif()

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CONFIGURE_COMMAND
            ${PRE_CONFIGURE_COMMAND} ${PERL_EXECUTABLE} <SOURCE_DIR>/Configure ${PLATFORM} ${CONFIG_OPTIONS}
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
