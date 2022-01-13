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

set(PACKAGE_NAME zlib)
set(PACKAGE_VERSION 2.0.5)
set(PACKAGE_REPOSITORY zlib-ng)
set(GIT_TAG 2.0.5)
set(RELEASE_TAG 2.0.5)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_ZLIB)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             e93075a81154fbd87c0fa62607fb16182b776f52)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             d05e91b486b822804c8f2246161dc136318fca9f)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         4e5a99875e7ce0736161635f8d8a251511b18b2c)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         7ebc404bc150d5763c1c6fc27438ef7fe517f576)
    endif()
else()
    # Build our package.

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DBUILD_SHARED_LIBS=OFF
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DZLIB_COMPAT=ON
    )

    # Create our package.

    file(GLOB STATIC_LIBRARIES "${INSTALL_DIR}/lib/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
    string(REPLACE "${INSTALL_DIR}/" "" STATIC_LIBRARIES "${STATIC_LIBRARIES}")

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include ${STATIC_LIBRARIES})
endif()

# Make our package accessible.

if(WIN32)
    set(LIBRARY_SUFFIX libstatic)

    if(NOT RELEASE_MODE)
        set(LIBRARY_SUFFIX "${LIBRARY_SUFFIX}d")
    endif()
else()
    set(LIBRARY_PREFIX lib)
endif()

set(ZLIB_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(ZLIB_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(ZLIB_LIBRARY zlib::${LIBRARY_PREFIX}z${LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(ZLIB_LIBRARY_FILE ${INSTALL_DIR}/lib/${LIBRARY_PREFIX}z${LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
