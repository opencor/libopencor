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

function(configure_clang_and_clang_tidy TARGET)
    # Configure Clang.

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
       OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        # The full list of diagnostic flags for Clang can be found at
        # https://clang.llvm.org/docs/DiagnosticsReference.html.

        set(COMPILE_OPTIONS
            -Weverything
        )

        set_target_properties(${TARGET} PROPERTIES
            COMPILE_OPTIONS "${COMPILE_OPTIONS}"
        )
    endif()

    # Configure Clang-Tidy.

    if(CLANG_TIDY_AVAILABLE)
        # The full list of Clang-Tidy checks can be found at
        # https://clang.llvm.org/extra/clang-tidy/checks/list.html.

        set(CLANG_TIDY_CHECKS
            -*
            bugprone-*
            cert-*
            cppcoreguidelines-*
            fuchsia-*
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
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}${EXTRA_ARG};-checks=${CLANG_TIDY_CHECKS};-header-filter=${HEADER_FILTER_DIR}.*"
        )
    endif()
endfunction()
