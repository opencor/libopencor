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

                set(DEPENDENT_PACKAGES "${DEPENDENT_PACKAGES};${DEPENDENT_PACKAGE}")

                list(SORT DEPENDENT_PACKAGES)

                set(DEPENDENT_PACKAGES "${DEPENDENT_PACKAGES}" CACHE INTERNAL "Dependent packages to be built.")
            endif()
        endforeach()
    endif()
endfunction()

function(replace_compiler_flag OLD NEW)
    set(OLD "(^| )${OLD}($| )")

    if(NOT "${NEW}" STREQUAL "")
        set(NEW " ${NEW} ")
    endif()

    foreach(VAR CMAKE_CXX_FLAGS
                CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL
                CMAKE_CXX_FLAGS_RELWITHDEBINFO
                CMAKE_CXX_FLAGS_DEBUG)
        if("${${VAR}}" MATCHES "${OLD}")
            string(REGEX REPLACE "${OLD}" "${NEW}" ${VAR} "${${VAR}}")
        endif()

        set(${VAR} "${${VAR}}" PARENT_SCOPE)
    endforeach()
endfunction()

function(configure_target TARGET)
    # Treat warnings as errors.

    if(LIBOPENCOR_WARNINGS_AS_ERRORS)
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
            set(COMPILE_OPTIONS /W4 /WX)
        elseif(   "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
               OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
               OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
            set(COMPILE_OPTIONS -Wall -W -Werror)
        endif()

        if(COMPILE_OPTIONS)
            target_compile_options(${TARGET} PRIVATE
                                   ${COMPILE_OPTIONS})
        endif()
    endif()

    # Analyse the code.

    if(LIBOPENCOR_CODE_ANALYSIS)
        # Configure Clang.

        if(   "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
           OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
            # The full list of diagnostic flags for Clang can be found at
            # https://clang.llvm.org/docs/DiagnosticsReference.html.

            set(COMPILE_OPTIONS
                -Weverything
                -Wno-c++98-compat
                -Wno-exit-time-destructors
                -Wno-global-constructors
            )

            if(NOT "${TARGET}" STREQUAL "${CMAKE_PROJECT_NAME}")
                list(APPEND COMPILE_OPTIONS
                    -Wno-c++98-compat-pedantic
                )
            endif()

            target_compile_options(${TARGET} PRIVATE
                                   ${COMPILE_OPTIONS})
        endif()

        # Configure Clang-Tidy.

        if(CODE_ANALYSIS_AVAILABLE)
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
                -fuchsia-default-arguments-calls
                google-*
                hicpp-*
                llvm-*
                -llvm-header-guard
                -llvm-include-order
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
    endif()

    # Statically link our packages to the target.

    foreach(PACKAGE ${PACKAGES})
        string(TOUPPER "${PACKAGE}" PACKAGE_UC)

        if(    "${${PACKAGE_UC}_CMAKE_DIR}" STREQUAL ""
           AND "${${PACKAGE_UC}_CMAKE_PACKAGE_NAME}" STREQUAL "")
            # There are no CMake configuration files, so manually configure the
            # package using targets of the form <PACKAGE_NAME>::<LIBRARY_NAME>.

            foreach(PACKAGE_LIBRARY ${${PACKAGE_UC}_LIBRARY} ${${PACKAGE_UC}_LIBRARIES})
                if(NOT TARGET ${PACKAGE_LIBRARY})
                    add_library(${PACKAGE_LIBRARY} STATIC IMPORTED)

                    if(RELEASE_MODE)
                        set(LIBRARY_BUILD_TYPE RELEASE)
                    else()
                        set(LIBRARY_BUILD_TYPE DEBUG)
                    endif()

                    set_property(TARGET ${PACKAGE_LIBRARY}
                                 APPEND PROPERTY IMPORTED_CONFIGURATIONS ${LIBRARY_BUILD_TYPE})

                    string(REPLACE "::" ";" PACKAGE_LIBRARY_LIST ${PACKAGE_LIBRARY})
                    list(GET PACKAGE_LIBRARY_LIST 1 PACKAGE_LIBRARY_NAME)

                    set_target_properties(${PACKAGE_LIBRARY} PROPERTIES
                        IMPORTED_LOCATION_${LIBRARY_BUILD_TYPE} "${PREBUILT_DIR}/${PACKAGE}/lib/${PACKAGE_LIBRARY_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
                    )
                endif()
            endforeach()
        else()
            # There are some CMake configuration files, so use them to configure the
            # package.

            find_package(${${PACKAGE_UC}_CMAKE_PACKAGE_NAME} REQUIRED
                         PATHS ${${PACKAGE_UC}_CMAKE_DIR}
                         NO_SYSTEM_ENVIRONMENT_PATH
                         NO_CMAKE_ENVIRONMENT_PATH
                         NO_CMAKE_SYSTEM_PATH)
        endif()

        target_include_directories(${TARGET} PUBLIC
                                   $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/3rdparty/${PACKAGE}>
                                   $<BUILD_INTERFACE:${${PACKAGE_UC}_INCLUDE_DIR}>)

        target_link_libraries(${TARGET} PRIVATE
                              ${${PACKAGE_UC}_LIBRARY} ${${PACKAGE_UC}_LIBRARIES})

        if(${PACKAGE_UC}_DEFINITIONS)
            target_compile_definitions(${TARGET} PRIVATE
                                       ${${PACKAGE_UC}_DEFINITIONS})
        endif()
    endforeach()

    # Mark as advanced the CMake variables set by our various packages.

    mark_as_advanced(CURL_DIR
                     Clang_DIR
                     LLVM_DIR
                     Libssh2_DIR
                     SUNDIALS_DIR
                     combine-static_DIR
                     libCellML_DIR
                     numl-static_DIR
                     sbml-static_DIR
                     sedml-static_DIR)
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

    target_link_libraries(${TARGET} PRIVATE
                          gtest_main
                          ${CMAKE_PROJECT_NAME})

    configure_target(${TARGET})

    list(APPEND TEST_TARGETS "${TARGET}")

    set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)
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
