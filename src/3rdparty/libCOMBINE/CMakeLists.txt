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

set(PACKAGE_NAME libCOMBINE)
set(PACKAGE_VERSION 0.2.20)
set(PACKAGE_REPOSITORY libCombine)
set(RELEASE_TAG v0.2.20-buffer)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(WIN32)
    set(LIBRARY_PREFIX lib)
endif()

if(LIBOPENCOR_PREBUILT_LIBCOMBINE)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         c9bab42772e66abd8a93c4b649d949d28285e625)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     1e9e2a453c434360e6a0672a9eee12b639f0322c)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     7922864d07ad12a95d48d68912d13b246d200666)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     f9bd7ac52939fabe8c68aea7feb5e8e95ff0f028)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     bd9e6bb70e94251657c90474601c8b008988b50d)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 d3c2dd16db126c0d63422c06bc7bc4ef8aab9850)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 e0b90f44fcc27f79e31c97d6899a032f2fa4dbd4)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 f1183ec38ebf408d99b7604f52e3eed54b10a204)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 b84b4cb60e9a60700bddf68f1ffcfeba88681bbd)
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
            -DBUILD_TEST=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBCOMBINE_SHARED_VERSION=OFF
            -DLIBCOMBINE_SKIP_SHARED_LIBRARY=ON
            -DLIBSBML_INCLUDE_DIR=${LIBSBML_INCLUDE_DIR}
            -DLIBSBML_LIBRARY=${LIBSBML_LIBRARY_FILE}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DXML_INCLUDE_DIR=${LIBXML2_INCLUDE_DIR}
            -DXML_LIBRARY=${LIBXML2_LIBRARY_FILE}
            -DZIPPER_INCLUDE_DIR=${ZIPPER_INCLUDE_DIR}
            -DZIPPER_LIBRARY=${ZIPPER_LIBRARY_FILE}
            -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DZLIB_LIBRARY=${ZLIB_LIBRARY_FILE}
    )

    # Patch some of our CMake configuration files.

    file(GLOB INSTALLED_FILES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}" "${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}/*.cmake")

    foreach(INSTALLED_FILE ${INSTALLED_FILES})
        string(REPLACE "Combine" "combine" NEW_INSTALLED_FILE "${INSTALLED_FILE}")

        file(RENAME ${INSTALLED_FILE} ${NEW_INSTALLED_FILE})
    endforeach()

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}combine-static-config.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE "${INSTALL_DIR}" "\${_IMPORT_PREFIX}" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REGEX REPLACE "foreach \\\(library .* \\\)" "foreach (library)" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "Combine" "combine" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}combine-static-targets.cmake)

    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REGEX REPLACE "INTERFACE_LINK_LIBRARIES \\\"[^\\\"]*\\\"" "INTERFACE_LINK_LIBRARIES \"\"" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "Combine" "combine" FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")

    foreach(PACKAGE_MODE release debug)
        set(CONFIG_FILE ${INSTALL_DIR}/lib/cmake/${LIBRARY_PREFIX}combine-static-targets-${PACKAGE_MODE}.cmake)

        if(EXISTS ${CONFIG_FILE})
            file(READ ${CONFIG_FILE} FILE_CONTENTS)

            string(REPLACE "Combine" "combine" FILE_CONTENTS "${FILE_CONTENTS}")

            file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")
        endif()
    endforeach()

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(LIBCOMBINE_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LIBCOMBINE_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(LIBCOMBINE_CMAKE_PACKAGE_NAME ${LIBRARY_PREFIX}combine-static CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(LIBCOMBINE_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LIBCOMBINE_LIBRARY ${LIBRARY_PREFIX}combine-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(LIBCOMBINE_DEFINITIONS LIBCOMBINE_STATIC CACHE INTERNAL "${PACKAGE_NAME}'s definitions.")
