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
set(PACKAGE_VERSION 2026-10-04)
set(PACKAGE_REPOSITORY libcellml)
set(RELEASE_TAG 2026-10-04)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

if(WIN32 AND NOT RELEASE_MODE)
    set(DEBUG_TAG d)
endif()

# Provide some CMake configuration files for our copy of libxml2 and zlib.
# Note: libCellML resolves libxml2 and zlib using find_package(), first in config mode, both when building it and when
#       using it (see libcellml-config.cmake). So, we set LibXml2_DIR and ZLIB_DIR to the location of our CMake
#       configuration files so that libCellML is built with and uses our copy of libxml2 and zlib rather than a system
#       copy.

set(LibXml2_DIR ${CMAKE_CURRENT_BINARY_DIR}/LibXml2 CACHE INTERNAL "LibXml2's CMake directory.")
set(ZLIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/ZLIB CACHE INTERNAL "ZLIB's CMake directory.")

file(CONFIGURE OUTPUT ${LibXml2_DIR}/LibXml2Config.cmake
     CONTENT [=[
if(NOT TARGET LibXml2::LibXml2)
    add_library(LibXml2::LibXml2 STATIC IMPORTED)

    set_target_properties(LibXml2::LibXml2 PROPERTIES
                          IMPORTED_LOCATION "@LIBXML2_LIBRARY_FILE@"
                          INTERFACE_COMPILE_DEFINITIONS "@LIBXML2_DEFINITIONS@"
                          INTERFACE_INCLUDE_DIRECTORIES "@LIBXML2_INCLUDE_DIR@")
endif()

set(LIBXML2_INCLUDE_DIRS "@LIBXML2_INCLUDE_DIR@")
]=] @ONLY)

file(CONFIGURE OUTPUT ${ZLIB_DIR}/ZLIBConfig.cmake
     CONTENT [=[
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB STATIC IMPORTED)

    set_target_properties(ZLIB::ZLIB PROPERTIES
                          IMPORTED_LOCATION "@ZLIB_LIBRARY_FILE@"
                          INTERFACE_INCLUDE_DIRECTORIES "@ZLIB_INCLUDE_DIR@")
endif()

set(ZLIB_INCLUDE_DIRS "@ZLIB_INCLUDE_DIR@")
]=] @ONLY)

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
                         cb427d7adee4fd8dea5b6e64954e6c359da9f828)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     e5b249c7c48e1c51855bd27e2fa8324b4b6f11a6)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     10f823b865a20ed44f359eb018b0497693269739)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     5147e233b15725632ff067dda05a7156904cf9ce)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     c2e7c49d0abf0c69314551d5abba89643678e586)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 7c2ace71fa19fe5ce996155c9e83db35c5859ed1)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 c8e860f48a961b028a65210e25c79b8b4f645319)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 1b4483a6a7e2987b15ff6407817c552a7a1c177f)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 e42f2e939b38f3583fc945f1d9540696c91fee95)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our WASM version of libCellML with size optimisation.
    # Note: -Os makes libCellML's contribution to our WASM file noticeably smaller without making it noticeably slower to
    #       load (i.e. parse and validate) or instantiate (i.e. analyse and generate code for) a model, unlike -Oz.

    if(EMSCRIPTEN)
        set(WASM_FLAGS
            "-DCMAKE_CXX_FLAGS_RELEASE=-Os -DNDEBUG"
            "-DCMAKE_C_FLAGS_RELEASE=-Os -DNDEBUG"
        )
    endif()

    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            # https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
            https://github.com/agarny/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            -DBINDINGS_PYTHON=OFF
            -DBUILD_SHARED=OFF
            ${CMAKE_ARGS}
            ${WASM_FLAGS}
            -DCOVERAGE=OFF
            -DINSTALL_PREFIX=${INSTALL_DIR}
            -DLibXml2_DIR=${LibXml2_DIR}
            -DLLVM_COVERAGE=OFF
            -DMEMCHECK=OFF
            -DPYTHON_COVERAGE=OFF
            -DUNIT_TESTS=OFF
            -DZLIB_DIR=${ZLIB_DIR}
    )

    # Patch some of our CMake configuration files.

    if(WIN32)
        set(CONFIG_FILE ${INSTALL_DIR}/${PACKAGE}/CMake/libcellml-targets.cmake)
    else()
        set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${PACKAGE}/libcellml-targets.cmake)
    endif()

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")

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
