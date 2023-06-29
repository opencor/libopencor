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

# Include our different tests.

include(compiler/tests.cmake)
include(file/tests.cmake)
include(runtime/tests.cmake)
include(solver/tests.cmake)
include(version/tests.cmake)

# Prepare our different tests.

set(GIT_TESTS_SOURCE_FILES)
set(GIT_TESTS_HEADER_FILES)

set(RESOURCE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/res)

set(UTILS_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp)
set(UTILS_HEADER_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/utils.h.in)
set(UTILS_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/utils.h)

file(READ res/cellml_2.cellml SOME_CELLML_CONTENTS)
file(READ res/cellml_2.sedml SOME_SEDML_CONTENTS)

string(REPLACE "\"" "\\\"" SOME_CELLML_CONTENTS "${SOME_CELLML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SOME_CELLML_CONTENTS_C "${SOME_CELLML_CONTENTS}")

string(REPLACE "\"" "\\\"" SOME_SEDML_CONTENTS "${SOME_SEDML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SOME_SEDML_CONTENTS_C "${SOME_SEDML_CONTENTS}")

configure_file(${UTILS_HEADER_FILE_IN} ${UTILS_HEADER_FILE})

include_directories(${CMAKE_SOURCE_DIR}/src/misc
                    ${CMAKE_BINARY_DIR})

foreach(TEST ${TESTS})
    if(LIBOPENCOR_UNIT_TESTING)
        if(NOT "${${TEST}_CATEGORY}" STREQUAL "")
            set(TEST_CATEGORY "${${TEST}_CATEGORY}_")
        endif()

        prepare_test(${TEST_CATEGORY}${TEST}_tests ${${TEST}_SOURCE_FILES} ${UTILS_SOURCE_FILE})
    endif()

    list(APPEND GIT_TESTS_SOURCE_FILES ${${TEST}_SOURCE_FILES})
    list(APPEND GIT_TESTS_HEADER_FILES ${${TEST}_HEADER_FILES})
endforeach()

list(APPEND GIT_TESTS_SOURCE_FILES ${UTILS_SOURCE_FILE})
list(APPEND GIT_TESTS_HEADER_FILES ${UTILS_HEADER_FILE_IN})

set(GIT_TESTS_SOURCE_FILES ${GIT_TESTS_SOURCE_FILES} PARENT_SCOPE)
set(GIT_TESTS_HEADER_FILES ${GIT_TESTS_HEADER_FILES} PARENT_SCOPE)

# Prepare the tests for our header files.

include(headerfiles/tests.cmake)

# Add our prepared tests, after sorting them, as well as our unit testing target.

if(LIBOPENCOR_UNIT_TESTING)
    list(SORT TEST_TARGETS)

    foreach(TEST_TARGET ${TEST_TARGETS})
        add_test(NAME ${TEST_TARGET}
                 COMMAND ${TEST_TARGET})

        if(BUILDING_USING_MSVC)
            set_tests_properties(${TEST_TARGET} PROPERTIES
                                 ENVIRONMENT "PATH=$<TARGET_FILE_DIR:gtest_main>\;$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>")
        endif()
    endforeach()

    add_target(unit_testing
               COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process
               DEPENDS ${TEST_TARGETS}
               WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
               USES_TERMINAL
               COMMENT "Running unit testing...")
endif()

# Let libOpenCOR know if unit testing is enabled.

if(LIBOPENCOR_UNIT_TESTING OR LIBOPENCOR_PYTHON_UNIT_TESTING)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
                               UNIT_TESTING_ENABLED)
endif()

# Add our bindings tests.

add_subdirectory(bindings)

# Let libOpenCOR know if coverage is enabled.

if(LIBOPENCOR_CODE_COVERAGE_GCOV OR LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
                               COVERAGE_ENABLED)
endif()

# Run our code coverage tests using gcov.

if(LIBOPENCOR_CODE_COVERAGE_GCOV)
    # Clean our old gcov-based code coverage data.

    add_custom_target(clean_code_coverage_gcov_data
                      COMMAND ${FIND_EXE} ${GCOV_ANALYSIS_PATH} -type f -name \"*.gcda\" -print | xargs /bin/rm -f
                      COMMENT "Cleaning old gcov-based code coverage data...")

    # Clean our old gcov-based code coverage data when building libOpenCOR.

    add_dependencies(${CMAKE_PROJECT_NAME} clean_code_coverage_gcov_data)

    # Prepare for gcov-based code coverage.

    add_custom_target(prepare_code_coverage_gcov
                      COMMAND ${CMAKE_CTEST_COMMAND} > /dev/null
                      DEPENDS clean_code_coverage_gcov_data ${TEST_TARGETS}
                      COMMENT "Preparing for gcov-based code coverage...")

    # Run our code coverage tests using gcov.

    add_target(code_coverage_gcov
               COMMAND ${GCOVR_EXE} -r ${GCOV_PREFIX_PATH} --gcov-executable=${GCOV_EXE} ${GCOV_ANALYSIS_PATH}
               DEPENDS prepare_code_coverage_gcov
               COMMENT "Running code coverage tests using gcov...")
endif()

# Run our code coverage tests using llvm-cov.

if(LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
    # Clean our old llvm-cov-based code coverage data.

    add_custom_target(clean_code_coverage_llvm_cov_data
                      COMMAND ${FIND_EXE} ${CMAKE_CURRENT_BINARY_DIR} -type f -name \"${CMAKE_PROJECT_NAME}*.prof*\" -print | xargs /bin/rm -f
                      COMMENT "Cleaning old llvm-cov-based code coverage data...")

    # Clean our old llvm-cov-based code coverage data when building libOpenCOR.

    add_dependencies(${CMAKE_PROJECT_NAME} clean_code_coverage_llvm_cov_data)

    # Prepare for llvm-cov-based code coverage.

    foreach(TEST_TARGET ${TEST_TARGETS})
        list(APPEND COMMANDS COMMAND LLVM_PROFILE_FILE=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_${TEST_TARGET}.profraw ./${TEST_TARGET} > /dev/null 2> /dev/null)
    endforeach()

    set(PROFDATA_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.profdata)

    add_custom_target(prepare_code_coverage_llvm_cov
                      ${COMMANDS}
                      COMMAND ${LLVM_PROFDATA_EXE} merge -sparse ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}*.profraw -o ${PROFDATA_FILE}
                      DEPENDS clean_code_coverage_llvm_cov_data ${TEST_TARGETS}
                      COMMENT "Preparing for llvm-cov-based code coverage...")

    # Run our code coverage tests using llvm-cov.

    set(LLVM_COV_OPTIONS -ignore-filename-regex="prebuilt.*" -instr-profile=${PROFDATA_FILE})

    add_target(code_coverage_llvm_cov
               COMMAND ${LLVM_COV_EXE} report $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${LLVM_COV_OPTIONS} -use-color
               DEPENDS prepare_code_coverage_llvm_cov
               COMMENT "Running code coverage tests using llvm-cov...")

    # Run our code coverage report using llvm-cov.

    set(COVERAGE_REPORT_DIR code_coverage_llvm_cov_report)

    add_target(code_coverage_llvm_cov_report
               COMMAND ${LLVM_COV_EXE} show $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${LLVM_COV_OPTIONS} -format=html -show-branches=count -show-line-counts-or-regions -o ${COVERAGE_REPORT_DIR}
               COMMAND echo "The coverage report can be found at ${CMAKE_CURRENT_BINARY_DIR}/${COVERAGE_REPORT_DIR}/index.html."
               DEPENDS prepare_code_coverage_llvm_cov
               COMMENT "Running code coverage report using llvm-cov...")
endif()

# Run our memory checks.

if(LIBOPENCOR_MEMORY_CHECKS)
    add_target(memory_checks
               COMMAND ${PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/memory_checks.py ${CMAKE_CURRENT_BINARY_DIR} ${TEST_TARGETS}
               DEPENDS ${TEST_TARGETS}
               COMMENT "Running memory checks...")
endif()
