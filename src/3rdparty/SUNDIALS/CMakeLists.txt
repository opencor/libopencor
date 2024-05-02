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
set(PACKAGE_VERSION v6.2.0)
set(PACKAGE_REPOSITORY sundials)
set(RELEASE_TAG v6.2.0-libopencor)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_SUNDIALS)
    if(EMSCRIPTEN)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         42770edddf6ed996a55a50078e54e387ceb8da0a)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     7bc8ba9a62a6bb8efd52bc7aea1f17aa2d8ef024)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     59647bc79af40a528802941f6dce584fd1cbe50e)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     1471019d8221d5f079a30baa0394e0dc54ba3edb)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     d9367c7fa4f33d8e2f9b1b1654a1766d26abaa60)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 ef844d27b82a63b7167f3165e45ce78d20c3a7cf)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 a2399ef9cf50d64c1dad7d6e554700a47adde9b2)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 b0d953dffcab7be7440d148bf334cac7913914dd)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 bcaaad18087d6e4a002b3eda5f5c49eca7e255cc)
            endif()
        endif()
    endif()
else()
    # Build our package.

    set(PACKAGE_C_FLAGS -DNO_FPRINTF_OUTPUT ${CMAKE_C_FLAGS})

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_CACHE_ARGS
            -DCMAKE_C_FLAGS:STRING=${PACKAGE_C_FLAGS}
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS=OFF
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DEXAMPLES_ENABLE_C=OFF
            -DEXAMPLES_INSTALL=OFF
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
