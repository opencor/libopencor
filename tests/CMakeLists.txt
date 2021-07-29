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

# Prepare our different tests.

set(GIT_TESTS_SOURCE_FILES)
set(GIT_TESTS_HEADER_FILES)

foreach(TEST ${TESTS})
    if(DEFINED ${TEST}_CATEGORY)
        set(TEST_CATEGORY "${${TEST}_CATEGORY}_")
    else()
        set(TEST_CATEGORY)
    endif()

    prepare_test(${TEST_CATEGORY}${TEST}_tests ${${TEST}_SOURCE_FILES})

    list(APPEND GIT_TESTS_SOURCE_FILES ${${TEST}_SOURCE_FILES})
    list(APPEND GIT_TESTS_HEADER_FILES ${${TEST}_HEADER_FILES})
endforeach()

set(GIT_TESTS_SOURCE_FILES ${GIT_TESTS_SOURCE_FILES} PARENT_SCOPE)
set(GIT_TESTS_HEADER_FILES ${GIT_TESTS_HEADER_FILES} PARENT_SCOPE)

# Prepare the tests for our header files.

include(header_files/tests.cmake)

# Add our prepared tests after sorting them.

list(SORT TEST_TARGETS)

foreach(TEST_TARGET ${TEST_TARGETS})
    add_test(NAME ${TEST_TARGET}
             COMMAND ${TEST_TARGET})

    if(MSVC)
        set_tests_properties(${TEST_TARGET} PROPERTIES
            ENVIRONMENT "PATH=$<TARGET_FILE_DIR:gtest_main>\;$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
        )
    endif()
endforeach()

# Get ready for coverage in our tests.

if(LIBOPENCOR_COVERAGE)
    # Clean our old coverage data.

    add_custom_target(clean_coverage_data
                      COMMAND ${FIND_EXE} ${GCOV_ANALYSIS_PATH} -type f -name '*.gcda' -print | xargs /bin/rm -f
                      COMMENT "Cleaning old coverage data...")

    # Clean our old coverage data when building libOpenCOR.

    add_dependencies(${CMAKE_PROJECT_NAME} clean_coverage_data)

    # Add a target to prepare our covereage tests.

    add_custom_target(prepare_coverage
                      DEPENDS clean_coverage_data ${TEST_TARGETS}
                      COMMAND ${CMAKE_CTEST_COMMAND} > /dev/null
                      COMMENT "Preparing for coverage tests...")

    # Add a target for our coverage tests.

    add_custom_target(coverage
                      COMMAND ${GCOVR_EXE} -r ${GCOV_PREFIX_PATH} --gcov-executable=${GCOV_EXE} ${GCOV_ANALYSIS_PATH}
                      DEPENDS prepare_coverage
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Running coverage tests...")
endif()

# Add our Python tests.

add_subdirectory(bindings)

# Add a target for our memory checks.

if(LIBOPENCOR_MEMORY_CHECKS)
    add_custom_target(memory_checks
                      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/memorychecks
                      COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/memorychecks.py ${CMAKE_CURRENT_BINARY_DIR} ${TEST_TARGETS}
                      DEPENDS ${TEST_TARGETS} ${prepare_memcheck}
                      COMMENT "Running memory checks...")
endif()
