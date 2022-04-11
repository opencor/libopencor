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
set(PACKAGE_VERSION 0.2.15)
set(PACKAGE_REPOSITORY libCombine)
set(GIT_TAG v0.2.15)
set(RELEASE_TAG v0.2.15)
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
                             123a68d8022c114a370f6a17cc48e03be928d61d)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             d2c2298f822bc2bd08f37b7412f13d30d6011de5)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         3d8dadeb3e65fe8c030eb81c4c66e9a7e2c56798)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         ebdd0bfe0b04b680a9b33450fefbf9e90ed1a524)
    endif()
else()
    # Build our package.

    if(WIN32)
        set(DEXTRA_LIBS WS2_32)
    endif()

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCMAKE_POLICY_DEFAULT_CMP0074:STRING=NEW
            -DBUILD_TEST=OFF
            ${DEXTRA_LIBS}
            -DLIBCOMBINE_SHARED_VERSION=OFF
            -DLIBCOMBINE_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_ROOT=${LIBSBML_CMAKE_DIR}
            -DLIBZ_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DLIBZ_LIBRARY=${ZLIB_LIBRARY_FILE}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DZIPPER_INCLUDE_DIR=${ZIPPER_INCLUDE_DIR}
            -DZIPPER_LIBRARY=${ZIPPER_LIBRARY_FILE}
    )

    # Patch some of our CMake configuration files.

    file(GLOB INSTALLED_FILES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}" "${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}/*.cmake")

    foreach(INSTALLED_FILE ${INSTALLED_FILES})
        string(REPLACE "Combine" "combine" NEW_INSTALLED_FILE "${INSTALLED_FILE}")

        file(RENAME ${INSTALLED_FILE} ${NEW_INSTALLED_FILE})
    endforeach()

    if(RELEASE_MODE)
        set(PACKAGE_MODE release)
    else()
        set(PACKAGE_MODE debug)
    endif()

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}combine-static-config-${PACKAGE_MODE}.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REGEX REPLACE "  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE \"[^\"]*\"\n" "" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "Combine" "combine" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}combine-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "Combine" "combine" FILE_CONTENTS "${FILE_CONTENTS}")

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
