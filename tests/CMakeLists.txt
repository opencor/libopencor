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

# Run our tests.

if(LIBOPENCOR_UNIT_TESTING)
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

    # Run our unit testing.

    add_target(unit_testing
               COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process
               WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
               USES_TERMINAL
               COMMENT "Running unit testing...")
endif()

# Add our bindings tests.

add_subdirectory(bindings)

# Run our coverage tests using gcov.

if(LIBOPENCOR_COVERAGE_GCOV)
    # Clean our old gcov-based coverage data.

    add_custom_target(clean_coverage_gcov_data
                      COMMAND ${FIND_EXE} ${GCOV_ANALYSIS_PATH} -type f -name "*.gcda" -print | xargs /bin/rm -f
                      COMMENT "Cleaning old gcov-based coverage data...")

    # Clean our old gcov-based coverage data when building libOpenCOR.

    add_dependencies(${CMAKE_PROJECT_NAME} clean_coverage_gcov_data)

    # Prepare for gcov-based coverage.

    add_custom_target(prepare_coverage_gcov
                      COMMAND ${CMAKE_CTEST_COMMAND} > /dev/null
                      DEPENDS clean_coverage_gcov_data ${TEST_TARGETS}
                      COMMENT "Preparing for gcov-based coverage...")

    # Run our coverage tests using gcov.

    add_target(coverage_gcov
               COMMAND ${GCOVR_EXE} -r ${GCOV_PREFIX_PATH} --gcov-executable=${GCOV_EXE} ${GCOV_ANALYSIS_PATH}
               DEPENDS prepare_coverage_gcov
               COMMENT "Running coverage tests using gcov...")
endif()

# Run our coverage tests using llvm-cov.

if(LIBOPENCOR_COVERAGE_LLVM_COV)
    # Clean our old llvm-cov-based coverage data.

    set(PROFRAW_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.profraw)

    foreach(TEST_TARGET ${TEST_TARGETS})
        list(APPEND COMMANDS COMMAND LLVM_PROFILE_FILE=${PROFRAW_FILE} ./${TEST_TARGET} > /dev/null 2> /dev/null)
    endforeach()

    add_custom_target(clean_coverage_llvm_cov_data
                      COMMAND ${FIND_EXE} ${CMAKE_CURRENT_BINARY_DIR} -type f -name "${CMAKE_PROJECT_NAME}.prof*" -print | xargs /bin/rm -f
                      COMMENT "Cleaning old llvm-cov-based coverage data...")

    # Clean our old llvm-cov-based coverage data when building libOpenCOR.

    add_dependencies(${CMAKE_PROJECT_NAME} clean_coverage_llvm_cov_data)

    # Prepare for llvm-cov-based coverage.

    set(PROFDATA_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.profdata)

    add_custom_target(prepare_coverage_llvm_cov
                      ${COMMANDS}
                      COMMAND ${LLVM_PROFDATA_EXE} merge -sparse ${PROFRAW_FILE} -o ${PROFDATA_FILE}
                      DEPENDS clean_coverage_llvm_cov_data ${TEST_TARGETS}
                      COMMENT "Preparing for llvm-cov-based coverage...")

    # Run our coverage tests using llvm-cov.

    add_target(coverage_llvm_cov
               COMMAND ${LLVM_COV_EXE} report $<TARGET_FILE:${CMAKE_PROJECT_NAME}> -instr-profile=${PROFDATA_FILE} -use-color
               DEPENDS prepare_coverage_llvm_cov
               COMMENT "Running coverage tests using llvm-cov...")

    # Run our coverage report using llvm-cov.

    add_target(coverage_llvm_cov_report
               COMMAND ${LLVM_COV_EXE} show $<TARGET_FILE:${CMAKE_PROJECT_NAME}> -instr-profile=${PROFDATA_FILE} -format=html -o coverage_llvm_cov_report
               DEPENDS prepare_coverage_llvm_cov
               COMMENT "Running coverage report using llvm-cov...")
endif()

# Run our memory checks.

if(LIBOPENCOR_MEMORY_CHECKS)
    add_target(memory_checks
               COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/memorychecks
               COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/memorychecks.py ${CMAKE_CURRENT_BINARY_DIR} ${TEST_TARGETS}
               DEPENDS ${TEST_TARGETS} ${prepare_memcheck}
               COMMENT "Running memory checks...")
endif()
