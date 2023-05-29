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

function(check_dependent_packages PACKAGE)
    string(TOUPPER "${PACKAGE}" PACKAGE_UC)

    if(NOT LIBOPENCOR_PREBUILT_${PACKAGE_UC})
        foreach(DEPENDENT_PACKAGE ${ARGN})
            string(TOUPPER "${DEPENDENT_PACKAGE}" DEPENDENT_PACKAGE_UC)

            if(LIBOPENCOR_PREBUILT_${DEPENDENT_PACKAGE_UC})
                set(LIBOPENCOR_PREBUILT_${DEPENDENT_PACKAGE_UC} OFF CACHE BOOL "${LIBOPENCOR_PREBUILT_${PACKAGE_UC}_DOCSTRING}" FORCE)

                set(DEPENDENT_PACKAGES_TO_BUILD "${DEPENDENT_PACKAGES_TO_BUILD};${DEPENDENT_PACKAGE}")

                list(SORT DEPENDENT_PACKAGES_TO_BUILD)

                set(DEPENDENT_PACKAGES_TO_BUILD "${DEPENDENT_PACKAGES_TO_BUILD}" CACHE INTERNAL "Dependent packages to be built.")
            endif()
        endforeach()
    endif()
endfunction()

function(build_package PACKAGE_NAME)
    # Configure and run a CMake script to build the package for us.

    message(STATUS "Building ${PACKAGE_NAME}")

    configure_file(${CMAKE_SOURCE_DIR}/cmake/buildpackage.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt)

    if(BUILDING_JAVASCRIPT_BINDINGS)
        set(CONFIGURE_COMMAND ${EMCMAKE_EXE} ${CMAKE_COMMAND})
    else()
        set(CONFIGURE_COMMAND ${CMAKE_COMMAND})
    endif()

    execute_process(COMMAND ${CONFIGURE_COMMAND} -G "${CMAKE_GENERATOR}" -S . -B build
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    RESULT_VARIABLE RESULT
                    ERROR_VARIABLE ERROR
                    OUTPUT_QUIET)

    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "${ERROR}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build build
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    RESULT_VARIABLE RESULT
                    ERROR_VARIABLE ERROR
                    OUTPUT_QUIET)

    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "${ERROR}")
    endif()

    message(STATUS "Building ${PACKAGE_NAME} - Success")
endfunction()

function(fix_import_prefix CONFIG_FILE)
    file(READ ${CONFIG_FILE} FILE_CONTENTS)

    string(REPLACE
           "# The installation prefix configured by this project.
set(_IMPORT_PREFIX \"${INSTALL_DIR}\")"
           "# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)
get_filename_component(_IMPORT_PREFIX \"\${_IMPORT_PREFIX}\" PATH)
get_filename_component(_IMPORT_PREFIX \"\${_IMPORT_PREFIX}\" PATH)
if(_IMPORT_PREFIX STREQUAL \"/\")
  set(_IMPORT_PREFIX \"\")
endif()"
           FILE_CONTENTS "${FILE_CONTENTS}")

    file(WRITE ${CONFIG_FILE} "${FILE_CONTENTS}")
endfunction()

function(create_package PACKAGE_NAME PACKAGE_VERSION PACKAGE_REPOSITORY RELEASE_TAG)
    # Create the package.

    message(STATUS "Creating ${PACKAGE_NAME} package")

    set(PACKAGE_FILENAME ${PACKAGE_NAME}.${PACKAGE_VERSION}.${TARGET_PLATFORM_ARCHITECTURE}.tar.gz)

    if(BUILDING_JAVASCRIPT_BINDINGS)
        set(PACKAGE_FILE ${CMAKE_BINARY_DIR}/../../../../${PACKAGE_FILENAME})
    else()
        set(PACKAGE_FILE ${CMAKE_BINARY_DIR}/${PACKAGE_FILENAME})
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} -E tar -czf ${PACKAGE_FILE} ${ARGN}
                    WORKING_DIRECTORY ${PREBUILT_DIR}/${PACKAGE_NAME}
                    RESULT_VARIABLE RESULT
                    ERROR_VARIABLE ERROR
                    OUTPUT_QUIET)

    # Calculate the SHA-1 value of our package, if it exists, and let people know how we should call the
    # retrieve_package() function.

    if(RESULT EQUAL 0 AND EXISTS ${PACKAGE_FILE})
        message(STATUS "Creating ${PACKAGE_NAME} package - Success")

        file(SHA1 ${PACKAGE_FILE} SHA1_VALUE)

        message("To retrieve the ${PACKAGE_NAME} package, use:
    retrieve_package(\$\{PACKAGE_NAME\} \$\{PACKAGE_VERSION\}
                     \$\{PACKAGE_REPOSITORY\} \$\{RELEASE_TAG\}
                     ${SHA1_VALUE})")
    else()
        if(EXISTS ${PACKAGE_FILE})
            file(REMOVE ${PACKAGE_FILE})
        endif()

        message(FATAL_ERROR "The ${PACKAGE_NAME} package could not be created.")
    endif()
endfunction()

function(add_package PACKAGE)
    add_subdirectory(${PACKAGE})

    # Keep track of the package.

    set(AVAILABLE_PACKAGES "${AVAILABLE_PACKAGES};${PACKAGE}")

    list(SORT AVAILABLE_PACKAGES)

    set(AVAILABLE_PACKAGES "${AVAILABLE_PACKAGES}" CACHE INTERNAL "Available packages.")

    # Keep track of the package's (special) header files.

    file(GLOB PACKAGE_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE}/*.h")

    set(THIRD_PARTY_HEADER_FILES "${THIRD_PARTY_HEADER_FILES};${PACKAGE_HEADER_FILES}")

    set(THIRD_PARTY_HEADER_FILES "${THIRD_PARTY_HEADER_FILES}" CACHE INTERNAL "Third-party (special) header files.")
endfunction()

function(retrieve_package PACKAGE_NAME PACKAGE_VERSION PACKAGE_REPOSITORY RELEASE_TAG SHA1_VALUE)
    # Check whether the package has already been retrieved by simply checking whether its installation directory exists.

    set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

    message(STATUS "Retrieving Package ${PACKAGE_NAME}")

    if(EXISTS ${INSTALL_DIR})
        message(STATUS "Retrieving Package ${PACKAGE_NAME} - Success")
    else()
        # Retrieve the package.

        set(PACKAGE_FILE ${PACKAGE_NAME}.${PACKAGE_VERSION}.${TARGET_PLATFORM_ARCHITECTURE}.tar.gz)
        set(REAL_PACKAGE_FILE ${INSTALL_DIR}/${PACKAGE_FILE})
        set(PACKAGE_URL "https://github.com/opencor/${PACKAGE_REPOSITORY}/releases/download/${RELEASE_TAG}/${PACKAGE_FILE}")

        file(DOWNLOAD ${PACKAGE_URL} ${REAL_PACKAGE_FILE} STATUS STATUS)

        # Uncompress the package, should we have managed to retrieve it.

        list(GET STATUS 0 STATUS_CODE)

        if(${STATUS_CODE} EQUAL 0)
            file(SHA1 ${REAL_PACKAGE_FILE} REAL_SHA1_VALUE)

            if(NOT "${REAL_SHA1_VALUE}" STREQUAL "${SHA1_VALUE}")
                file(REMOVE_RECURSE ${INSTALL_DIR})

                message(STATUS "Retrieving Package ${PACKAGE_NAME} - Failed")
                message(FATAL_ERROR "The ${PACKAGE_NAME} package (downloaded from ${PACKAGE_URL}) does not have the expected SHA-1 value.")
            endif()

            execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${REAL_PACKAGE_FILE}
                            WORKING_DIRECTORY ${INSTALL_DIR}
                            RESULT_VARIABLE RESULT
                            OUTPUT_QUIET ERROR_QUIET)

            if(NOT RESULT EQUAL 0)
                file(REMOVE_RECURSE ${INSTALL_DIR})

                message(STATUS "Retrieving Package ${PACKAGE_NAME} - Failed")
                message(FATAL_ERROR "The ${PACKAGE_NAME} package (downloaded from ${PACKAGE_URL}) could not be uncompressed.")
            endif()

            file(REMOVE ${REAL_PACKAGE_FILE})

            message(STATUS "Retrieving Package ${PACKAGE_NAME} - Success")
        else()
            file(REMOVE_RECURSE ${INSTALL_DIR})

            message(STATUS "Retrieving Package ${PACKAGE_NAME} - Failed")
            message(FATAL_ERROR "The ${PACKAGE_NAME} package could not be retrieved from ${PACKAGE_URL}.")
        endif()
    endif()

    # Create a dummy target for our package.
    # Note: this is needed so that we can build a package that depends on other packages that have been retrieved.

    add_custom_target(${PACKAGE_NAME})
endfunction()

function(escape_path OLD_PATH NEW_PATH)
    # Escape spaces and parentheses in a path.

    string(REPLACE " " "\\ " NEW_PATH_VALUE "${OLD_PATH}")
    string(REPLACE "(" "\\(" NEW_PATH_VALUE "${NEW_PATH_VALUE}")
    string(REPLACE ")" "\\)" NEW_PATH_VALUE "${NEW_PATH_VALUE}")

    set(${NEW_PATH} ${NEW_PATH_VALUE} PARENT_SCOPE)
endfunction()

# Determine the platform on which we are and the architecture on which we want to build.

if(BUILDING_JAVASCRIPT_BINDINGS)
    set(TARGET_PLATFORM_ARCHITECTURE wasm)
else()
    if(WIN32)
        if(RELEASE_MODE)
            set(TARGET_PLATFORM_ARCHITECTURE windows.release)
        else()
            set(TARGET_PLATFORM_ARCHITECTURE windows.debug)
        endif()
    elseif(APPLE)
        set(TARGET_PLATFORM_ARCHITECTURE macos)
    else()
        set(TARGET_PLATFORM_ARCHITECTURE linux)
    endif()

    if("${LIBOPENCOR_TARGET_ARCHITECTURE}" STREQUAL "Intel")
        set(TARGET_PLATFORM_ARCHITECTURE ${TARGET_PLATFORM_ARCHITECTURE}.intel)
    else()
        set(TARGET_PLATFORM_ARCHITECTURE ${TARGET_PLATFORM_ARCHITECTURE}.arm)
    endif()
endif()

# Location of our prebuilt packages.

set(PREBUILT_DIR ${CMAKE_SOURCE_DIR}/prebuilt/${TARGET_PLATFORM_ARCHITECTURE})

if(WIN32)
    if(RELEASE_MODE)
        set(PREBUILT_DIR ${PREBUILT_DIR}/release)
    else()
        set(PREBUILT_DIR ${PREBUILT_DIR}/debug)
    endif()
endif()

set(PREBUILT_DIR "${PREBUILT_DIR}" CACHE INTERNAL "Prebuilt directory.")

# Build our third-party libraries the same way that we build libOpenCOR.
# Note: the build type on Linux/macOS is always Release since we don't need to debug third-party libraries and a debug
#       library can use release libraries.

include(ExternalProject)

if(NOT WIN32 AND NOT APPLE AND NOT BUILDING_JAVASCRIPT_BINDINGS)
    set(CMAKE_C_FLAGS_ARGS -DCMAKE_C_FLAGS=-fPIC)
    set(CMAKE_CXX_FLAGS_ARGS -DCMAKE_CXX_FLAGS=-fPIC)
endif()

set(CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${LIBOPENCOR_BUILD_TYPE}
    ${CMAKE_C_FLAGS_ARGS}
    ${CMAKE_CXX_FLAGS_ARGS}
)

if(CMAKE_C_COMPILER_LAUNCHER AND CMAKE_CXX_COMPILER_LAUNCHER)
    escape_path(${CMAKE_C_COMPILER_LAUNCHER} EP_CMAKE_C_COMPILER_LAUNCHER)
    escape_path(${CMAKE_CXX_COMPILER_LAUNCHER} EP_CMAKE_CXX_COMPILER_LAUNCHER)

    list(APPEND CMAKE_ARGS
        -DCMAKE_C_COMPILER_LAUNCHER=${EP_CMAKE_C_COMPILER_LAUNCHER}
        -DCMAKE_CXX_COMPILER_LAUNCHER=${EP_CMAKE_CXX_COMPILER_LAUNCHER}
    )
endif()

if(BUILDING_JAVASCRIPT_BINDINGS)
    list(APPEND CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_CROSSCOMPILING_EMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}
    )
else()
    escape_path(${CMAKE_C_COMPILER} EP_CMAKE_C_COMPILER)
    escape_path(${CMAKE_CXX_COMPILER} EP_CMAKE_CXX_COMPILER)

    list(APPEND CMAKE_ARGS
        -DCMAKE_C_COMPILER=${EP_CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${EP_CMAKE_CXX_COMPILER}
    )

    if(APPLE)
        list(APPEND CMAKE_ARGS
            -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        )
    endif()
endif()

# Get ready to use nmake/make for some third-party libraries (e.g., OpenSSL).

if(WIN32)
    set(MAKE_NMAKE_COMMAND nmake)
else()
    set(MAKE_NMAKE_COMMAND make)

    if(APPLE)
        list(APPEND MAKE_NMAKE_COMMAND
            SDKROOT=${CMAKE_OSX_SYSROOT}
        )
    endif()

    include(ProcessorCount)

    ProcessorCount(PROCESSOR_COUNT)

    if(NOT PROCESSOR_COUNT EQUAL 0)
        list(APPEND MAKE_NMAKE_COMMAND
            -j ${PROCESSOR_COUNT}
        )
    endif()
endif()
