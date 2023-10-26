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
set(PACKAGE_VERSION 0.5.0)
set(PACKAGE_REPOSITORY libcellml)
set(RELEASE_TAG v0.5.0)
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

if(LIBOPENCOR_PREBUILT_LIBCELLML OR LIBOPENCOR_JAVASCRIPT_BINDINGS)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         67b8aeda04b896c59a2144e9922fd8bda6e52452)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     a2d2555c60de750dfcf42bf3040ffa903919b5e4)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     f00d15f699dcf253e8d85e91d0965d6ae248323c)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     883cafe9b6e993db2f7ebce5d04a02566e77d0b6)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     32b0f6ed51eb5863fb1f409a1f045ced4fb36f07)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 eb486f76b6d274b0695c2ac7a817754f01560a48)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 cc33300cb65f19d20a05772f625cf7377ad67e14)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 8ee500edbddfc06bf8f5fbdfdb041aef19824ef9)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 97aa4c2b0b046f67a08fab1106fbe698c7b927d5)
            endif()
        endif()
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
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
