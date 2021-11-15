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

if(WIN32)
    if(RELEASE_MODE)
        set(DEBUG_TAG)
    else()
        set(DEBUG_TAG d)
    endif()

    set(STATIC_LIBRARY lib/libcellml${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(STATIC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}cellml${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBCELLML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             b6f7374ef7ece62d0a3eb3f55e0d8a49d5831f8d
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 4333f52a1ea0d8be47d4e3b5b8c7a2e1cbd400c4)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8726e4331940c9049c82cbb1b8d9d693e6454527
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 088404266713b5419ad1784605c4702d04ac8a95)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         7b32b9d52d1a616cd90a9aca3161f079593b18b0
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES a98359bd3f5b11792b373e2b41511860923090d4)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         0f2a05e4f0bb2d4b0c5498333dbe61d5f095dad8
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
            -DLIBXML2_LIBRARY=${LIBXML2_LIBRARY}
            -DLLVM_COVERAGE=OFF
            -DMEMCHECK=OFF
            -DPYTHON_COVERAGE=OFF
            -DUNIT_TESTS=OFF
            -DZLIB_LIBRARY_RELEASE=${ZLIB_LIBRARY}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
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
