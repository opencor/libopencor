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
                         32b705eb70df3bcc2ad4da195e9d1004ac6965e3)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     8fc953ba4b8878ba8fc8d7653d1fc761e3afc445)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     966cdcb24b529e09a3b479412e8afd1f867b538f)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     21716d691f64023295c53352282534d61f175f72)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     114c3f6216eec5d235ad77f6c61ac5050c883cbb)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 897e91edab0afb593cf34a16af35c2a4eae3f9cd)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 7a4c8a9460d63c04af063458b1ca73c4b564eb2c)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 c43d6334ccaf5a89fd552ddb455dc40f5083b16f)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 f96849b82c76df295dd44c962034717561216b20)
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
