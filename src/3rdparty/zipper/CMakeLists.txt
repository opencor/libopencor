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

set(PACKAGE_NAME zipper)
set(PACKAGE_VERSION 0.9.1)
set(PACKAGE_REPOSITORY zipper)
set(GIT_TAG v0.9.1)
set(RELEASE_TAG v0.9.1)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

set(ZIPPER_LIBRARY lib/libZipper-static${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${ZIPPER_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_ZIPPER)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             e3cc97b76a186b42de47c3d4d064e7d42e5377da
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 69942417d3029f2b1e33edc7ccfcce36afc98d26)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             aaa78da05fba41b1841fcf27149c14196bb6cbaa
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES adff3fa12e376f4103fd28125bbd578c3ec3ad60)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         2d9fa06079853a44120239f21c976334487ece3c
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 54a00e531d57b23662eceffeee4704870a2f4ce3)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         012f9c4c7af7e093b8bbbdff752ae0f0bd16b1e6
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 90a98c32492466a09e10b0c5fae4f2e64475e3c2)
    endif()
else()
    # Set some compilation flags and/or ignore some warnings

    if(APPLE)
        set(PACKAGE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-implicit-function-declaration")
    endif()

    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_CACHE_ARGS
            -DCMAKE_C_FLAGS:STRING=${PACKAGE_C_FLAGS}
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DBUILD_TEST=OFF
            -DLIBZ_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DLIBZ_LIBRARY=${ZLIB_LIBRARY}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${ZIPPER_LIBRARY}
        DEPENDS
            zlib
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(ZIPPER_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(ZIPPER_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(ZIPPER_LIBRARIES
    ${INSTALL_DIR}/${ZIPPER_LIBRARY}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
