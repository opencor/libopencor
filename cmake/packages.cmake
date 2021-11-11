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

function(create_package PACKAGE_NAME PACKAGE_VERSION PACKAGE_REPOSITORY RELEASE_TAG)
    # Parse our different arguments.

    set(OPTIONS)
    set(ONE_VALUE_KEYWORDS)
    set(MULTI_VALUE_KEYWORDS
        PACKAGED_FILES
        SHA1_FILES
    )

    cmake_parse_arguments(ARG "${OPTIONS}" "${ONE_VALUE_KEYWORDS}" "${MULTI_VALUE_KEYWORDS}" ${ARGN})

    # Make sure that we have at least one SHA-1 file.

    list(LENGTH ARG_SHA1_FILES ARG_SHA1_FILES_COUNT)

    if(ARG_SHA1_FILES_COUNT EQUAL 0)
        message(FATAL_ERROR "The ${PACKAGE_NAME} package must have at least one SHA-1 file.")
    endif()

    # Configure and run a CMake script to do the packaging for us, once the
    # corresponding target has been built.

    set(CREATE_PACKAGE_SCRIPT ${CMAKE_BINARY_DIR}/cmake/create${PACKAGE_NAME}package.cmake)

    configure_file(${CMAKE_SOURCE_DIR}/cmake/createpackage.cmake.in ${CREATE_PACKAGE_SCRIPT} @ONLY)

    add_custom_command(TARGET ${PACKAGE_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -P ${CREATE_PACKAGE_SCRIPT})
endfunction()

function(check_sha1_files PACKAGE_NAME SHA1_FILES SHA1_VALUES OK INVALID_SHA1_FILES MISSING_SHA1_FILES)
    # By default, everything is OK.

    set(OK_VALUE TRUE)
    set(INVALID_SHA1_FILES_VALUE)
    set(MISSING_SHA1_FILES_VALUE)

    # Go through the SHA-1 files and, if they exist, make sure that they have
    # the expected SHA-1 value.

    list(LENGTH SHA1_FILES SHA1_FILES_COUNT)

    if(SHA1_FILES_COUNT)
        math(EXPR RANGE "${SHA1_FILES_COUNT}-1")

        foreach(I RANGE ${RANGE})
            list(GET SHA1_FILES ${I} SHA1_FILE)
            list(GET SHA1_VALUES ${I} SHA1_VALUE)

            set(REAL_SHA1_FILE ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}/${SHA1_FILE})

            if(EXISTS ${REAL_SHA1_FILE})
                file(SHA1 ${REAL_SHA1_FILE} REAL_SHA1_VALUE)

                if(NOT "${REAL_SHA1_VALUE}" STREQUAL "${SHA1_VALUE}")
                    # The SHA-1 file doesn't have the expected SHA-1 value, so
                    # remove it.

                    file(REMOVE ${REAL_SHA1_FILE})

                    set(OK_VALUE FALSE)

                    list(APPEND INVALID_SHA1_FILES_VALUE ${SHA1_FILE})
                endif()
            else()
                # The SHA-1 file is missing.

                set(OK_VALUE FALSE)

                list(APPEND MISSING_SHA1_FILES_VALUE ${SHA1_FILE})
            endif()
        endforeach()
    endif()

    # Return the results of our checks.

    set(${OK} ${OK_VALUE} PARENT_SCOPE)
    set(${INVALID_SHA1_FILES} ${INVALID_SHA1_FILES_VALUE} PARENT_SCOPE)
    set(${MISSING_SHA1_FILES} ${MISSING_SHA1_FILES_VALUE} PARENT_SCOPE)
endfunction()

function(check_sha1_file PACKAGE_NAME SHA1_FILE SHA1_VALUE OK)
    # Convenience function to check one SHA-1 file.

    check_sha1_files(${PACKAGE_NAME} ${SHA1_FILE} ${SHA1_VALUE} OK_VALUE INVALID_SHA1_FILES_VALUE MISSING_SHA1_FILES_VALUE)

    set(${OK} ${OK_VALUE} PARENT_SCOPE)
endfunction()

function(retrieve_package PACKAGE_NAME PACKAGE_VERSION PACKAGE_REPOSITORY RELEASE_TAG SHA1_VALUE)
    # Parse our different arguments.

    set(OPTIONS)
    set(ONE_VALUE_KEYWORDS)
    set(MULTI_VALUE_KEYWORDS
        SHA1_FILES
        SHA1_VALUES
    )

    cmake_parse_arguments(ARG "${OPTIONS}" "${ONE_VALUE_KEYWORDS}" "${MULTI_VALUE_KEYWORDS}" ${ARGN})

    # Make sure that we have at least one SHA-1 file and value.

    list(LENGTH ARG_SHA1_FILES ARG_SHA1_FILES_COUNT)
    list(LENGTH ARG_SHA1_VALUES ARG_SHA1_VALUES_COUNT)

    if(       ARG_SHA1_FILES_COUNT EQUAL 0
       OR NOT ARG_SHA1_FILES_COUNT EQUAL ARG_SHA1_VALUES_COUNT)
        message(FATAL_ERROR "The ${PACKAGE_NAME} package must have at least one SHA-1 file and value.")
    endif()

    # Create our installation directory, if needed.

    set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

    if(NOT EXISTS ${INSTALL_DIR})
        file(MAKE_DIRECTORY ${INSTALL_DIR})
    endif()

    # Check whether the package has already been retrieved.

    check_sha1_files(${PACKAGE_NAME} "${ARG_SHA1_FILES}" "${ARG_SHA1_VALUES}" OK INVALID_SHA1_FILES MISSING_SHA1_FILES)

    if(NOT OK)
        # Retrieve the package.

        message(STATUS "Retrieving Package ${PACKAGE_NAME}")

        set(PACKAGE_FILE ${PACKAGE_NAME}.${PACKAGE_VERSION}.${TARGET_PLATFORM}.tar.gz)
        set(FULL_PACKAGE_FILE ${INSTALL_DIR}/${PACKAGE_FILE})
        set(PACKAGE_URL "https://github.com/opencor/${PACKAGE_REPOSITORY}/releases/download/${RELEASE_TAG}/${PACKAGE_FILE}")

        file(DOWNLOAD ${PACKAGE_URL} ${FULL_PACKAGE_FILE} STATUS STATUS)

        # Uncompress the package, should we have managed to retrieve it.

        list(GET STATUS 0 STATUS_CODE)

        if(${STATUS_CODE} EQUAL 0)
            check_sha1_file(${PACKAGE_NAME} ${PACKAGE_FILE} ${SHA1_VALUE} OK)

            if(NOT OK)
                message(STATUS "Retrieving Package ${PACKAGE_NAME} - Failed")
                message(FATAL_ERROR "The ${PACKAGE_NAME} package (downloaded from ${PACKAGE_URL}) does not have the expected SHA-1 value.")
            endif()

            execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${FULL_PACKAGE_FILE}
                            WORKING_DIRECTORY ${INSTALL_DIR}
                            RESULT_VARIABLE RESULT
                            OUTPUT_QUIET ERROR_QUIET)

            if(NOT RESULT EQUAL 0)
                message(STATUS "Retrieving Package ${PACKAGE_NAME} - Failed")
                message(FATAL_ERROR "The ${PACKAGE_NAME} package (downloaded from ${PACKAGE_URL}) could not be uncompressed.")
            endif()

            file(REMOVE ${FULL_PACKAGE_FILE})
        else()
            file(REMOVE ${FULL_PACKAGE_FILE})
            # Note: this is in case we had an HTTP/S error of sorts, in which
            #       case we would end up with an empty file.

            message(STATUS "Retrieving Package ${PACKAGE_NAME} - Failed")
            message(FATAL_ERROR "The ${PACKAGE_NAME} package could not be retrieved from ${PACKAGE_URL}.")
        endif()

        # Check that the package's files, if we managed to uncompress the
        # package, have the expected SHA-1 values.

        check_sha1_files(${PACKAGE_NAME} "${ARG_SHA1_FILES}" "${ARG_SHA1_VALUES}" OK INVALID_SHA1_FILES MISSING_SHA1_FILES)

        if(OK)
            message(STATUS "Retrieving Package ${PACKAGE_NAME} - Success")
        else()
            message(STATUS "Retrieving Package ${PACKAGE_NAME} - Failed")
            message(STATUS "The ${PACKAGE_NAME} package (downloaded from ${PACKAGE_URL}) is invalid:")

            foreach(SHA1_FILE ${ARG_SHA1_FILES})
                if(${SHA1_FILE} IN_LIST INVALID_SHA1_FILES)
                    message(STATUS " - ${SHA1_FILE} does not have the expected SHA-1 value.")
                elseif(${SHA1_FILE} IN_LIST MISSING_SHA1_FILES)
                    message(STATUS " - ${SHA1_FILE} is missing.")
                endif()
            endforeach()

            message(FATAL_ERROR)
        endif()
    endif()
endfunction()

# Get ready to build external projects.

include(ExternalProject)

# Determine whether we are building in release mode.
# Note: this only makes sense if we are not in a multi-configuration mode, i.e.
#       IS_MULTI_CONFIG is equal to FALSE, which is likely to be the case when
#       building a third-party library.

if("${LIBOPENCOR_BUILD_TYPE}" STREQUAL "Debug")
    set(RELEASE_MODE FALSE)
else()
    set(RELEASE_MODE TRUE)
endif()

# Determine the platform on which we are.

if(WIN32)
    if(RELEASE_MODE)
        set(TARGET_PLATFORM windows.release)
    else()
        set(TARGET_PLATFORM windows.debug)
    endif()
elseif(APPLE)
    set(TARGET_PLATFORM macos)
else()
    set(TARGET_PLATFORM linux)
endif()

# Build our third-party libraries the same way that we build libOpenCOR.
# Note: the build type on Linux/macOS is always Release since we don't need to
#       debug third-party libraries and a debug library can use release
#       libraries.

if(WIN32)
    set(EP_CMAKE_BUILD_TYPE ${LIBOPENCOR_BUILD_TYPE})
else()
    set(EP_CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_BUILD_TYPE=${EP_CMAKE_BUILD_TYPE}
)

if(CMAKE_C_COMPILER_LAUNCHER AND CMAKE_CXX_COMPILER_LAUNCHER)
    list(APPEND CMAKE_ARGS
        -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
        -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    )
endif()

# Get ready to use nmake/make for some third-party libraries (e.g., OpenSSL).

if(WIN32)
    set(MAKE_NMAKE_COMMAND nmake)
else()
    set(MAKE_NMAKE_COMMAND make)

    include(ProcessorCount)

    ProcessorCount(PROCESSOR_COUNT)

    if(NOT PROCESSOR_COUNT EQUAL 0)
        set(MAKE_NMAKE_COMMAND ${MAKE_NMAKE_COMMAND} -j ${PROCESSOR_COUNT})
    endif()
endif()
