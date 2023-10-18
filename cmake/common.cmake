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
    # Ignore some MSVC warnings.

    if(BUILDING_USING_MSVC)
        target_compile_options(${TARGET} PRIVATE
                               /wd4251)
    endif()

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
                -Wno-disabled-macro-expansion
                -Wno-exit-time-destructors
                -Wno-global-constructors
                -Wno-padded
                -Wno-switch-enum
                -Wno-unsafe-buffer-usage
                -Wno-weak-vtables
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
                set(DISABLED_BUGPRONE_CHECKS
                    -bugprone-reserved-identifier
                )
                set(DISABLED_CERT_CHECKS
                    -cert-err58-cpp
                )
                set(DISABLED_CPPCOREGUIDELINES_CHECKS
                    -cppcoreguidelines-avoid-non-const-global-variables
                    -cppcoreguidelines-non-private-member-variables-in-classes
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
                -bugprone-easily-swappable-parameters
                ${DISABLED_BUGPRONE_CHECKS}
                cert-*
                ${DISABLED_CERT_CHECKS}
                cppcoreguidelines-*
                -cppcoreguidelines-owning-memory
                -cppcoreguidelines-pro-type-reinterpret-cast
                -cppcoreguidelines-pro-type-static-cast-downcast
                ${DISABLED_CPPCOREGUIDELINES_CHECKS}
                fuchsia-*
                -fuchsia-default-arguments-calls
                ${DISABLED_FUCHSIA_CHECKS}
                google-*
                hicpp-*
                llvm-*
                -llvm-header-guard
                -llvm-include-order
                misc-*
                -misc-include-cleaner
                ${MISC_CHECKS}
                modernize-*
                -modernize-pass-by-value
                -modernize-use-trailing-return-type
                performance-*
                readability-*
                -readability-function-cognitive-complexity
                -readability-identifier-length
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
                                  CXX_CLANG_TIDY "${CXX_CLANG_TIDY}")
        endif()
    endif()

    # Let the target know which operating system we are using.

    if(WIN32)
        target_compile_definitions(${TARGET} PRIVATE
                                   BUILDING_ON_WINDOWS)
    elseif(APPLE)
        target_compile_definitions(${TARGET} PRIVATE
                                   BUILDING_ON_MACOS)
    else()
        target_compile_definitions(${TARGET} PRIVATE
                                   BUILDING_ON_LINUX)
    endif()

    # Let the target know which compiler we are using.

    if(BUILDING_USING_MSVC)
        target_compile_definitions(${TARGET} PRIVATE
                                   BUILDING_USING_MSVC)
    elseif(BUILDING_USING_GNU)
        target_compile_definitions(${TARGET} PRIVATE
                                   BUILDING_USING_GNU)
    elseif(BUILDING_USING_CLANG)
        target_compile_definitions(${TARGET} PRIVATE
                                   BUILDING_USING_CLANG)
    endif()

    # Let the target know that we are building with coverage enabled.

    if(LIBOPENCOR_CODE_COVERAGE)
        target_compile_definitions(${TARGET} PRIVATE
                                   COVERAGE_ENABLED)
    endif()

    # Let the target know that we are fine with using std::codecvt_utf8() even though it has been deprecated in C++17
    # (but no replacement has been provided yet).
    # Note: this is so that we can convert a std::wstring to a std::string on Windows (see wideStringToString()), hence
    #       the WIN32 check.

    if(WIN32)
        target_compile_definitions(${TARGET} PRIVATE
                                   _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    endif()

    # Statically link our packages to the target.

    foreach(AVAILABLE_PACKAGE ${AVAILABLE_PACKAGES})
        string(TOUPPER "${AVAILABLE_PACKAGE}" AVAILABLE_PACKAGE_UC)

        if(    "${${AVAILABLE_PACKAGE_UC}_CMAKE_DIR}" STREQUAL ""
           AND "${${AVAILABLE_PACKAGE_UC}_CMAKE_PACKAGE_NAME}" STREQUAL "")
            # There are no CMake configuration files, so manually configure the package using targets of the form
            # <PACKAGE_NAME>::<LIBRARY_NAME>.

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
                                          IMPORTED_LOCATION_${LIBRARY_BUILD_TYPE} "${PREBUILT_DIR}/${AVAILABLE_PACKAGE}/lib/${PACKAGE_LIBRARY_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
                endif()
            endforeach()
        else()
            # There are some CMake configuration files, so use them to configure the package.

            if(EMSCRIPTEN)
                if("${${AVAILABLE_PACKAGE_UC}_CMAKE_DIRS}" STREQUAL "")
                    set(${${AVAILABLE_PACKAGE_UC}_CMAKE_PACKAGE_NAME}_DIR ${${AVAILABLE_PACKAGE_UC}_CMAKE_DIR})
                else()
                    list(LENGTH ${AVAILABLE_PACKAGE_UC}_CMAKE_PACKAGE_NAMES SUB_PACKAGES_COUNT)

                    foreach(INDEX RANGE 1 ${SUB_PACKAGES_COUNT})
                        math(EXPR REAL_INDEX "${INDEX}-1")

                        list(GET ${AVAILABLE_PACKAGE_UC}_CMAKE_PACKAGE_NAMES ${REAL_INDEX} SUB_PACKAGE_NAME)
                        list(GET ${AVAILABLE_PACKAGE_UC}_CMAKE_DIRS ${REAL_INDEX} SUB_PACKAGE_DIR)

                        set(${SUB_PACKAGE_NAME}_DIR ${SUB_PACKAGE_DIR})
                    endforeach()
                endif()
            endif()

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

    # For some reasons, libNuML needs to be linked last when building using GNU.

    if(BUILDING_USING_GNU)
        target_link_libraries(${TARGET} PRIVATE
                              ${LIBNUML_LIBRARY_FILE})
    endif()

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

    add_dependencies(${TARGET} ${CMAKE_PROJECT_NAME})

    target_link_libraries(${TARGET} PRIVATE
                          gtest_main
                          ${CMAKE_PROJECT_NAME})

    configure_target(${TARGET})

    list(APPEND TEST_TARGETS "${TARGET}")

    set(TEST_TARGETS ${TEST_TARGETS} PARENT_SCOPE)
endfunction()

function(build_documentation DOCUMENTATION_NAME REPOSITORY_NAME DESTINATION_DIR)
    # Build the given documentation as an external project and have it copied to our final documentation directory.

    set(DOCUMENTATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/${DOCUMENTATION_NAME})

    configure_file(${CMAKE_SOURCE_DIR}/cmake/builddocumentation.cmake.in ${DOCUMENTATION_BUILD_DIR}/CMakeLists.txt)

    add_custom_target(${DOCUMENTATION_NAME}
                      COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S . -B build
                      COMMAND ${CMAKE_COMMAND} --build build
                      WORKING_DIRECTORY ${DOCUMENTATION_BUILD_DIR})
endfunction()
