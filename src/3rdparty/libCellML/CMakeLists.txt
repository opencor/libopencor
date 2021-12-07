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
set(GIT_TAG v0.2.0-dev-libopencor)
set(RELEASE_TAG v0.2.0-dev-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

if(WIN32 AND NOT RELEASE_MODE)
    set(DEBUG_TAG d)
endif()

set(LIBCELLML_LIBRARY lib/libcellml${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${LIBCELLML_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBCELLML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             d326fc1d79813d3d90a6247ce162bbf1b9190715
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES fa36f0a151e29ded0b7f1bad762663c82f43600f)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             cde5ae29c1ac7df3e2dacda7274fdc541e455e98
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 22142c4365215b7af220bab1d89a4f151461fd77)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         fa171be5c631b67d9b0582ccc8220fb2aae0697f
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 7f68d9e07221b93e3a287df29a09fc8e689aea1a)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         3d9b9318f5184d7fbb80092ebeaa0ffe3aabcf04
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES f4a3eb0ceae8ce5987cbc0fc60bd2428952d3741)
    endif()
else()
    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DBINDINGS_PYTHON=OFF
            -DBUILD_SHARED=OFF
            -DCOVERAGE=OFF
            -DINSTALL_PREFIX=${INSTALL_DIR}
            -DLIBXML2_INCLUDE_DIR=${LIBXML2_INCLUDE_DIR}
            -DLIBXML2_LIBRARY=${LIBXML2_LIBRARY}
            -DLLVM_COVERAGE=OFF
            -DMEMCHECK=OFF
            -DPYTHON_COVERAGE=OFF
            -DUNIT_TESTS=OFF
            -DZLIB_ROOT=${ZLIB_ROOT}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${LIBCELLML_LIBRARY}
        DEPENDS
            libxml2
            zlib
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(LIBCELLML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBCELLML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCELLML_LIBRARY ${INSTALL_DIR}/${LIBCELLML_LIBRARY} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
