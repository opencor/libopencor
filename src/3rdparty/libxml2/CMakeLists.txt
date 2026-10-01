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

set(PACKAGE_NAME libxml2)
set(PACKAGE_VERSION 2.9.14)
set(PACKAGE_REPOSITORY libxml2)
set(RELEASE_TAG v2.9.14)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBXML2)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         2ffe324bacff69797be5370476c33f179d0a18b9)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     57954ad0fab2e06453a8cd81bc9ef1d84b918e38)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     6b1717d8dcf585acf2e521f57debf92c4637a16d)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     2708c5b98762b797fe64764f18af699542e8a7c0)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     67b654292b5b267e83cb735c21bd879d2efdbc85)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 72ba239ddb572251c5d14ede6997aec80b52527c)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 ce4e88957c21e39eef300d36227170524b375bdb)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 ecaaf18a6ec7995d9a2ed0a5bc86225c8251f366)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 e3d956440b5a571c937c3e3b75cd527cf8ffdf96)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.
    # Note: libCellML, libSBML, and libCOMBINE only need libxml2's tree, parsing (incl. push parsing), serialisation,
    #       and DTD validation support (the latter needs regular expressions support), so we disable everything else
    #       (e.g., HTML, XPath, XInclude, schemas, catalogs, FTP/HTTP support, the older SAX1 interface, and the
    #       built-in ISO-8859-2 to ISO-8859-16 encodings, UTF-8, UTF-16, and ISO-8859-1 being always supported).

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBXML2_WITH_C14N=OFF
            -DLIBXML2_WITH_CATALOG=OFF
            -DLIBXML2_WITH_DEBUG=OFF
            -DLIBXML2_WITH_DOCB=OFF
            -DLIBXML2_WITH_FTP=OFF
            -DLIBXML2_WITH_HTML=OFF
            -DLIBXML2_WITH_HTTP=OFF
            -DLIBXML2_WITH_ICONV=OFF
            -DLIBXML2_WITH_ISO8859X=OFF
            -DLIBXML2_WITH_LEGACY=OFF
            -DLIBXML2_WITH_LZMA=OFF
            -DLIBXML2_WITH_MODULES=OFF
            -DLIBXML2_WITH_PATTERN=OFF
            -DLIBXML2_WITH_PROGRAMS=OFF
            -DLIBXML2_WITH_PYTHON=OFF
            -DLIBXML2_WITH_READER=OFF
            -DLIBXML2_WITH_SAX1=OFF
            -DLIBXML2_WITH_SCHEMAS=OFF
            -DLIBXML2_WITH_SCHEMATRON=OFF
            -DLIBXML2_WITH_TESTS=OFF
            -DLIBXML2_WITH_WRITER=OFF
            -DLIBXML2_WITH_XINCLUDE=OFF
            -DLIBXML2_WITH_XPATH=OFF
            -DLIBXML2_WITH_XPTR=OFF
            -DLIBXML2_WITH_ZLIB=OFF
    )

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include ${STATIC_LIBRARIES})
endif()

# Make our package accessible.

if(WIN32)
    set(LIBRARY_SUFFIX s)

    if(NOT RELEASE_MODE)
        set(LIBRARY_SUFFIX "${LIBRARY_SUFFIX}d")
    endif()
endif()

set(LIBXML2_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBXML2_INCLUDE_DIR ${INSTALL_DIR}/include/libxml2 CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBXML2_LIBRARY libxml2::libxml2${LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBXML2_LIBRARY_FILE ${INSTALL_DIR}/lib/libxml2${LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBXML2_DEFINITIONS LIBXML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
