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
                         9c1fe015da0346d3ef1ec91b1647f494716d8c22)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     378c89f47fc7f74aaf0edce15f3c994cf9b3fa55)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     00ce01f6185c476b8309e7ced1a9feb7f214b843)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     06eb375428a112db46c70c2635fd1277569a2b28)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     80cce552f635b7889b24c1d55483fafcb17f9a03)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 cbb43477a9b7165c7fcf676de0af5d7a1ac8e10b)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 7afb1e77575826c2849178f68eceba0f109a755a)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 6d027cecf6b654fe21f5cc0ff7bb05fdccc107b7)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 3451b2411555ef1bbbf554c624efd0c0edccfff6)
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
