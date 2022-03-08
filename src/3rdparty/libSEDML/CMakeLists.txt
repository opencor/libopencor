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
set(PACKAGE_VERSION 2.0.29)
set(PACKAGE_REPOSITORY libSEDML)
set(GIT_TAG v2.0.29)
set(RELEASE_TAG v2.0.29)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBSEDML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             637fb779f1fdc33cfa9473801d04fc1c5d0117ab)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             0337f1c756bebd4338585e606f75f404a21d3d79)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         e771693f8c3aacf8298ca4b63b510007ff58ab8d)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         984335b4205c66591dd733d615977deefd100644)
    endif()
else()
    # Build our package.

    set(EXTRA_LIBS ${LIBXML2_LIBRARY_FILE})

    if(WIN32)
        string(APPEND EXTRA_LIBS |WS2_32)
    endif()

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        LIST_SEPARATOR
            |
        CMAKE_ARGS
            -DBUILD_TESTS=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DCMAKE_POLICY_DEFAULT_CMP0074:STRING=NEW
            -DEXTRA_LIBS=${EXTRA_LIBS}
            -DLIBNUML_ROOT=${LIBNUML_CMAKE_DIR}
            -DLIBNUML_STATIC=ON
            -DLIBSBML_ROOT=${LIBSBML_CMAKE_DIR}
            -DLIBSBML_STATIC=ON
            -DLIBSEDML_SHARED_VERSION=OFF
            -DLIBSEDML_SKIP_SHARED_LIBRARY=ON
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DWITH_ZLIB=OFF
    )

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(LIBSEDML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBSEDML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSEDML_LIBRARY libSEDML::libsedml-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSEDML_DEFINITIONS LIBSEDML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
