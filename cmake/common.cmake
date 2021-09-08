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

macro(add_target TARGET)
    add_custom_target(${TARGET} ${ARGN})

    set(TARGETS "${TARGETS};${TARGET}" CACHE INTERNAL "Available targets.")
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
        ${ARGN}
    )

    target_link_libraries(${TARGET}
        gtest_main
        ${CMAKE_PROJECT_NAME}
    )

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
