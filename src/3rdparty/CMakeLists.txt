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

set(PACKAGES_TO_BUILD)

foreach(PACKAGE libCellML libCOMBINE libcurl libNuML libSBML libSEDML libssh2 libxml2 LLVMClang OpenSSL SUNDIALS zipper zlib)
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
check_dependent_packages(OpenSSL libcurl libssh2)
check_dependent_packages(zlib libCellML libCOMBINE libcurl zipper)
check_dependent_packages(libSBML libCOMBINE libNuML libSEDML)
check_dependent_packages(libNuML libSEDML)
check_dependent_packages(libssh2 libcurl)
check_dependent_packages(zipper libCOMBINE)

if(NOT "${DEPENDENT_PACKAGES_TO_BUILD}" STREQUAL "")
    message(STATUS "The following dependent packages are also to be built:")

    foreach(DEPENDENT_PACKAGE_TO_BUILD ${DEPENDENT_PACKAGES_TO_BUILD})
        message(STATUS " - ${DEPENDENT_PACKAGE_TO_BUILD}")
    endforeach()
endif()

# Initialise/reset our available packages and third-party (special) header files.

set(AVAILABLE_PACKAGES "" CACHE INTERNAL "Available packages.")
set(THIRD_PARTY_HEADER_FILES "" CACHE INTERNAL "Third-party (special) header files.")

# Prerequisite third-party libraries, respecting their own prerequisites.

add_package(libxml2)
add_package(OpenSSL)
add_package(zlib)

add_package(libSBML) # Requires libxml2.

add_package(libNuML) # Requires libSBML.

add_package(libssh2) # Requires OpenSSL.

add_package(zipper) # Requires zlib.

# Required third-party libraries.

add_package(libCellML) # Requires libxml2 and zlib.
add_package(libCOMBINE) # Requires libSBML, zipper and zlib.
add_package(libcurl) # Requires libssh2, OpenSSL and zlib.
add_package(libSEDML) # Requires libNuML and libSBML.
add_package(LLVMClang)
add_package(SUNDIALS)
