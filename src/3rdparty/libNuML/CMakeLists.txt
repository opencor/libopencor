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
set(PACKAGE_VERSION 1.1.4)
set(PACKAGE_REPOSITORY NuML)
set(GIT_TAG v1.1.4-libopencor)
set(RELEASE_TAG v1.1.4-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBNUML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             593d2cb56b33efa28c63eeca58848e60e46e849c)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8eb1e5391a0c5bebc1bb865a8e450f005bf3bd85)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         afcefed644c01fb7b500c5b9ff1d2d69226e9dce)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         092a4eba9c06dfee03ece8ef006d903f05b12034)
    endif()
else()
    # Build our package.

    set(EXTRA_LIBS ${LIBXML2_LIBRARY})

    if(WIN32)
        string(APPEND EXTRA_LIBS |WS2_32)
    endif()

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        SOURCE_SUBDIR
            libnuml
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        LIST_SEPARATOR
            |
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DEXTRA_LIBS=${EXTRA_LIBS}
            -DLIBNUML_SHARED_VERSION=OFF
            -DLIBNUML_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_INCLUDE_DIR=${LIBSBML_INCLUDE_DIR}
            -DLIBSBML_LIBRARY=${LIBSBML_LIBRARY}
            -DLIBSBML_STATIC=ON
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
    )

    # Patch one of our CMake configuration files (see
    # https://github.com/NuML/NuML/issues/24).

    if(WIN32)
        set(LIBRARY_PREFIX lib)
    endif()

    fix_import_prefix(${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}numl-static-config.cmake)

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

set(LIBNUML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBNUML_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBNUML_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBNUML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBNUML_LIBRARY ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBNUML_DEFINITIONS LIBNUML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
