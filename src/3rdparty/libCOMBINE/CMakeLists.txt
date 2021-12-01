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

set(PACKAGE_NAME libCOMBINE)
set(PACKAGE_VERSION 0.2.13)
set(PACKAGE_REPOSITORY libCombine)
set(GIT_TAG v0.2.13)
set(RELEASE_TAG v0.2.13)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

set(LIBCOMBINE_LIBRARY lib/libCombine-static${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${LIBCOMBINE_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBCOMBINE)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8fde9db85dc96e7e151e2533f118bd4936aa192d
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 67eadcb0fcf91b64672f64e296dd633d27a9287f)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             f112f45a75959e6512f59006879ef14f36b09d47
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 254bbd8aed65e874e7db117029e739850d905354)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         c0035b6a46bb5f84fb28181079aec819d5c81150
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES b631c526c0d59e860646ee85c7794574ff7c785a)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         854acaf7db0c25fbcf4c89b64e85ee367e404a74
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 9870103b1a4cffd037b3824cd591d9cd9e749c76)
    endif()
else()
    set(EXTRA_LIBS ${LIBXML2_LIBRARY})

    if(WIN32)
        string(APPEND EXTRA_LIBS |WS2_32)
    endif()

    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        LIST_SEPARATOR
            |
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DBUILD_TEST=OFF
            -DEXTRA_LIBS=${EXTRA_LIBS}
            -DLIBCOMBINE_SHARED_VERSION=OFF
            -DLIBCOMBINE_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_INCLUDE_DIR=${LIBSBML_INCLUDE_DIR}
            -DLIBSBML_LIBRARY=${LIBSBML_LIBRARY}
            -DLIBSBML_STATIC=ON
            -DLIBZ_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DLIBZ_LIBRARY=${ZLIB_LIBRARY}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DZIPPER_INCLUDE_DIR=${ZIPPER_INCLUDE_DIR}
            -DZIPPER_LIBRARY=${ZIPPER_LIBRARY}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${LIBCOMBINE_LIBRARY}
        DEPENDS
            libSBML
            libxml2
            zipper
            zlib
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(LIBCOMBINE_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBCOMBINE_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCOMBINE_LIBRARIES
    ${INSTALL_DIR}/${LIBCOMBINE_LIBRARY}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
set(LIBCOMBINE_DEFINITIONS LIBCOMBINE_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
