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
                         f3ef4afe47f722699c55a2221faf65c1f17fd5a0)
    else()
        if(WIN32)
            if(RELEASE_MODE)
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     cf87f196975c1d3d62b5733e64028e00d20bc61c)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     54edefd4839b544fcda11ffbea6c410e328fd812)
                endif()
            else()
                if(INTEL_MODE)
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     3b2d251b1b51b558594347e10b631542f60903f5)
                else()
                    retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                     ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                     1685edc065fd7b323c7cd49956dcf0df188329d4)
                endif()
            endif()
        elseif(APPLE)
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 a2494dd173e01f513e9d1ce1eb9bee89440a7dbf)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 56ccaf46db0f853f601dfc70415481009d0fe89d)
            endif()
        else()
            if(INTEL_MODE)
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 ff9a7ccb8998222bf69af9c2473c4748933c531a)
            else()
                retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                 ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                                 b18b19b6a0a22bb9ecfc1970372422ba4a94cf52)
            endif()
        endif()
    endif()
elseif(NOT ONLY_BUILD_JAVASCRIPT_THIRD_PARTY_LIBRARIES)
    # Build our package.

    set(PACKAGE_C_FLAGS "-DNO_FPRINTF_OUTPUT ${CMAKE_C_FLAGS}")

    # Enable WebAssembly SIMD for our WASM version.
    # Note: most of the simulation time is spent in SUNDIALS' dense linear algebra, which the compiler can then
    #       auto-vectorise. This makes our stiff models run 8-16% faster. WebAssembly SIMD is supported by all modern
    #       browsers (Chrome 91+, Firefox 89+, and Safari 16.4+).

    if(EMSCRIPTEN)
        set(PACKAGE_C_FLAGS "${PACKAGE_C_FLAGS} -msimd128")
    endif()

    build_package(${PACKAGE_NAME}
        URL
            https://github.com/opencor/${PACKAGE_REPOSITORY}/archive/refs/tags/${RELEASE_TAG}.tar.gz
        DOWNLOAD_NO_PROGRESS ON
        CMAKE_CACHE_ARGS
            -DCMAKE_C_FLAGS:STRING=${PACKAGE_C_FLAGS}
        CMAKE_ARGS
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
    SUNDIALS::core_static
    SUNDIALS::cvode_static
    SUNDIALS::cvodes_static
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
