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

# Build our different third-party libraries as external projects

include(packages)

# Keep track of which packages are to be built.

set(PACKAGES libCellML libCOMBINE libNuML libSBML libSEDML libxml2 SUNDIALS zipper zlib)

if(NOT EMSCRIPTEN)
    list(APPEND PACKAGES libcurl libssh2 LLVMClang OpenSSL)
endif()

set(PACKAGES_TO_BUILD)

foreach(PACKAGE ${PACKAGES})
    string(TOUPPER "${PACKAGE}" PACKAGE_UC)

    if(NOT LIBOPENCOR_PREBUILT_${PACKAGE_UC})
        set(PACKAGES_TO_BUILD "${PACKAGES_TO_BUILD};${PACKAGE}")
    endif()
endforeach()

if(NOT "${PACKAGES_TO_BUILD}" STREQUAL "")
    message(STATUS "The following packages are to be built:")

    foreach(PACKAGE_TO_BUILD ${PACKAGES_TO_BUILD})
        message(STATUS " - ${PACKAGE_TO_BUILD}")
    endforeach()
endif()

# Keep track of which dependent packages are to be built.

set(DEPENDENT_PACKAGES_TO_BUILD "" CACHE INTERNAL "Dependent packages to be built.")

check_dependent_packages(libxml2 libCellML libSBML)

if(EMSCRIPTEN)
    check_dependent_packages(zlib libCellML libCOMBINE zipper)
else()
    check_dependent_packages(OpenSSL libcurl libssh2)
    check_dependent_packages(zlib libCellML libCOMBINE libcurl zipper)
endif()

check_dependent_packages(libSBML libCOMBINE libNuML libSEDML)
check_dependent_packages(libNuML libSEDML)

if(NOT EMSCRIPTEN)
    check_dependent_packages(libssh2 libcurl)
endif()

check_dependent_packages(zipper libCOMBINE)

if(NOT "${DEPENDENT_PACKAGES_TO_BUILD}" STREQUAL "")
    message(STATUS "The following dependent packages are also to be built:")

    foreach(DEPENDENT_PACKAGE_TO_BUILD ${DEPENDENT_PACKAGES_TO_BUILD})
        message(STATUS " - ${DEPENDENT_PACKAGE_TO_BUILD}")
    endforeach()
endif()

# Keep track of which packages are required to build the requested packages.

check_required_packages(libSBML libxml2)
check_required_packages(libNuML libSBML)

if(NOT EMSCRIPTEN)
    check_required_packages(libssh2 OpenSSL)
endif()

check_required_packages(zipper zlib)

check_required_packages(libCellML libxml2 zlib)
check_required_packages(libCOMBINE libSBML libxml2 zipper zlib)

if(NOT EMSCRIPTEN)
    check_required_packages(libcurl libssh2 OpenSSL zlib)
endif()

check_required_packages(libSEDML libNuML libSBML)

# Initialise/reset our available packages and third-party (special) header files.

set(AVAILABLE_PACKAGES "" CACHE INTERNAL "Available packages.")
set(THIRD_PARTY_HEADER_FILES "" CACHE INTERNAL "Third-party (special) header files.")

# Prerequisite third-party libraries, respecting their own prerequisites.

set(PACKAGES_TO_ADD ${REQUIRED_PACKAGES_TO_ADD} ${PACKAGES_TO_BUILD} ${DEPENDENT_PACKAGES_TO_BUILD})

list(REMOVE_DUPLICATES PACKAGES_TO_ADD)
list(SORT PACKAGES_TO_ADD)

add_package(libxml2)

if(NOT EMSCRIPTEN)
    add_package(OpenSSL)
endif()

add_package(zlib)

add_package(libSBML) # Requires libxml2.
add_package(libNuML) # Requires libSBML.

if(NOT EMSCRIPTEN)
    add_package(libssh2) # Requires OpenSSL.
endif()

add_package(zipper) # Requires zlib.

# Required third-party libraries.

add_package(libCellML) # Requires libxml2 and zlib.
add_package(libCOMBINE) # Requires libSBML, zipper and zlib.

if(NOT EMSCRIPTEN)
    add_package(libcurl) # Requires libssh2, OpenSSL and zlib.
endif()

add_package(libSEDML) # Requires libNuML and libSBML.

if(NOT EMSCRIPTEN)
    add_package(LLVMClang)
endif()

add_package(SUNDIALS)
