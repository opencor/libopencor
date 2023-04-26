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

set(PACKAGE_NAME zipper)
set(PACKAGE_VERSION 0.9.1)
set(PACKAGE_REPOSITORY zipper)
set(GIT_TAG 6f54c03)
set(RELEASE_TAG 6f54c03)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_ZIPPER)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             18d91c7bc8e8a576c748bcfc5a8dbbe27a4d4551)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             ce60c507fddc00d2fa3092e5383fd46852504722)
        endif()
    elseif(APPLE)
        if(INTEL_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             934654bea9a68d2dd1ccad70d3942cec9a17e101)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             372736c5821254782e1794b5b902a5de120379d4)
        endif()
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         7a75f83c6dae5237701775795e14b7579d8030aa)
    endif()
else()
    # Build our package.

    if(APPLE)
        set(PACKAGE_C_FLAGS -Wno-implicit-function-declaration)
    endif()

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_CACHE_ARGS
            -DCMAKE_C_FLAGS:STRING=${PACKAGE_C_FLAGS}
        CMAKE_ARGS
            -DBUILD_TEST=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
            -DZLIB_LIBRARY=${ZLIB_LIBRARY_FILE}
    )

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(ZIPPER_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(ZIPPER_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(ZIPPER_LIBRARY zipper::libZipper-static CACHE INTERNAL "${PACKAGE_NAME}'s library.")
set(ZIPPER_LIBRARY_FILE ${INSTALL_DIR}/lib/libZipper-static${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE INTERNAL "${PACKAGE_NAME}'s library.")
