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
set(GIT_TAG libopencor)
set(RELEASE_TAG libopencor)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

if(WIN32 AND NOT RELEASE_MODE)
    set(DEBUG_TAG d)
endif()

set(STATIC_LIBRARY lib/libcellml${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBCELLML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             f12b23cafe2ae13a6a217223a2a8b18ef42405f6
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 42a470ae095c2b8632f118f79a9a366a0a122cd5)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             2c697bf2f4e9682fcf9f4f151df6d0bca1099ae5
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 7638b990d3c753b46d0897067353f97e9ad39983)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         893d3cf69c720c693af1659b0319efb846703cea
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 196898a324bdc45ca5a6baca1e99d5075b1c270f)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         cf0fb2b804a809afeb9984b049e1c2f7c2205571
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 58cfcb61f88f33fd472fb4f32b6f6cb67bc50618)
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
            -DBUILD_TYPE=${EP_CMAKE_BUILD_TYPE}
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
            ${INSTALL_DIR}/${STATIC_LIBRARY}
        DEPENDS
            libxml2
            zlib
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Statically link our package to libOpenCOR.

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${INSTALL_DIR}/include>)

if(NOT LIBOPENCOR_PREBUILT_LIBCELLML)
    add_dependencies(${CMAKE_PROJECT_NAME} ${PACKAGE_NAME})
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
                      PRIVATE ${INSTALL_DIR}/${STATIC_LIBRARY})
