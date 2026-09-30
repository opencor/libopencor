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

set(PACKAGE_NAME libSBML)
set(PACKAGE_VERSION 5.21.0)
set(PACKAGE_REPOSITORY libsbml)
set(RELEASE_TAG v5.21.0)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBSBML)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         e82783a6c27de7d7c44c401e8d683d67f879b5c5)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     ab73327428ed5b38595b75a6dc8d4bea276a56ab)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     949588f30b0f6dc6f58dde49d16243cf1e026d8d)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     57acaae652f1d110a24e95165e63e20e1dacef10)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     39f2354f1de60b8546de67bdd165a21ac572203a)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 01a7d0a5e5db7e2ae235a3fe8f8a6cd2c6d66edf)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 3702b328899fcbffe175e9647812cc59a8278568)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 3d55218f7ac8d0d430bce65df0c27a5a058051d0)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 59057477b9f374e8a056135db7ab289525140dff)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.

    # libSBML's FindLIBXML.cmake unconditionally probes ZLIB. Pre-seed the ZLIB variables so that FindZLIB.cmake can
    # succeed even when zlib is not available (on Windows and with Emscripten), while still keeping zlib disabled for
    # libSBML itself.

    set(ZLIB_ARGS
        -DWITH_ZLIB=OFF
        -DZLIB_FOUND=TRUE
        -DZLIB_INCLUDE_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DZLIB_LIBRARY=${CMAKE_CURRENT_BINARY_DIR}/zlib.not.used
    )

    # Apply our patches to libSBML's source code (see README.md).

    patch_package_args(${PACKAGE_NAME} ${RELEASE_TAG} LIBSBML_PATCH_ARGS)

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        ${LIBSBML_PATCH_ARGS}
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBSBML_SHARED_VERSION=OFF
            -DLIBSBML_SKIP_SHARED_LIBRARY=ON
            -DLIBXML_INCLUDE_DIR=${LIBXML2_INCLUDE_DIR}
            -DLIBXML_LIBRARY=${LIBXML2_LIBRARY_FILE}
            -DWITH_BZIP2=OFF
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            ${ZLIB_ARGS}
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sbml-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REGEX REPLACE "foreach \\\(library .* \\\)" "foreach (library)" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sbml-static-targets.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REGEX REPLACE "INTERFACE_LINK_LIBRARIES \\\"[^\\\"]*\\\"" "INTERFACE_LINK_LIBRARIES \"\"" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib/cmake ${STATIC_LIBRARIES})
endif()

# Make our package accessible.

set(LIBSBML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBSBML_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBSBML_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}sbml-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBSBML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSBML_LIBRARY ${LIBRARY_PREFIX}sbml-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSBML_LIBRARY_FILE ${INSTALL_DIR}/lib/libsbml-static${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSBML_DEFINITIONS LIBSBML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
