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
set(PACKAGE_VERSION 2.0.32)
set(PACKAGE_REPOSITORY libSEDML)
set(GIT_TAG v2.0.32-libopencor)
set(RELEASE_TAG v2.0.32-libopencor)
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
                             4bc7994abe4cee2637841d9717f0781dc804fab8)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             692e61277297fa5065896fa06179531a36692b06)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         94b83bef7b8ee4262edd97e210b368dd736df0b9)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         3a92e274425f9f9eebc9d3a9771b6f2dd2c1c5f8)
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
            -DBUILD_TESTS=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBNUML_ROOT=${LIBNUML_CMAKE_DIR}
            -DLIBSBML_ROOT=${LIBSBML_CMAKE_DIR}
            -DLIBSEDML_SHARED_VERSION=OFF
            -DLIBSEDML_SKIP_SHARED_LIBRARY=ON
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DWITH_ZLIB=OFF
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sedml-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "find_dependency(LIBNUML)" "string(REPLACE \"libSEDML\" \"libNuML\" LIBNUML_CMAKE_DIR \"\${LIBSEDML_CMAKE_DIR}\")\nfind_dependency(\${LIBNUML_LIBRARY_NAME} PATHS \${LIBNUML_CMAKE_DIR})" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "find_dependency(LIBSBML)" "string(REPLACE \"libSEDML\" \"libSBML\" LIBSBML_CMAKE_DIR \"\${LIBSEDML_CMAKE_DIR}\")\nfind_dependency(\${LIBSBML_LIBRARY_NAME} PATHS \${LIBSBML_CMAKE_DIR})" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sedml-static-targets.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REGEX REPLACE "INTERFACE_LINK_LIBRARIES \\\"[^\\\"]*\\\"" "INTERFACE_LINK_LIBRARIES \"\"" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    if(RELEASE_MODE)
        set(PACKAGE_MODE release)
    else()
        set(PACKAGE_MODE debug)
    endif()

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sedml-static-targets-${PACKAGE_MODE}.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${PACKAGE_NAME}" "libNuML" LIBNUML_INSTALL_DIR "${INSTALL_DIR}")
    string(REPLACE "${PACKAGE_NAME}" "libSBML" LIBSBML_INSTALL_DIR "${INSTALL_DIR}")
    string(REPLACE "${PACKAGE_NAME}" "libxml2" LIBXML2_INSTALL_DIR "${INSTALL_DIR}")

    string(REPLACE "${LIBNUML_INSTALL_DIR}" "\${_IMPORT_PREFIX}/../libNuML" RELATIVE_LIBNUML_LIBRARY_FILE "${LIBNUML_LIBRARY_FILE}")
    string(REPLACE "${LIBSBML_INSTALL_DIR}" "\${_IMPORT_PREFIX}/../libSBML" RELATIVE_LIBSBML_LIBRARY_FILE "${LIBSBML_LIBRARY_FILE}")
    string(REPLACE "${LIBXML2_INSTALL_DIR}" "\${_IMPORT_PREFIX}/../libxml2" RELATIVE_LIBXML2_LIBRARY_FILE "${LIBXML2_LIBRARY_FILE}")

    string(REPLACE "IMPORTED_LOCATION_RELEASE" "IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE \"${RELATIVE_LIBNUML_LIBRARY_FILE};${RELATIVE_LIBSBML_LIBRARY_FILE};${RELATIVE_LIBXML2_LIBRARY_FILE}\"\n  IMPORTED_LOCATION_RELEASE" FILE_CONTENTS "${FILE_CONTENTS}")

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
