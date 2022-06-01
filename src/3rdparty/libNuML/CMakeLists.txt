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
set(PACKAGE_VERSION 1.1.6)
set(PACKAGE_REPOSITORY NuML)
set(GIT_TAG v1.1.6-libopencor)
set(RELEASE_TAG v1.1.6-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBNUML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             a7f4ed6ae9eae334cab330a0d85893d75c779c74)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             0401ba442e2ae1a69f2d2f4ca8394c275801b5fe)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         0fe9e2ac29205d5a317fe04ac813d21e58563502)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         defc818c8e51e6c0aa1f3ed54a1b6470bdfae4a7)
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
        SOURCE_SUBDIR
            libnuml
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            ${DEXTRA_LIBS}
            -DLIBNUML_SHARED_VERSION=OFF
            -DLIBNUML_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_ROOT=${LIBSBML_CMAKE_DIR}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}numl-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "find_dependency(LIBSBML)" "string(REPLACE \"libNuML\" \"libSBML\" REAL_LIBSBML_CMAKE_DIR \"\${LIBSBML_CMAKE_DIR}\")\nfind_dependency(\${LIBSBML_LIBRARY_NAME} PATHS \${REAL_LIBSBML_CMAKE_DIR})" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    fix_import_prefix(${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}numl-static-targets.cmake)

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(LIBNUML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBNUML_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBNUML_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBNUML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBNUML_LIBRARY ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBNUML_DEFINITIONS LIBNUML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
