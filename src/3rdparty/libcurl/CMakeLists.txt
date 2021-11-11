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

set(PACKAGE_NAME libcurl)
set(PACKAGE_VERSION 7.80.0)
set(PACKAGE_REPOSITORY curl)
set(GIT_TAG curl-7_80_0)
set(RELEASE_TAG curl-7_80_0)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

if(WIN32)
    if(RELEASE_MODE)
        set(DEBUG_TAG)
    else()
        set(DEBUG_TAG -d)
    endif()

    set(STATIC_LIBRARY lib/libcurl${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(STATIC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}curl${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build libcurl.

if(LIBOPENCOR_PREBUILT_LIBCURL)
    if(WIN32)
        if(RELEASE_MODE)
            message(">>> Retrieving libcurl...")
        else()
            message(">>> Retrieving libcurl...")
        endif()
    elseif(APPLE)
        message(">>> Retrieving libcurl...")
    else()
        message(">>> Retrieving libcurl...")
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
            -DBUILD_CURL_EXE=OFF
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_TESTING=OFF
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Statically link libcurl to libOpenCOR.

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${INSTALL_DIR}/include>)

if(NOT LIBOPENCOR_PREBUILT_LIBCURL)
    add_dependencies(${CMAKE_PROJECT_NAME} libcurl)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
                      PRIVATE ${INSTALL_DIR}/${STATIC_LIBRARY})
