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

set(PACKAGE_NAME libSBML)
set(PACKAGE_VERSION 5.19.0)
set(PACKAGE_REPOSITORY libsbml)
set(GIT_TAG v5.19.0-libopencor)
set(RELEASE_TAG v5.19.0-libopencor)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

set(STATIC_LIBRARY lib/libsbml-static${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LIBSBML)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             7ec10246a30050aacc07842756f62ea33fdfc9b7
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES dfbc06f97e91999f1b53b36acc10f506d6e1250c)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             9e80ec456b4f4cdb780d6ddd5b2faaaa298de0d8
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES d9c6e9911682c9f3d7c6b7b02a624e2198ae2031)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         e65e2402660de7b3b33c67e345e2c57d6b098699
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 2a4d8fa86b159318c59ac0218968b09f77dbfd07)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         f181e3bff33395a081a36a9946b9b26f61fc7090
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 78ea2181e9b5850541306de9ca6dee0f511cf17b)
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
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBXML_INCLUDE_DIR=${LIBXML2_INCLUDE_DIR}
            -DLIBXML_LIBRARY=${LIBXML2_LIBRARY}
            -DWITH_BZIP2=OFF
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DWITH_ZLIB=OFF
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
        DEPENDS
            libxml2
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(LIBSBML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBSBML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSBML_LIBRARY ${INSTALL_DIR}/${STATIC_LIBRARY} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
