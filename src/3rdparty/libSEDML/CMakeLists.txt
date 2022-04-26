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

set(PACKAGE_NAME libSEDML)
set(PACKAGE_VERSION 2.0.31)
set(PACKAGE_REPOSITORY libSEDML)
set(GIT_TAG v2.0.31-libopencor)
set(RELEASE_TAG v2.0.31-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBSEDML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             e6e9e64656cb78039e1f93b95a869eb2b54ab8bf)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             0ea8f0d46e00641374515593cb493b0b3a56920c)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         66c3e0582f47c0ec2903ccbb5e3dfcdf08c46c0e)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         73f017250ef2c9d1ca59df2019db2548fcb939db)
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
            -DBUILD_TESTS=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            ${DEXTRA_LIBS}
            -DLIBNUML_ROOT=${LIBNUML_CMAKE_DIR}
            -DLIBSBML_ROOT=${LIBSBML_CMAKE_DIR}
            -DLIBSEDML_SHARED_VERSION=OFF
            -DLIBSEDML_SKIP_SHARED_LIBRARY=ON
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DWITH_ZLIB=OFF
    )

    # Patch some of our CMake configuration files.

    if(RELEASE_MODE)
        set(PACKAGE_MODE release)
    else()
        set(PACKAGE_MODE debug)
    endif()

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sedml-static-config-${PACKAGE_MODE}.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REGEX REPLACE "  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE \"[^\"]*\"\n" "" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib/cmake ${STATIC_LIBRARIES})
endif()

# Make our package accessible.

set(LIBSEDML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBSEDML_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBSEDML_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}sedml-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBSEDML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSEDML_LIBRARY ${LIBRARY_PREFIX}sedml-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSEDML_DEFINITIONS LIBSEDML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
