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
set(PACKAGE_VERSION 3.5.7)
set(PACKAGE_REPOSITORY openssl)
set(RELEASE_TAG openssl-3.5.7)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_OPENSSL)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 3a443e06ead4d1dac5227bfa75b62475f8545a29)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 9e9ab6273e4ff1486b12977a52dbdfa34f8d1d4b)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 632c107b9ace83c7cfadb77d11b10aece0e1020e)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 2b8f3c948289b253cda43a4049a3250afb163771)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             5182803956386e5cda0a0d8d2058cef5d2ba84f6)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             3425a6cbc763c62250d59752a6f661f78ca48e81)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             31b94c25bf89208b30afdf024f4061a4be154920)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             5f60d7207b6d5bd12c62771610ed645eb4d5a366)
        endif()
    endif()
else()
    # Build our package.

    if(WIN32)
        # Look for Strawberry Perl since Git for Windows' Perl is incomplete.

        find_program(STRAWBERRY_PERL perl.exe HINTS "C:/Strawberry/perl/bin")

        if(STRAWBERRY_PERL)
            set(PERL_EXECUTABLE "${STRAWBERRY_PERL}")
        else()
            find_package(Perl REQUIRED QUIET)
        endif()

        # Ensure that MSVC's linker takes precedence over any other linker.

        get_filename_component(MSVC_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)

        set(ENV{PATH} "${MSVC_BIN_DIR};$ENV{PATH}")
    else()
        find_package(Perl REQUIRED QUIET)
    endif()

    # Configure OpenSSL.
    # Note: OpenSSL is only used by libcurl, which libOpenCOR only uses to download files over HTTPS. So, we only keep
    #       what a TLS 1.2/1.3 client needs (e.g., RSA, EC, X25519/X448, AES, ChaCha20-Poly1305, SHA-2/3, HKDF, and
    #       ML-KEM) and disable everything else (e.g., the OpenSSL command-line tool, the legacy provider, engines,
    #       deprecated APIs, TLS 1.0/1.1, DTLS, QUIC, error strings, the automatic loading of OpenSSL's configuration
    #       file, and many algorithms that are either obsolete or not used by TLS). This matters since OpenSSL's default
    #       provider references every algorithm that is built, so the linker cannot remove the ones that we don't use.
    # Note: we set --libdir to lib since, on some platforms (e.g., linux-x86_64), OpenSSL would otherwise use lib64 (see
    #       CMAKE_INSTALL_LIBDIR in cmake/packages.cmake).

    set(CONFIG_OPTIONS no-shared no-tests
                       no-apps no-argon2 no-aria no-autoload-config no-bf no-blake2 no-camellia no-cast no-cmp no-cms
                       no-comp no-ct no-deprecated no-des no-docs no-dsa no-dtls no-ec2m no-engine no-err no-gost
                       no-http no-idea no-legacy no-md4 no-mdc2 no-ml-dsa no-module no-nextprotoneg no-ocb no-ocsp
                       no-psk no-quic no-rc2 no-rc4 no-rc5 no-rfc3779 no-rmd160 no-scrypt no-seed no-siphash no-siv
                       no-slh-dsa no-sm2 no-sm3 no-sm4 no-srp no-srtp no-ssl-trace no-ssl3 no-tls-deprecated-ec no-tls1
                       no-tls1_1 no-ts no-ui-console no-whirlpool
                       --libdir=lib
                       --prefix=${INSTALL_DIR})

    if(WIN32)
        list(APPEND CONFIG_OPTIONS no-asm)

        # On ARM, MSVC 2026 may call __chkstk() before saving the link register in a function's prologue, meaning that
        # the function then returns into its own prologue and crashes. This is notably the case for
        # tls_parse_all_extensions(), i.e. every TLS handshake. OpenSSL builds with /Gs0, i.e. it probes the stack in
        # every function with a stack frame, so we restore MSVC's default threshold (i.e. /Gs4096), which avoids the
        # issue.

        if(NOT INTEL_MODE)
            list(APPEND CONFIG_OPTIONS -Gs4096)
        endif()

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

        # Put each function and data in its own section, so that the linker can remove the ones that we don't use (see
        # THIRD_PARTY_C_FLAGS in cmake/packages.cmake).

        list(APPEND CONFIG_OPTIONS "-Wa,--noexecstack" -ffunction-sections -fdata-sections)
    endif()

    # Build OpenSSL for x86-64-v3 on Intel, like our other third-party libraries (see X86_64_V3_COMPILER_FLAGS in
    # src/CMakeLists.txt and cmake/packages.cmake), something that we need to do ourselves since OpenSSL is not built
    # using CMake.
    # Note: OpenSSL's Configure script only accepts compiler options that start with "-", so we replace the leading "/"
    #       of MSVC's options with "-" (which MSVC also accepts).

    if(X86_64_V3_COMPILER_FLAGS)
        separate_arguments(OPENSSL_X86_64_V3_COMPILER_FLAGS UNIX_COMMAND "${X86_64_V3_COMPILER_FLAGS}")

        list(TRANSFORM OPENSSL_X86_64_V3_COMPILER_FLAGS REPLACE "^/" "-")
        list(APPEND CONFIG_OPTIONS ${OPENSSL_X86_64_V3_COMPILER_FLAGS})
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
