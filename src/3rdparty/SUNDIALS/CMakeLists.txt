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

set(PACKAGE_NAME SUNDIALS)
set(PACKAGE_VERSION 6.1.0)
set(PACKAGE_REPOSITORY sundials)
set(GIT_TAG v6.1.0-libopencor)
set(RELEASE_TAG v6.1.0-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_SUNDIALS)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             3fe4ad9884a39f804324c8ff1e5d397447cf5290)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8c27449dcd97a55facc59880be65218d8a6a4607)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         57423b930dccc9b0d9433a0583abba72903f499d)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         c8e96551912f18eec4cfb8a246225d463325c26f)
    endif()
else()
    # Build our package.

    set(PACKAGE_C_FLAGS -DNO_FPRINTF_OUTPUT ${CMAKE_C_FLAGS})

    build_package(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_CACHE_ARGS
            -DCMAKE_C_FLAGS:STRING=${PACKAGE_C_FLAGS}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_TESTING=OFF
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DEXAMPLES_ENABLE_C=OFF
    )

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(SUNDIALS_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(SUNDIALS_CMAKE_DIR ${INSTALL_DIR}/lib/cmake CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(SUNDIALS_CMAKE_PACKAGE_NAME ${PACKAGE_NAME} CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(SUNDIALS_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(SUNDIALS_LIBRARIES
    SUNDIALS::arkode_static
    SUNDIALS::cvode_static
    SUNDIALS::cvodes_static
    SUNDIALS::generic_static
    SUNDIALS::ida_static
    SUNDIALS::idas_static
    SUNDIALS::kinsol_static
    SUNDIALS::nvecmanyvector_static
    SUNDIALS::nvecserial_static
    SUNDIALS::sunlinsolband_static
    SUNDIALS::sunlinsoldense_static
    SUNDIALS::sunlinsolpcg_static
    SUNDIALS::sunlinsolspbcgs_static
    SUNDIALS::sunlinsolspfgmr_static
    SUNDIALS::sunlinsolspgmr_static
    SUNDIALS::sunlinsolsptfqmr_static
    SUNDIALS::sunmatrixband_static
    SUNDIALS::sunmatrixdense_static
    SUNDIALS::sunmatrixsparse_static
    SUNDIALS::sunnonlinsolfixedpoint_static
    SUNDIALS::sunnonlinsolnewton_static
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
