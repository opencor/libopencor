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
set(PACKAGE_VERSION 2.15.4)
set(PACKAGE_REPOSITORY libxml2)
set(RELEASE_TAG v2.15.4)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBXML2)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         d755e638bc31e3dd07ffa30e309a6ad57f0dd0c6)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     90fc907f46f4681341cb5fc0a1cdb31a27fdb8c9)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     57ddbd6460f3eda5a8b05159bc91b1a7cc1aa856)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     5127f23517e220ad1467e8b606e03ee1336a3304)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     5ba67a19782f9a8866213c906d0613c9909f5357)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 fcb8b2943f3ddcbbda5e6ceec50bd817afe11bfb)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 a24568fb8b499380255ae0c97a4814b2b9c848d1)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 6899d9121febe3b8f39045274ee9bbf43ea3225d)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 649d950bd9ea4df97c55bda6179db3c2205b4ffa)
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

# On Windows, libxml2 (2.14+) uses BCryptGenRandom() to seed its random number generator, so anything that links
# against our copy of libxml2 also needs to link against bcrypt.
# Note: this is the case whether our package was built or retrieved, hence we set it here.

if(WIN32)
    set(LIBXML2_LIBRARIES bcrypt CACHE INTERNAL "${PACKAGE_NAME}'s system libraries.")
else()
    set(LIBXML2_LIBRARIES "" CACHE INTERNAL "${PACKAGE_NAME}'s system libraries.")
endif()
