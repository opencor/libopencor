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

set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/libCellML)
set(EP_INSTALL_DIR ${EP_SOURCE_DIR}/install)

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

ExternalProject_Add(libCellML
    SOURCE_DIR
        ${EP_SOURCE_DIR}
    INSTALL_DIR
        ${EP_INSTALL_DIR}
    GIT_REPOSITORY
        https://github.com/opencor/libcellml
    GIT_TAG
        libopencor
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

# Statically link libCellML to libOpenCOR.

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${EP_INSTALL_DIR}/include>)

add_dependencies(${CMAKE_PROJECT_NAME} libCellML)

target_link_libraries(${CMAKE_PROJECT_NAME}
                      PRIVATE ${EP_INSTALL_DIR}/lib/${STATIC_LIBRARY})
