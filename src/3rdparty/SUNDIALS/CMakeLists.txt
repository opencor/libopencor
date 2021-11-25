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
set(PACKAGE_VERSION 5.8.0)
set(PACKAGE_REPOSITORY sundials)
set(GIT_TAG v5.8.0-libopencor)
set(RELEASE_TAG v5.8.0-libopencor)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

set(ARKODE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_arkode${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CVODE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_cvode${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CVODES_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_cvodes${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GENERIC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_generic${CMAKE_STATIC_LIBRARY_SUFFIX})
set(IDA_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_ida${CMAKE_STATIC_LIBRARY_SUFFIX})
set(IDAS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_idas${CMAKE_STATIC_LIBRARY_SUFFIX})
set(KINSOL_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_kinsol${CMAKE_STATIC_LIBRARY_SUFFIX})
set(NVECMANYVECTOR_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_nvecmanyvector${CMAKE_STATIC_LIBRARY_SUFFIX})
set(NVECSERIAL_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_nvecserial${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNLINSOLBAND_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunlinsolband${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNLINSOLDENSE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunlinsoldense${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNLINSOLPCG_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunlinsolpcg${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNLINSOLSPBCGS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunlinsolspbcgs${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNLINSOLSPFGMR_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunlinsolspfgmr${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNLINSOLSPGMR_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunlinsolspgmr${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNLINSOLSPTFQMR_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunlinsolsptfqmr${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNMATRIXBAND_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunmatrixband${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNMATRIXDENSE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunmatrixdense${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNMATRIXSPARSE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunmatrixsparse${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNNONLINSOLFIXEDPOINT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunnonlinsolfixedpoint${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SUNNONLINSOLNEWTON_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}sundials_sunnonlinsolnewton${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${ARKODE_LIBRARY}
    ${CVODE_LIBRARY}
    ${CVODES_LIBRARY}
    ${GENERIC_LIBRARY}
    ${IDA_LIBRARY}
    ${IDAS_LIBRARY}
    ${KINSOL_LIBRARY}
    ${NVECMANYVECTOR_LIBRARY}
    ${NVECSERIAL_LIBRARY}
    ${SUNLINSOLBAND_LIBRARY}
    ${SUNLINSOLDENSE_LIBRARY}
    ${SUNLINSOLPCG_LIBRARY}
    ${SUNLINSOLSPBCGS_LIBRARY}
    ${SUNLINSOLSPFGMR_LIBRARY}
    ${SUNLINSOLSPGMR_LIBRARY}
    ${SUNLINSOLSPTFQMR_LIBRARY}
    ${SUNMATRIXBAND_LIBRARY}
    ${SUNMATRIXDENSE_LIBRARY}
    ${SUNMATRIXSPARSE_LIBRARY}
    ${SUNNONLINSOLFIXEDPOINT_LIBRARY}
    ${SUNNONLINSOLNEWTON_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_SUNDIALS)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             5aeea37319951b010ee335c3c720ae8a1435aa4f
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 794e5e7f33cc3eb6f87cc05fd7164ae903e41be4
                                         e99a7c9fc970f6ecfb0189ad8f4748f8203d12f2
                                         4421d442d23da796d0ed91d154971afeefbbf326
                                         eee4ad63deccc940b5555af17f9ab822f7bb8703
                                         cb1245c8b5c7e13ecf18ac6c68676d2aa7c5ffae
                                         54f9e4f9749d4b39379eecd66802b20b01c5422d
                                         c1cbda283db0336ecb3ccf33014f85d15f802b97
                                         0e6b3c7e4c22dd4418ae3c6bc443dc69fb7e8ecd
                                         aa2e4361eddec8d26e003a7862c3d159fd6a294e
                                         7e758d32cbad470d6aa6117a2385ca37f957d4c2
                                         9f8fc03137ccd89f69e342a3f4566bf169caa75c
                                         33761deaafe128b031be33d8159593ecfe34fa4d
                                         d8879f87429512e11fa88587d0904e389bbb1525
                                         df7bf64aee8ed4029603ccdeeea6ca8749cf26fc
                                         6a84ef986b024faaa4b63657b1f54478d544af55
                                         f0a94194e18396c3e53aa5c67392dc8ce4e72785
                                         4e5b6859e572f5e289a42ae6c271d16d18653a73
                                         a95f7a0979306b37891e52be033eebe5c0940155
                                         ecd13219a3d328acd2cdc563bd594ca1577d6d13
                                         e7b4a43967d7397b9127708d6cdf621cb8e48341
                                         236cfe2955f3c1e8e5ffbc2a53dfd3e69cc29c25)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             c5378ceefe2d524f06e11d4aa9189081130e1a97
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES d417c59a0e034a0291e73095f5ea7c14a13a4cab
                                         c6c8a666c4008926b11b28652fbfaab596f7d2d3
                                         c75b3370f12f29edfb5f5dfa847fb3ff756dae7e
                                         d586ac7583c3aca5f6f0a195e9f258166d97d975
                                         91ec652b8e9dfbdcc63a89425f1eb9f7eceeddf6
                                         9b759c7281bed468fe8ccc41c28b3b71e8741d9a
                                         f4fa3badeef72760099828767ef88a0754dfb51b
                                         dbf0c81c75e1b15f7ac3fa1f3be50a2703f7f3c9
                                         44fb9db49682505a64847a4c4ebc7dbc74f745f7
                                         2b651ef2e4a3ace1eed508861545b522c869d27e
                                         fb3fa40ed649cba9e01be372aab06d32537b8f71
                                         ccb3729bf3d2a9f3029522c3efd02aba6ece0534
                                         bcf7d3adb3d4cddb5276f778fe403db228a00944
                                         bfce6628be06fc04754202a1e922b2415dde0332
                                         e879400645c13d49300e9e51e911f06a421a2205
                                         4c886e73a2715f9eb93aca5a5e3ed997f653b06c
                                         c547ce947e1dc161156d81aaede9abcab47fae34
                                         8738793256608ba6f5ee9c9a384013745baa30db
                                         625efcc6c7dcc3c5f80f68cda184ce55dc92f56d
                                         15ff698357ea1ba21850722e48706ee1511b1ca5
                                         193333a000794985a1b1ad03288a7bfebc27cb7c)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         3e381eb543758baf9abb591e969c6103505db0dc
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 07687cd740daffd6c8e431f8c72c8be6a31b8850
                                     342495ea2ab838dfabd87569cff7664f512940e4
                                     e30530da98ec72d883bb8b01688d444f11eb47b0
                                     9b5bb3750f3e5eef6809863d612af91e24784619
                                     53bfd9ca71582bd98bbd30478a87288500e83f3d
                                     461bb947a8676d1d9b0dba6a39a7f2b3987a5bae
                                     75430f08c7bc19bc8ce7e75160f57239408fd5cc
                                     9365d7c09c107d5fcae42b0e045b9f46b1790343
                                     9a1af83c4838085a71d0e77458368b560c7cf9a4
                                     9d1057c6f0460a81ca80eeee0503d29c63237883
                                     10bab929fe31f3a44be2f92da864500abf49582b
                                     0166cfc77153ebbfb5831ff0fca80e486ca88e21
                                     fb2cd808ab2b92eaee8f16638862a3db7ea14134
                                     75e65e7734eb6c392be8dbb4a1ee169db5c7b3f9
                                     41e8c09ded29d612a988961f0423803bff2b1c65
                                     5cb16c2ec5c93fc9426c62f718e265ea8f28489e
                                     9bd219f80eded744f657c0d16bcaa1048fddd9db
                                     df60e238a5b14862070c702a327220886ebe50be
                                     6d956fb9036c656bac99228647fb43d814049125
                                     8480085ebca97bb9b4e2eb7ddcae02ae4728ea69
                                     9c2b82ae3c9c038af59d5fe014804edfb08a03ee)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         f2f473b19ee166e1a36daab9f85a8cf3a528838f
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 371ec699d136db788d66d9550c3dda9dc36d6f60
                                     fadd84a88ecb44f1e742f0b7364e57fdba64a2ca
                                     9b0934a94a6e4bb8f209873f7da4c2ddfa108530
                                     70974d2a66db6cef2c2b26e93a9563bd3379a232
                                     f2851c34041c795c5a770961836880ef0ca00920
                                     3796604018da3376d69e7f282e24fa14595c14ac
                                     8cf8aa1362f9091ab60da121c3fd028133c612ff
                                     040d549daef7f16cf57c01bd9d1cbd8f2cbce1ce
                                     6d65420f0c5871e81920094e644da5ebeb344e06
                                     4b8e1debc8e58bb43ae4e79a2696b0c8c2d8a182
                                     44eb4886e8b386dd4edcfb408794cb9c93924c53
                                     47884085398c856bcc563e2dffb3bf91168dde47
                                     22c886bc43d46283bc2da50005ef0e70f43dcc8a
                                     b183ef3dbb04e183efa05f8b5f5b9c92d05d9965
                                     a405047259e97cf24c04b39132b574d00b74e2a8
                                     1363e11039ead939806fb482291cd9e3d0a99c78
                                     94b51d18ca41c81dc34460736dd08d03d5cc3085
                                     9f12947b5a693b124cf0c64e3e92a7e621500002
                                     8337d8a3af0e3bf2d2c2bede65cd95076578781c
                                     44ca6e33fb9b3017b4a8c705d1cd1572cff44618
                                     98c4c274a6d0984dacb82160042e074eef471f30)
    endif()
else()
    set(EP_CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_FPRINTF_OUTPUT")

    ExternalProject_Add(${PACKAGE_NAME}
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
            -DEXAMPLES_ENABLE_C=OFF
        CMAKE_CACHE_ARGS
            -DCMAKE_C_FLAGS:STRING=${EP_CMAKE_C_FLAGS}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${ARKODE_LIBRARY}
            ${INSTALL_DIR}/${CVODE_LIBRARY}
            ${INSTALL_DIR}/${CVODES_LIBRARY}
            ${INSTALL_DIR}/${GENERIC_LIBRARY}
            ${INSTALL_DIR}/${IDA_LIBRARY}
            ${INSTALL_DIR}/${IDAS_LIBRARY}
            ${INSTALL_DIR}/${KINSOL_LIBRARY}
            ${INSTALL_DIR}/${NVECMANYVECTOR_LIBRARY}
            ${INSTALL_DIR}/${NVECSERIAL_LIBRARY}
            ${INSTALL_DIR}/${SUNLINSOLBAND_LIBRARY}
            ${INSTALL_DIR}/${SUNLINSOLDENSE_LIBRARY}
            ${INSTALL_DIR}/${SUNLINSOLPCG_LIBRARY}
            ${INSTALL_DIR}/${SUNLINSOLSPBCGS_LIBRARY}
            ${INSTALL_DIR}/${SUNLINSOLSPFGMR_LIBRARY}
            ${INSTALL_DIR}/${SUNLINSOLSPGMR_LIBRARY}
            ${INSTALL_DIR}/${SUNLINSOLSPTFQMR_LIBRARY}
            ${INSTALL_DIR}/${SUNMATRIXBAND_LIBRARY}
            ${INSTALL_DIR}/${SUNMATRIXDENSE_LIBRARY}
            ${INSTALL_DIR}/${SUNMATRIXSPARSE_LIBRARY}
            ${INSTALL_DIR}/${SUNNONLINSOLFIXEDPOINT_LIBRARY}
            ${INSTALL_DIR}/${SUNNONLINSOLNEWTON_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(SUNDIALS_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(SUNDIALS_INCLUDE_DIR ${INSTALL_DIR}/include/sundials CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(SUNDIALS_LIBRARIES
    ${INSTALL_DIR}/${ARKODE_LIBRARY}
    ${INSTALL_DIR}/${CVODE_LIBRARY}
    ${INSTALL_DIR}/${CVODES_LIBRARY}
    ${INSTALL_DIR}/${GENERIC_LIBRARY}
    ${INSTALL_DIR}/${IDA_LIBRARY}
    ${INSTALL_DIR}/${IDAS_LIBRARY}
    ${INSTALL_DIR}/${KINSOL_LIBRARY}
    ${INSTALL_DIR}/${NVECMANYVECTOR_LIBRARY}
    ${INSTALL_DIR}/${NVECSERIAL_LIBRARY}
    ${INSTALL_DIR}/${SUNLINSOLBAND_LIBRARY}
    ${INSTALL_DIR}/${SUNLINSOLDENSE_LIBRARY}
    ${INSTALL_DIR}/${SUNLINSOLPCG_LIBRARY}
    ${INSTALL_DIR}/${SUNLINSOLSPBCGS_LIBRARY}
    ${INSTALL_DIR}/${SUNLINSOLSPFGMR_LIBRARY}
    ${INSTALL_DIR}/${SUNLINSOLSPGMR_LIBRARY}
    ${INSTALL_DIR}/${SUNLINSOLSPTFQMR_LIBRARY}
    ${INSTALL_DIR}/${SUNMATRIXBAND_LIBRARY}
    ${INSTALL_DIR}/${SUNMATRIXDENSE_LIBRARY}
    ${INSTALL_DIR}/${SUNMATRIXSPARSE_LIBRARY}
    ${INSTALL_DIR}/${SUNNONLINSOLFIXEDPOINT_LIBRARY}
    ${INSTALL_DIR}/${SUNNONLINSOLNEWTON_LIBRARY}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
