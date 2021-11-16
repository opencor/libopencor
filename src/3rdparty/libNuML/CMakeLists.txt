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
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

set(STATIC_LIBRARY lib/libnuml-static${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBNUML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8ca4be16fc0fc09be4f0ce78063a2a8c17d3c041
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES da081654ea5610de59bc8ac872823d66b9537472)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             67b386141e4685300ed633fe4b8419dcc6bdb2b7
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES c31e7b596226781cb17eee2e4d031db8ca8a107c)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         ba4d284d4fd761c6c7c22ad0caf29dedb06a41d8
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES a3e85c78909012f61a10df6d2341fb2da5633cba)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         b3e3caa2d3532572201085e69ba4e0d57225736f
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 1ddef2fc9ab8b20c749549e89470105aece62419)
    endif()
else()
    set(EXTRA_LIBS ${LIBXML2_LIBRARY})

    if(WIN32)
        string(APPEND EXTRA_LIBS |Ws2_32)
    endif()

    ExternalProject_Add(${PACKAGE_NAME}
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
            -DLIBSBML_INCLUDE_DIR=${LIBSBML_INCLUDE_DIR}
            -DLIBSBML_LIBRARY=${LIBSBML_LIBRARY}
            -DLIBSBML_STATIC=ON
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()
