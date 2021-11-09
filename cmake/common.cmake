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

function(replace_compiler_flag OLD NEW)
    set(OLD "(^| )${OLD}($| )")

    if(NOT "${NEW}" STREQUAL "")
        set(NEW " ${NEW} ")
    endif()

    foreach(VAR CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if("${${VAR}}" MATCHES "${OLD}")
            string(REGEX REPLACE "${OLD}" "${NEW}" ${VAR} "${${VAR}}")
        endif()

        set(${VAR} "${${VAR}}" PARENT_SCOPE)
    endforeach()
endfunction()

function(treat_warnings_as_errors TARGET)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(COMPILE_OPTIONS /W4 /WX)
    elseif(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
           OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
           OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        set(COMPILE_OPTIONS -Wall -W -Werror)
    endif()

    if(COMPILE_OPTIONS)
        target_compile_options(${TARGET} PRIVATE ${COMPILE_OPTIONS})
    endif()
endfunction()

function(configure_clang_and_clang_tidy TARGET)
    # Configure Clang.

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
       OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        # The full list of diagnostic flags for Clang can be found at
        # https://clang.llvm.org/docs/DiagnosticsReference.html.

        set(COMPILE_OPTIONS
            -Weverything
            -Wno-exit-time-destructors
            -Wno-global-constructors
        )

        if(NOT "${TARGET}" STREQUAL "${CMAKE_PROJECT_NAME}")
            list(APPEND COMPILE_OPTIONS
                -Wno-c++98-compat
                -Wno-c++98-compat-pedantic
            )
        endif()

        target_compile_options(${TARGET} PRIVATE ${COMPILE_OPTIONS})
    endif()

    # Configure Clang-Tidy.

    if(CLANG_TIDY_AVAILABLE)
        # The full list of Clang-Tidy checks can be found at
        # https://clang.llvm.org/extra/clang-tidy/checks/list.html.

        if(NOT "${TARGET}" STREQUAL "${CMAKE_PROJECT_NAME}")
            set(DISABLED_CERT_CHECKS
                -cert-err58-cpp
            )
            set(DISABLED_CPPCOREGUIDELINES_CHECKS
                -cppcoreguidelines-avoid-non-const-global-variables
                -cppcoreguidelines-owning-memory
            )
            set(DISABLED_FUCHSIA_CHECKS
                -fuchsia-statically-constructed-objects
            )
        endif()

        set(CLANG_TIDY_CHECKS
            -*
            bugprone-*
            cert-*
            ${DISABLED_CERT_CHECKS}
            cppcoreguidelines-*
            ${DISABLED_CPPCOREGUIDELINES_CHECKS}
            fuchsia-*
            ${DISABLED_FUCHSIA_CHECKS}
            google-*
            hicpp-*
            llvm-*
            misc-*
            modernize-*
            -modernize-use-trailing-return-type
            performance-*
            readability-*
        )

        string(REPLACE ";" "," CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS}")

        if(LIBOPENCOR_WARNINGS_AS_ERRORS)
            set(CLANG_TIDY_WARNINGS_AS_ERRORS ";-warnings-as-errors=${CLANG_TIDY_CHECKS}")
        endif()

        if("${CMAKE_GENERATOR}" STREQUAL "Ninja")
            set(HEADER_FILTER_DIR ..)
        else()
            set(HEADER_FILTER_DIR ${CMAKE_SOURCE_DIR})
        endif()

        set(HEADER_FILTER_DIR "${HEADER_FILTER_DIR}/src/")

        string(REPLACE "." "\\\." HEADER_FILTER_DIR "${HEADER_FILTER_DIR}")
        string(REPLACE "/" "\\\/" HEADER_FILTER_DIR "${HEADER_FILTER_DIR}")

        if(MSVC)
            # Extra argument for Clang-Tidy when using MSVC.
            # See https://gitlab.kitware.com/cmake/cmake/-/issues/20512#note_722771.

            set(EXTRA_ARG ";--extra-arg=/EHsc")
        endif()

        set_target_properties(${TARGET} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}${EXTRA_ARG};-checks=${CLANG_TIDY_CHECKS};-header-filter=${HEADER_FILTER_DIR}.*${CLANG_TIDY_WARNINGS_AS_ERRORS}"
        )
    endif()
endfunction()

function(check_python_package PACKAGE AVAILABLE)
    set(PACKAGE_VARIABLE "HAS_PYTHON_${PACKAGE}")

    string(TOUPPER "${PACKAGE_VARIABLE}" PACKAGE_VARIABLE)
    string(REPLACE "-" "_" PACKAGE_VARIABLE "${PACKAGE_VARIABLE}")

    message(STATUS "Performing Test ${PACKAGE_VARIABLE}")

    execute_process(COMMAND ${PYTHON_EXE} ${CMAKE_SOURCE_DIR}/cmake/check_python_package.py ${PACKAGE}
                    RESULT_VARIABLE RESULT
                    OUTPUT_QUIET ERROR_QUIET)

    if(RESULT EQUAL 0)
        set(${AVAILABLE} TRUE PARENT_SCOPE)

        message(STATUS "Performing Test ${PACKAGE_VARIABLE} - Success")
    else()
        message(STATUS "Performing Test ${PACKAGE_VARIABLE} - Failed")
    endif()
endfunction()

function(create_package_file PACKAGE_NAME PACKAGE_VERSION)
    # Parse our different arguments.

    set(OPTIONS)
    set(ONE_VALUE_KEYWORDS
        PACKAGE_REPOSITORY
        RELEASE_TAG
    )
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

function(check_sha1_files INSTALL_DIR SHA1_FILES SHA1_VALUES OK INVALID_SHA1_FILES MISSING_SHA1_FILES)
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

            set(REAL_SHA1_FILE ${INSTALL_DIR}/${SHA1_FILE})

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

function(check_sha1_file INSTALL_DIR SHA1_FILE SHA1_VALUE OK)
    # Convenience function to check one SHA-1 file.

    check_sha1_files(${INSTALL_DIR} ${SHA1_FILE} ${SHA1_VALUE} OK_VALUE INVALID_SHA1_FILES_VALUE MISSING_SHA1_FILES_VALUE)

    set(${OK} ${OK_VALUE} PARENT_SCOPE)
endfunction()

function(retrieve_package_file PACKAGE_NAME PACKAGE_VERSION PACKAGE_REPOSITORY RELEASE_TAG INSTALL_DIR SHA1_VALUE)
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

    if(NOT EXISTS ${INSTALL_DIR})
        file(MAKE_DIRECTORY ${INSTALL_DIR})
    endif()

    # Check whether the package has already been retrieved.

    check_sha1_files(${INSTALL_DIR} "${ARG_SHA1_FILES}" "${ARG_SHA1_VALUES}" OK INVALID_SHA1_FILES MISSING_SHA1_FILES)

    if(NOT OK)
        # Retrieve the package.

        message(STATUS "Retrieving ${PACKAGE_NAME} package")

        set(PACKAGE_FILE ${PACKAGE_NAME}.${PACKAGE_VERSION}.${TARGET_PLATFORM}.tar.gz)
        set(FULL_PACKAGE_FILE ${INSTALL_DIR}/${PACKAGE_FILE})
        set(PACKAGE_URL "https://github.com/opencor/${PACKAGE_REPOSITORY}/releases/download/${RELEASE_TAG}/${PACKAGE_FILE}")

        file(DOWNLOAD ${PACKAGE_URL} ${FULL_PACKAGE_FILE}
             SHOW_PROGRESS STATUS STATUS)

        # Uncompress the package, should we have managed to retrieve it.

        list(GET STATUS 0 STATUS_CODE)

        if(${STATUS_CODE} EQUAL 0)
            check_sha1_file(${INSTALL_DIR} ${PACKAGE_FILE} ${SHA1_VALUE} OK)

            if(OK)
                message(STATUS "Retrieving ${PACKAGE_NAME} package - Success")
            else()
                message(STATUS "Retrieving ${PACKAGE_NAME} package - Failed")
                message(FATAL_ERROR "The ${PACKAGE_NAME} package (downloaded from ${PACKAGE_URL}) does not have the expected SHA-1 value.")
            endif()

            execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${FULL_PACKAGE_FILE}
                            WORKING_DIRECTORY ${INSTALL_DIR}
                            RESULT_VARIABLE RESULT
                            OUTPUT_QUIET ERROR_QUIET)

            file(REMOVE ${FULL_PACKAGE_FILE})
        else()
            file(REMOVE ${FULL_PACKAGE_FILE})
            # Note: this is in case we had an HTTP/S error of sorts, in which
            #       case we would end up with an empty file.

            message(STATUS "Retrieving ${PACKAGE_NAME} package - Failed")
            message(FATAL_ERROR "The ${PACKAGE_NAME} package could not be retrieved from ${PACKAGE_URL}.")
        endif()

        # Check that the package's files, if we managed to uncompress the
        # package, have the expected SHA-1 values.

        if(RESULT EQUAL 0)
            check_sha1_files(${INSTALL_DIR} ${ARG_SHA1_FILES} ${ARG_SHA1_VALUES} OK INVALID_SHA1_FILES MISSING_SHA1_FILES)

            if(NOT OK)
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
        else()
            message(FATAL_ERROR "The ${PACKAGE_NAME} package (downloaded from ${PACKAGE_URL}) could not be uncompressed.")
        endif()
    endif()
endfunction()

macro(add_target TARGET)
    add_custom_target(${TARGET} ${ARGN})

    set(TARGETS "${TARGETS};${TARGET}")

    list(SORT TARGETS)

    set(TARGETS "${TARGETS}" CACHE INTERNAL "Available targets.")
endmacro()

function(add_target_property TARGET PROPERTY VALUE)
    get_target_property(OLD_VALUE ${TARGET} ${PROPERTY})

    if(OLD_VALUE)
        set(NEW_VALUE "${OLD_VALUE} ${VALUE}")
    else()
        set(NEW_VALUE "${VALUE}")
    endif()

    set_target_properties(${TARGET} PROPERTIES ${PROPERTY} "${NEW_VALUE}")
endfunction()

function(prepare_test TARGET)
    # Prepare the given test.

    add_executable(${TARGET}
                   ${ARGN})

    target_link_libraries(${TARGET}
                          PRIVATE gtest_main
                          PRIVATE ${CMAKE_PROJECT_NAME})

    list(APPEND TEST_TARGETS "${TARGET}")

    set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)

    # Treat warnings as errors for the given test.

    if(LIBOPENCOR_WARNINGS_AS_ERRORS)
        treat_warnings_as_errors(${TARGET})
    endif()

    # Analyse the code of the given test.

    if(LIBOPENCOR_CODE_ANALYSIS)
        configure_clang_and_clang_tidy(${TARGET})
    endif()
endfunction()

function(build_documentation DOCUMENTATION_NAME)
    # Build the given documentation as an external project and have it copied to
    # our final documentation directory.

    set(DOCUMENTATION_BUILD ${DOCUMENTATION_NAME}_documentation)

    ExternalProject_Add(${DOCUMENTATION_BUILD}
                        SOURCE_DIR ${EXTERNAL_PROJECT_BUILD_DIR}/doc/${DOCUMENTATION_NAME}
                        GIT_REPOSITORY https://github.com/opencor/libopencor-${DOCUMENTATION_NAME}-documentation
                        CMAKE_ARGS -DPYTHON_EXECUTABLE=${PYTHON_EXE}
                                   -DSPHINX_EXECUTABLE=${SPHINX_EXE}
                        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_PROJECT_BUILD_DIR}/Build/${DOCUMENTATION_BUILD}/html
                                                                           ${CMAKE_BINARY_DIR}/doc/html/${DOCUMENTATION_NAME})
endfunction()
