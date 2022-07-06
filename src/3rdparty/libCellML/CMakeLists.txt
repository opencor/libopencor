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
set(PACKAGE_VERSION 0.2.0)
set(PACKAGE_REPOSITORY libcellml)
set(GIT_TAG 2022-07-05)
set(RELEASE_TAG 2022-07-05)
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
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             40602d38b14f28aa50c85ea27687fe7852bc4e00)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             be92b9c44767b1cbe286af13f8e8e8b25791fe7d)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         bf23afac309ce45efb7e8adc89fa11adfcdc534d)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         cd2b7b09ba3357d1c68b2f03efa0855712cb84c4)
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_ARGS
            -DBINDINGS_PYTHON=OFF
            -DBUILD_SHARED=OFF
            ${CMAKE_ARGS}
            -DCOVERAGE=OFF
            -DINSTALL_PREFIX=${INSTALL_DIR}
            -DLIBXML2_INCLUDE_DIR=${LIBXML2_INCLUDE_DIR}
            -DLIBXML2_LIBRARY=${LIBXML2_LIBRARY_FILE}
            -DLLVM_COVERAGE=OFF
            -DMEMCHECK=OFF
            -DPYTHON_COVERAGE=OFF
            -DUNIT_TESTS=OFF
            -DZLIB_ROOT=${ZLIB_ROOT}
    )

    # Patch some of our CMake configuration files.

    if(WIN32)
        set(CONFIG_FILE ${INSTALL_DIR}/${PACKAGE}/CMake/libcellml-targets.cmake)
    else()
        set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${PACKAGE}/libcellml-targets.cmake)
    endif()

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${PREBUILT_DIR}" "\${_IMPORT_PREFIX}/.." FILE_CONTENTS "${FILE_CONTENTS}")

    if(RELEASE_MODE)
        string(REPLACE ";\\$<LINK_ONLY:\\$<\\$<CONFIG:DEBUG>:cellml_debug_utilities>>" "" FILE_CONTENTS "${FILE_CONTENTS}")
    endif()

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    # Create our package.

    if(WIN32)
        # The below is a temporary fix for the debug version of libCellML on
        # Windows (see https://github.com/cellml/libcellml/issues/980).

        if(NOT RELEASE_MODE)
            set(DEBUG_UTILITIES_LIBRARY cellml_debug_utilities.lib)

            file(COPY ${CMAKE_CURRENT_BINARY_DIR}/build/${PACKAGE_NAME}-prefix/src/${PACKAGE_NAME}-build/src/${DEBUG_UTILITIES_LIBRARY}
                 DESTINATION ${INSTALL_DIR}/lib)
        endif()
    endif()

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   ${CMAKE_DIR} include lib)
endif()

# Make our package accessible.

set(LIBCELLML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBCELLML_CMAKE_DIR ${INSTALL_DIR}/${CMAKE_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBCELLML_CMAKE_PACKAGE_NAME ${PACKAGE_NAME} CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBCELLML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCELLML_LIBRARY cellml CACHE INTERNAL "${PACKAGE_NAME}'s library.")

if(WIN32 AND NOT RELEASE_MODE)
    # The below is a temporary fix for the debug version of libCellML on Windows
    # (see https://github.com/cellml/libcellml/issues/980).

    file(COPY ${INSTALL_DIR}/lib/${DEBUG_UTILITIES_LIBRARY}
         DESTINATION ${CMAKE_BINARY_DIR})
endif()
