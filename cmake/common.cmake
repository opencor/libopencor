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
        if(BUILDING_USING_MSVC)
            set(COMPILE_OPTIONS
                /W4
                /WX
            )
        elseif(BUILDING_USING_GNU OR BUILDING_USING_CLANG)
            set(COMPILE_OPTIONS
                -Wall
                -W
                -Werror
                -Wno-unknown-warning-option
            )
        endif()

        if(COMPILE_OPTIONS)
            target_compile_options(${TARGET} PRIVATE
                                   ${COMPILE_OPTIONS})
        endif()
    endif()

    # Analyse the code.

    if(LIBOPENCOR_CODE_ANALYSIS)
        # Configure Clang.

        if(BUILDING_USING_CLANG)
            # The full list of diagnostic flags for Clang can be found at
            # https://clang.llvm.org/docs/DiagnosticsReference.html.

            set(COMPILE_OPTIONS
                -Weverything
                -Wno-c++98-compat
                -Wno-exit-time-destructors
                -Wno-global-constructors
                -Wno-padded
            )

            if(NOT "${TARGET}" STREQUAL "${CMAKE_PROJECT_NAME}")
                list(APPEND COMPILE_OPTIONS
                    -Wno-c++98-compat-pedantic
                    -Wno-weak-vtables
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
                set(DISABLED_BUGPRONE_CHECKS
                    -bugprone-reserved-identifier
                )
                set(DISABLED_CERT_CHECKS
                    -cert-err58-cpp
                )
                set(DISABLED_CPPCOREGUIDELINES_CHECKS
                    -cppcoreguidelines-avoid-non-const-global-variables
                    -cppcoreguidelines-non-private-member-variables-in-classes
                    -cppcoreguidelines-owning-memory
                    -cppcoreguidelines-pro-type-reinterpret-cast
                )
                set(DISABLED_FUCHSIA_CHECKS
                    -fuchsia-statically-constructed-objects
                )
                set(MISC_CHECKS
                    -misc-non-private-member-variables-in-classes
                )
                set(READABILITY_CHECKS
                    -readability-function-cognitive-complexity
                )
            endif()

            set(CLANG_TIDY_CHECKS
                -*
                bugprone-*
                ${DISABLED_BUGPRONE_CHECKS}
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
                ${MISC_CHECKS}
                modernize-*
                -modernize-use-trailing-return-type
                performance-*
                readability-*
                ${READABILITY_CHECKS}
            )

            string(REPLACE ";" "," CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS}")

            if(LIBOPENCOR_WARNINGS_AS_ERRORS)
                set(CLANG_TIDY_WARNINGS_AS_ERRORS "--warnings-as-errors=${CLANG_TIDY_CHECKS}")
            endif()

            if("${CMAKE_GENERATOR}" STREQUAL "Ninja")
                set(HEADER_FILTER_DIR ..)
            else()
                set(HEADER_FILTER_DIR ${CMAKE_SOURCE_DIR})
            endif()

            set(HEADER_FILTER_DIR "${HEADER_FILTER_DIR}/src/")

            string(REPLACE "." "\\\." HEADER_FILTER_DIR "${HEADER_FILTER_DIR}")
            string(REPLACE "/" "\\\/" HEADER_FILTER_DIR "${HEADER_FILTER_DIR}")

            if(BUILDING_USING_MSVC)
                # Extra argument for Clang-Tidy when building using MSVC.
                # See https://gitlab.kitware.com/cmake/cmake/-/issues/20512#note_722771.

                set(EXTRA_ARG "--extra-arg=/EHsc")
            endif()

            set(CXX_CLANG_TIDY
                ${CLANG_TIDY_EXE}
                "--checks=${CLANG_TIDY_CHECKS}"
                ${EXTRA_ARG}
                "--header-filter=${HEADER_FILTER_DIR}.*"
                ${CLANG_TIDY_WARNINGS_AS_ERRORS}
            )

            set_target_properties(${TARGET} PROPERTIES
                CXX_CLANG_TIDY "${CXX_CLANG_TIDY}"
            )
        endif()
    endif()

    # Statically link our packages to the target.

    foreach(AVAILABLE_PACKAGE ${AVAILABLE_PACKAGES})
        string(TOUPPER "${AVAILABLE_PACKAGE}" AVAILABLE_PACKAGE_UC)

        if(    "${${AVAILABLE_PACKAGE_UC}_CMAKE_DIR}" STREQUAL ""
           AND "${${AVAILABLE_PACKAGE_UC}_CMAKE_PACKAGE_NAME}" STREQUAL "")
            # There are no CMake configuration files, so manually configure the
            # package using targets of the form <PACKAGE_NAME>::<LIBRARY_NAME>.

            foreach(AVAILABLE_PACKAGE_LIBRARY ${${AVAILABLE_PACKAGE_UC}_LIBRARY} ${${AVAILABLE_PACKAGE_UC}_LIBRARIES})
                if(NOT TARGET ${AVAILABLE_PACKAGE_LIBRARY})
                    add_library(${AVAILABLE_PACKAGE_LIBRARY} STATIC IMPORTED)

                    if(RELEASE_MODE)
                        set(LIBRARY_BUILD_TYPE RELEASE)
                    else()
                        set(LIBRARY_BUILD_TYPE DEBUG)
                    endif()

                    set_property(TARGET ${AVAILABLE_PACKAGE_LIBRARY}
                                 APPEND PROPERTY IMPORTED_CONFIGURATIONS ${LIBRARY_BUILD_TYPE})

                    string(REPLACE "::" ";" PACKAGE_LIBRARY_LIST ${AVAILABLE_PACKAGE_LIBRARY})
                    list(GET PACKAGE_LIBRARY_LIST 1 PACKAGE_LIBRARY_NAME)

                    set_target_properties(${AVAILABLE_PACKAGE_LIBRARY} PROPERTIES
                        IMPORTED_LOCATION_${LIBRARY_BUILD_TYPE} "${PREBUILT_DIR}/${AVAILABLE_PACKAGE}/lib/${PACKAGE_LIBRARY_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
                    )
                endif()
            endforeach()
        else()
            # There are some CMake configuration files, so use them to configure the
            # package.

            find_package(${${AVAILABLE_PACKAGE_UC}_CMAKE_PACKAGE_NAME} REQUIRED
                         PATHS ${${AVAILABLE_PACKAGE_UC}_CMAKE_DIR}
                         NO_SYSTEM_ENVIRONMENT_PATH
                         NO_CMAKE_ENVIRONMENT_PATH
                         NO_CMAKE_SYSTEM_PATH)
        endif()

        target_include_directories(${TARGET} PRIVATE
                                   $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/3rdparty/${AVAILABLE_PACKAGE}>
                                   $<BUILD_INTERFACE:${${AVAILABLE_PACKAGE_UC}_INCLUDE_DIR}>)

        target_link_libraries(${TARGET} PRIVATE
                              ${${AVAILABLE_PACKAGE_UC}_LIBRARY} ${${AVAILABLE_PACKAGE_UC}_LIBRARIES})

        if(${AVAILABLE_PACKAGE_UC}_DEFINITIONS)
            target_compile_definitions(${TARGET} PRIVATE
                                       ${${AVAILABLE_PACKAGE_UC}_DEFINITIONS})
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
                   ${INTERNAL_SOURCE_FILES}
                   ${ARGN})

    add_dependencies(${TARGET} ${CMAKE_PROJECT_NAME})

    target_link_libraries(${TARGET} PRIVATE
                          gtest_main
                          ${CMAKE_PROJECT_NAME})

    configure_target(${TARGET})

    if(NOT LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
        target_compile_definitions(${TARGET} PRIVATE
                                   NLLVMCOV)
    endif()

    list(APPEND TEST_TARGETS "${TARGET}")

    set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)
endfunction()

function(build_documentation DOCUMENTATION_NAME)
    # Build the given documentation as an external project and have it copied to
    # our final documentation directory.

    set(DOCUMENTATION_BUILD ${DOCUMENTATION_NAME}_documentation)

    ExternalProject_Add(${DOCUMENTATION_BUILD}
                        GIT_REPOSITORY https://github.com/opencor/libopencor-${DOCUMENTATION_NAME}-documentation
                        GIT_SHALLOW
                        CMAKE_ARGS -DPYTHON_EXECUTABLE=${PYTHON_EXE}
                                   -DSPHINX_EXECUTABLE=${SPHINX_EXE}
                        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/doc/${DOCUMENTATION_BUILD}-prefix/src/${DOCUMENTATION_BUILD}-build/html
                                                                           ${CMAKE_BINARY_DIR}/doc/html/${DOCUMENTATION_NAME})
endfunction()
