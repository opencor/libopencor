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

# Configure GoogleTest.

if(APPLE)
    set_target_properties(gtest gtest_main
        PROPERTIES MACOSX_RPATH ON
    )
endif()

# Include our different tests.

include(version/tests.cmake)

# Build our different tests.

foreach(TEST ${TESTS})
    # Build our test.

    if(DEFINED ${TEST}_CATEGORY)
        set(TEST_CATEGORY "${${TEST}_CATEGORY}_")
    else()
        set(TEST_CATEGORY)
    endif()

    set(TEST_TARGET ${TEST_CATEGORY}${TEST}_tests)

    add_executable(${TEST_TARGET}
        ${${TEST}_SOURCE_FILES}
    )

    target_link_libraries(${TEST_TARGET}
        gtest_main
        ${CMAKE_PROJECT_NAME}
    )

    add_test(NAME ${TEST_TARGET}
             COMMAND ${TEST_TARGET})

    if(MSVC)
        set_tests_properties(${TEST_TARGET} PROPERTIES
            ENVIRONMENT "PATH=$<TARGET_FILE_DIR:gtest_main>\;$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
        )
    endif()

    # Treat warnings as errors.

    if(LIBOPENCOR_WARNINGS_AS_ERRORS)
        configure_warnings_as_errors(${TEST_TARGET})
    endif()

    # Use Clang-Tidy.

    if(LIBOPENCOR_CLANG_TIDY)
        configure_clang_and_clang_tidy(${TEST_TARGET})
    endif()
endforeach()
