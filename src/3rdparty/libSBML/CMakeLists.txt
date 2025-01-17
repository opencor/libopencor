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
set(PACKAGE_VERSION 5.20.2)
set(PACKAGE_REPOSITORY libsbml)
set(RELEASE_TAG v5.20.2b)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBSBML)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         835666b754839b676b4bb3700e02a51e4490d151)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     d3e6000f91f84645fcad33b2122d9b4498365993)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     723cc9ce5b3238a4f75d0edea92556160b6402e1)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     7ed21fc3cb3c923c5ca179ea73ab674f92a96d4e)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     af78f4d80ad40213a34ced762e8dc55313e20547)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 a9237452fda3f5a3edc94baa2da8ca6b8e6ecc0a)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 582ee27a38e0fd8864f2fc41f30afd37d4d3db3d)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 391b675c4bf59d3443fdf018ce5c505317ca1872)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 40dd7ff95fe5178d245dca6376c62f5b144f0eac)
            endif()
        endif()
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBSBML_SHARED_VERSION=OFF
            -DLIBSBML_SKIP_SHARED_LIBRARY=ON
            -DLIBXML_INCLUDE_DIR=${LIBXML2_INCLUDE_DIR}
            -DLIBXML_LIBRARY=${LIBXML2_LIBRARY_FILE}
            -DWITH_BZIP2=OFF
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DWITH_ZLIB=OFF
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sbml-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REGEX REPLACE "foreach \\\(library .* \\\)" "foreach (library)" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}sbml-static-targets.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REGEX REPLACE "INTERFACE_LINK_LIBRARIES \\\"[^\\\"]*\\\"" "INTERFACE_LINK_LIBRARIES \"\"" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib/cmake ${STATIC_LIBRARIES})
endif()

# Make our package accessible.

set(LIBSBML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBSBML_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBSBML_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}sbml-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBSBML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBSBML_LIBRARY ${LIBRARY_PREFIX}sbml-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSBML_LIBRARY_FILE ${INSTALL_DIR}/lib/libsbml-static${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBSBML_DEFINITIONS LIBSBML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
