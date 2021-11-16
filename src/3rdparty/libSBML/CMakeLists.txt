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
                         de7caa9e16ff4e30b5a64d96bf34c5c49a251f65
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 4d08de8699bf7db9263e2fc71482c46cdf03db26)
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
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let other third-party libraries find and use our package.

set(LIBSBML_INCLUDE_DIR ${INSTALL_DIR}/include PARENT_SCOPE)
set(LIBSBML_LIBRARY ${INSTALL_DIR}/${STATIC_LIBRARY} PARENT_SCOPE)