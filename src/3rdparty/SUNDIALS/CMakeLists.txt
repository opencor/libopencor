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
set(PACKAGE_VERSION v7.7.0)
set(PACKAGE_REPOSITORY sundials)
set(RELEASE_TAG v7.7.0)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_SUNDIALS)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         e543ccdaaca2174c10dec3ac77bfb12c7eb9b7be)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     afc3e17457739e8be0818f5a74ad082b89b00644)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     12d24b492b6ef5b267cb27548051f9e0116ae589)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     3282dee04bb9bdca574fe3596baa2dab10be37a1)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     505f61fc017cf1f11548f180ca1d2a19e41c808c)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 cb01f19ec58984344bf8954c8c600851c995a3a3)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 30cf28a0d3d12d41e2f28aea6cf1e81adb806dbd)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 bd49d8ace9b5a193d5734f5e3c04e86acc54a573)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 b24979460c1dbe712f558fde2238d1030f38c0b9)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.
    # Note: we only need CVODE and KINSOL (and none of SUNDIALS' other packages), and we don't need SUNDIALS' logging
    #       since we handle errors ourselves and don't use a logger (see src/solver/solvercvode.cpp and
    #       src/solver/solverkinsol.cpp).

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DSUNDIALS_ENABLE_ARKODE=OFF
            -DSUNDIALS_ENABLE_C_EXAMPLES=OFF
            -DSUNDIALS_ENABLE_CVODES=OFF
            -DSUNDIALS_ENABLE_EXAMPLES_INSTALL=OFF
            -DSUNDIALS_ENABLE_IDA=OFF
            -DSUNDIALS_ENABLE_IDAS=OFF
            -DSUNDIALS_ENABLE_NVECTOR_MANYVECTOR=OFF
            -DSUNDIALS_LOGGING_LEVEL=0
    )

    # Create our package.

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   include lib)
endif()

# Make our package accessible.

set(SUNDIALS_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(SUNDIALS_CMAKE_DIR ${INSTALL_DIR}/lib/cmake/sundials CACHE INTERNAL "${PACKAGE_NAME}'s CMake directory.")
set(SUNDIALS_CMAKE_PACKAGE_NAME ${PACKAGE_NAME} CACHE INTERNAL "${PACKAGE_NAME}'s CMake package name.")
set(SUNDIALS_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(SUNDIALS_LIBRARIES
    SUNDIALS::core_static
    SUNDIALS::cvode_static
    SUNDIALS::kinsol_static
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
