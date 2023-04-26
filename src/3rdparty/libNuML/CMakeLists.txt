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
set(PACKAGE_VERSION 1.1.6)
set(PACKAGE_REPOSITORY NuML)
set(RELEASE_TAG v1.1.6-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBNUML)
    if(WIN32)
        if(RELEASE_MODE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 f694750cb885e0ca0acdcff12ee7571f5e35b04e)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 9cc9e397c8d512a0d630cbe238c4d4903a45ddd7)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 d7d0639a87c7cab2c140d197e8a469786b6b0b6d)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 afd154cba12979e760ba7a0d8db34cd1e38dbb6e)
            endif()
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             b1aa08c1f2bb41298675165a1608bf6029743a5a)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8424f3ee04bcaa973160f56c6807b2d9d21e1482)
        endif()
    else()
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             826470e27e446414a9d1269efa89d935c8a1eb55)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             4acdd314a5c90a1208fd725901b36918dea1add9)
        endif()
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        SOURCE_SUBDIR
            libnuml
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBNUML_SHARED_VERSION=OFF
            -DLIBNUML_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_ROOT=${LIBSBML_CMAKE_DIR}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
    )

    # Patch some of our CMake configuration files.

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}numl-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "LIBSBML_CMAKE_DIR" "LIBNUML_CMAKE_DIR" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "find_dependency(LIBSBML)" "string(REPLACE \"libNuML\" \"libSBML\" LIBSBML_CMAKE_DIR \"\${LIBNUML_CMAKE_DIR}\")\nfind_dependency(\${LIBSBML_LIBRARY_NAME} PATHS \${LIBSBML_CMAKE_DIR})" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    fix_import_prefix(${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}numl-static-targets.cmake)

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib share/cmake)
endif()

# Make our package accessible.

set(LIBNUML_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBNUML_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBNUML_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBNUML_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBNUML_LIBRARY ${LIBRARY_PREFIX}numl-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBNUML_LIBRARY_FILE ${INSTALL_DIR}/lib/libnuml-static${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBNUML_DEFINITIONS LIBNUML_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
