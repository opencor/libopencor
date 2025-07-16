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

set(PACKAGE_NAME libNuML)
set(PACKAGE_VERSION 1.1.7)
set(PACKAGE_REPOSITORY NuML)
set(RELEASE_TAG v1.1.7)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBNUML OR ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         5f3ae380ed2234c11e62cc71739cbb54455d2de9)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     0321c59680dcdf5311e0a255faba6ee15f78ffa0)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     9c4d39e21244dea33f7960f8aaf44d56aa45a519)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     c4870916013bc2f742ba172396ab8e0daec51c18)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     ad6fcc17aeceb4e78c6a3af6918b8bdc58fd8eca)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 5e64ff7762ec28b258ea2c593eede2ddb99ddf60)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 3046b4a41ecf273f9b361f5cf6357b87dc1024ee)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 e48bfad4d787f02e6c32845ad964180be7c17e51)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 2a61a4b5f6982b02f31bb49e955d97e97202b892)
            endif()
        endif()
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        SOURCE_SUBDIR
            libnuml
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBNUML_SHARED_VERSION=OFF
            -DLIBNUML_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_INCLUDE_DIR=${LIBSBML_INCLUDE_DIR}
            -DLIBSBML_LIBRARY=${LIBSBML_LIBRARY_FILE}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}numl-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "LIBSBML_CMAKE_DIR" "LIBNUML_CMAKE_DIR" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "find_dependency" "# find_dependency" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    fix_import_prefix(${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}numl-static-targets.cmake)

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib share/cmake)
endif()

# Make our package accessible.

set(LIBNUML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBNUML_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBNUML_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBNUML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBNUML_LIBRARY ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBNUML_LIBRARY_FILE ${INSTALL_DIR}/lib/libnuml-static${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBNUML_DEFINITIONS LIBNUML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
