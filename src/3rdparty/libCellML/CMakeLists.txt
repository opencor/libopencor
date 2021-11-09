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

# Build libCellML as an external project.

set(PACKAGE_NAME libCellML)
set(PACKAGE_VERSION 0.2.0)
set(PACKAGE_REPOSITORY libcellml)
set(GIT_TAG libopencor)
set(RELEASE_TAG libopencor)
set(SOURCE_DIR ${CMAKE_BINARY_DIR}/libCellML)
set(INSTALL_DIR ${SOURCE_DIR}/install)

if(WIN32)
    if(RELEASE_MODE)
        set(DEBUG_TAG)
    else()
        set(DEBUG_TAG d)
    endif()

    set(STATIC_LIBRARY libcellml${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(STATIC_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}cellml${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(SHA1_FILES
    lib/${STATIC_LIBRARY}
)

ExternalProject_Add(${PACKAGE_NAME}
    SOURCE_DIR
        ${SOURCE_DIR}
    INSTALL_DIR
        ${INSTALL_DIR}
    GIT_REPOSITORY
        https://github.com/opencor/${PACKAGE_REPOSITORY}
    GIT_TAG
        ${GIT_TAG}
    CMAKE_GENERATOR
        ${CMAKE_GENERATOR}
    CMAKE_ARGS
        ${CMAKE_ARGS}
        -DBINDINGS_PYTHON=OFF
        -DBUILD_SHARED=OFF
        -DCOVERAGE=OFF
        -DINSTALL_PREFIX=<INSTALL_DIR>
        -DLLVM_COVERAGE=OFF
        -DMEMCHECK=OFF
        -DPYTHON_COVERAGE=OFF
        -DUNIT_TESTS=OFF
    BUILD_BYPRODUCTS
        <INSTALL_DIR>/lib/${STATIC_LIBRARY}
)

# Package our external project.

create_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                    PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                    RELEASE_TAG ${RELEASE_TAG}
                    PACKAGED_FILES include ${SHA1_FILES}
                    SHA1_FILES ${SHA1_FILES})

# Statically link libCellML to libOpenCOR.

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${INSTALL_DIR}/include>)

add_dependencies(${CMAKE_PROJECT_NAME} libCellML)

target_link_libraries(${CMAKE_PROJECT_NAME}
                      PRIVATE ${INSTALL_DIR}/lib/${STATIC_LIBRARY})
