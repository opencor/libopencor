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
set(PACKAGE_VERSION 2.9.10)
set(PACKAGE_REPOSITORY libxml2)
set(GIT_TAG v2.9.10-1)
set(RELEASE_TAG v2.9.10-1)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

if(WIN32)
    set(OS_TAG s)

    if(NOT RELEASE_MODE)
        set(DEBUG_TAG d)
    endif()
endif()

set(STATIC_LIBRARY lib/libxml2${OS_TAG}${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBXML2)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             1858ce703a3c1e17e7cd1619ce6944eaa60c4185
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 494503b9903db9cf7f010114f346dcd34250d63b)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             79b036176a032cc0239ecdfb47640732d2f5603c
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 99dc4576ce1d946d280bef7c7b7f82c3b30f73a4)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         06178306e9fcb9b0ac4981fddc2d0aa7f3b7fec0
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES bf3c4512630a1621697cbe30ce9a09f95dcec398)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         ec304220c2c61d5cbdc0bc038d0a8cfd990cb5f9
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 6a3f95fdd5291eebebf72db991b635fca622f9aa)
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
            -DBUILD_SHARED_LIBS=OFF
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBXML2_WITH_DEBUG=OFF
            -DLIBXML2_WITH_ICONV=OFF
            -DLIBXML2_WITH_LEGACY=OFF
            -DLIBXML2_WITH_LZMA=OFF
            -DLIBXML2_WITH_MODULES=OFF
            -DLIBXML2_WITH_PROGRAMS=OFF
            -DLIBXML2_WITH_PYTHON=OFF
            -DLIBXML2_WITH_TESTS=OFF
            -DLIBXML2_WITH_ZLIB=OFF
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(LIBXML2_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBXML2_INCLUDE_DIR ${INSTALL_DIR}/include/libxml2 CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBXML2_LIBRARIES
    ${INSTALL_DIR}/${STATIC_LIBRARY}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
