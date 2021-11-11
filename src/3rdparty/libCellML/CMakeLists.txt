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

# Either retrieve or build libCellML.

if(LIBOPENCOR_PREBUILT_LIBCELLML)
    if(WIN32)
        if(RELEASE_MODE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         ab6d22984d373b135dfe06b5835c5b2776a97d92
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 335aaa3c4de92eaa75b95089b9ff2e6815ca5483)
        else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         6524fe386dda33ca78e4e955bcbf5de864523eac
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES c3ae15e4d6eaab9b3fb4372c2c4bf8f78c5a29a6)
        endif()
    elseif(APPLE)
    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                     f4bf2a8d6279b2e98d4508b2d508bb1df6e67708
                     SHA1_FILES ${SHA1_FILES}
                     SHA1_VALUES 5e50877d730583360fab06be3feaa5bfec76ccc8)
    else()
    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                     cd010dc89b63aa83830106c19d6830a38c2aad99
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
            -DCOVERAGE=OFF
            -DINSTALL_PREFIX=${INSTALL_DIR}
            -DLLVM_COVERAGE=OFF
            -DMEMCHECK=OFF
            -DPYTHON_COVERAGE=OFF
            -DUNIT_TESTS=OFF
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Statically link libCellML to libOpenCOR.

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${INSTALL_DIR}/include>)

if(NOT LIBOPENCOR_PREBUILT_LIBCELLML)
    add_dependencies(${CMAKE_PROJECT_NAME} libCellML)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
                      PRIVATE ${INSTALL_DIR}/${STATIC_LIBRARY})
