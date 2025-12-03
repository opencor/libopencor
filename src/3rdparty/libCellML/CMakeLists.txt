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

set(PACKAGE_NAME libCellML)
set(PACKAGE_VERSION 0.6.3)
set(PACKAGE_REPOSITORY libcellml)
set(RELEASE_TAG v0.6.3)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

if(WIN32 AND NOT RELEASE_MODE)
    set(DEBUG_TAG d)
endif()

# Either retrieve or build our package.

if(WIN32)
    set(CMAKE_DIR ${PACKAGE_NAME}/CMake)
else()
    set(CMAKE_DIR lib/cmake)
endif()

if(LIBOPENCOR_PREBUILT_LIBCELLML)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         5bee3b3c508fb274e084468243b43d9f83647beb)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     d277ce028be3806aa2514ba27ec4bed352faf069)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     388a936417a5681fbcf835bdda20e8ed0fbf3382)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     c768d8c59db5ee4a07baa0fc61d7a8ccac4ea6db)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     2fd521abaf83b480e5b07c746ae310be529faa7e)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 055181384ac2e4e942603c6b1f3d0c20f439b523)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 847c024d893e3a1f73172e8f6d2e55136e46dd86)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 f8d847dffd74264a3074e742149dca0ab4c57653)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 674bb536a67cc7e7d4afef553c843ac38d44d43f)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            # https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
            https://github.com/agarny/${PACKAGE_REPOSITORY}/archive/refs/tags/7cc5bea.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            -DBINDINGS_PYTHON=OFF
            -DBUILD_SHARED=OFF
            ${CMAKE_ARGS}
            -DCOVERAGE=OFF
            -DINSTALL_PREFIX=${INSTALL_DIR}
            -DLIBXML2_INCLUDE_DIR=${LIBXML2_INCLUDE_DIR}
            -DLIBXML2_LIBRARY=${LIBXML2_LIBRARY_FILE}
            -DLIBXML2_DEFINITIONS=${LIBXML2_DEFINITIONS}
            -DLLVM_COVERAGE=OFF
            -DMEMCHECK=OFF
            -DPYTHON_COVERAGE=OFF
            -DUNIT_TESTS=OFF
            -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DZLIB_LIBRARY=${ZLIB_LIBRARY_FILE}
    )

    # Patch some of our CMake configuration files.

    if(WIN32)
        set(CONFIG_FILE ${INSTALL_DIR}/${PACKAGE}/CMake/libcellml-targets.cmake)
    else()
        set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${PACKAGE}/libcellml-targets.cmake)
    endif()

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")

    foreach(LIBRARY libxml2 zlib)
        string(REPLACE "${PREBUILT_DIR}/${LIBRARY}" "\${_IMPORT_PREFIX}/../${LIBRARY}" FILE_CONTENTS "${FILE_CONTENTS}")
    endforeach()

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   ${CMAKE_DIR} include lib)
endif()

# Make our package accessible.

set(LIBCELLML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")

if(WIN32)
    set(LIBCELLML_CMAKE_DIR ${INSTALL_DIR}/${CMAKE_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
else()
    set(LIBCELLML_CMAKE_DIR ${INSTALL_DIR}/${CMAKE_DIR}/${PACKAGE_NAME} CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
endif()

set(LIBCELLML_CMAKE_PACKAGE_NAME ${PACKAGE_NAME} CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBCELLML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCELLML_LIBRARY cellml CACHE INTERNAL "${PACKAGE_NAME}'s library.")
