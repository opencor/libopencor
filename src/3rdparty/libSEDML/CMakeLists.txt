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

set(PACKAGE_NAME libSEDML)
set(PACKAGE_VERSION 0.4.4)
set(PACKAGE_REPOSITORY libSEDML)
set(GIT_TAG v0.4.4)
set(RELEASE_TAG v0.4.4-libopencor)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

set(STATIC_LIBRARY lib/libsedml-static${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBSEDML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             15715abd1004b65c2c95f70a615be768a67456ca
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES f0f472585bc09ac8605f8014b3fe8c53214e5727)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                            ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                            fd5dd3ff18a99faee23fad00159d804cc96c7205
                            SHA1_FILES ${SHA1_FILES}
                            SHA1_VALUES e8dbe5b0f96a0a1bea889fef11592e5d620b1f9f)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         646cb5820a6ac5cb9c74345636f753a487081878
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 0de08f9ccea10c8eebf4369abd7d04b1dacd88f8)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         9553b9f350b21db13d4cde2c11fddf1609d34707
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES a9b12cdc27aae2bcb612f7ccf782b5e980da6aa6)
    endif()
else()
    set(EXTRA_LIBS ${LIBXML2_LIBRARY})

    if(WIN32)
        string(APPEND EXTRA_LIBS |WS2_32)
    endif()

    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        LIST_SEPARATOR
            |
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DEXTRA_LIBS=${EXTRA_LIBS}
            -DLIBNUML_INCLUDE_DIR=${LIBNUML_INCLUDE_DIR}
            -DLIBNUML_LIBRARY=${LIBNUML_LIBRARY}
            -DLIBNUML_STATIC=ON
            -DLIBSBML_INCLUDE_DIR=${LIBSBML_INCLUDE_DIR}
            -DLIBSBML_LIBRARY=${LIBSBML_LIBRARY}
            -DLIBSBML_STATIC=ON
            -DLIBSEDML_SHARED_VERSION=OFF
            -DLIBSEDML_SKIP_SHARED_LIBRARY=ON
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DWITH_ZLIB=OFF
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
        DEPENDS
            libNuML
            libSBML
            libxml2
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(LIBSEDML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBSEDML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSEDML_LIBRARY ${INSTALL_DIR}/${STATIC_LIBRARY} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSEDML_DEFINITIONS LIBSEDML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
