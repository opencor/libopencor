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

set(PACKAGE_NAME libCOMBINE)
set(PACKAGE_VERSION 0.2.13b)
set(PACKAGE_REPOSITORY libCombine)
set(GIT_TAG v0.2.13b)
set(RELEASE_TAG v0.2.13b)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBCOMBINE)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             c89d331a2b830f3adf1821851f17ca80bf840008)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             eff165b52343d540b9e9fa9c7fc5e47deb41113a)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         bceb0f1f3169d7228b88d9644238e119c6cd94f1)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         f47de2193b3ff9b333ef6d25b54c6e4ee905c948)
    endif()
else()
    # Build our package.

    set(EXTRA_LIBS ${LIBXML2_LIBRARY_FILE})

    if(WIN32)
        string(APPEND EXTRA_LIBS |WS2_32)
    endif()

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        LIST_SEPARATOR
            |
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCMAKE_POLICY_DEFAULT_CMP0074:STRING=NEW
            -DBUILD_TEST=OFF
            -DEXTRA_LIBS=${EXTRA_LIBS}
            -DLIBCOMBINE_SHARED_VERSION=OFF
            -DLIBCOMBINE_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_ROOT=${LIBSBML_CMAKE_DIR}
            -DLIBSBML_STATIC=ON
            -DLIBZ_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DLIBZ_LIBRARY=${ZLIB_LIBRARY_FILE}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DZIPPER_INCLUDE_DIR=${ZIPPER_INCLUDE_DIR}
            -DZIPPER_LIBRARY=${ZIPPER_LIBRARY}
    )

    # Patch some of our CMake configuration files (see
    # https://github.com/sbmlteam/libCombine/issues/49).

    if(RELEASE_MODE)
        set(PACKAGE_MODE release)
    else()
        set(PACKAGE_MODE debug)
    endif()

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}combine-static-config-${PACKAGE_MODE}.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${ZIPPER_LIBRARY_FILE}" "${ZIPPER_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "${ZLIB_LIBRARY_FILE}" "${ZLIB_LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    fix_import_prefix(${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}combine-static-config.cmake)

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(LIBCOMBINE_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBCOMBINE_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBCOMBINE_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}combine-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBCOMBINE_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCOMBINE_LIBRARY ${LIBRARY_PREFIX}combine-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBCOMBINE_DEFINITIONS LIBCOMBINE_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
