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

include(api/file/tests.cmake)
include(api/logger/tests.cmake)
include(api/sed/tests.cmake)
include(api/solver/tests.cmake)
include(api/version/tests.cmake)

include(misc/compiler/tests.cmake)

include(support/cellml/tests.cmake)

# Prepare our different tests.

set(GIT_TESTS_SOURCE_FILES)
set(GIT_TESTS_HEADER_FILES)

set(RESOURCE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/res)

set(UTILS_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp)
set(UTILS_HEADER_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/utils.h.in)
set(UTILS_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/utils.h)

file(READ res/cellml_2.cellml SOME_CELLML_CONTENTS)
file(READ res/error.cellml SOME_ERROR_CELLML_CONTENTS)
file(READ res/warning.cellml SOME_WARNING_CELLML_CONTENTS)
file(READ res/cellml_2.sedml SOME_SEDML_CONTENTS)

string(REPLACE "\"" "\\\"" SOME_CELLML_CONTENTS "${SOME_CELLML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SOME_CELLML_CONTENTS_C "${SOME_CELLML_CONTENTS}")

string(REPLACE "\"" "\\\"" SOME_ERROR_CELLML_CONTENTS "${SOME_ERROR_CELLML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SOME_ERROR_CELLML_CONTENTS_C "${SOME_ERROR_CELLML_CONTENTS}")

string(REPLACE "\"" "\\\"" SOME_WARNING_CELLML_CONTENTS "${SOME_WARNING_CELLML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SOME_WARNING_CELLML_CONTENTS_C "${SOME_WARNING_CELLML_CONTENTS}")

string(REPLACE "\"" "\\\"" SOME_SEDML_CONTENTS "${SOME_SEDML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SOME_SEDML_CONTENTS_C "${SOME_SEDML_CONTENTS}")

file(READ res/api/sed/algebraic.cellml SOME_ALGEBRAIC_CONTENTS)
file(READ res/api/sed/dae.cellml SOME_DAE_CONTENTS)
file(READ res/api/sed/nla.cellml SOME_NLA_CONTENTS)
file(READ res/api/sed/overconstrained.cellml SOME_OVERCONSTRAINED_CONTENTS)
file(READ res/api/sed/underconstrained.cellml SOME_UNDERCONSTRAINED_CONTENTS)
file(READ res/api/sed/unsuitably_constrained.cellml SOME_UNSUITABLY_CONSTRAINED_CONTENTS)

string(REPLACE "\"" "\\\"" SOME_ALGEBRAIC_CONTENTS "${SOME_ALGEBRAIC_CONTENTS}")
string(REPLACE "\"" "\\\"" SOME_DAE_CONTENTS "${SOME_DAE_CONTENTS}")
string(REPLACE "\"" "\\\"" SOME_NLA_CONTENTS "${SOME_NLA_CONTENTS}")
string(REPLACE "\"" "\\\"" SOME_OVERCONSTRAINED_CONTENTS "${SOME_OVERCONSTRAINED_CONTENTS}")
string(REPLACE "\"" "\\\"" SOME_UNDERCONSTRAINED_CONTENTS "${SOME_UNDERCONSTRAINED_CONTENTS}")
string(REPLACE "\"" "\\\"" SOME_UNSUITABLY_CONSTRAINED_CONTENTS "${SOME_UNSUITABLY_CONSTRAINED_CONTENTS}")

configure_file(${UTILS_HEADER_FILE_IN} ${UTILS_HEADER_FILE})

include_directories(${CMAKE_SOURCE_DIR}/src/misc
                    ${CMAKE_SOURCE_DIR}/src/solver
                    ${CMAKE_SOURCE_DIR}/src/support/cellml
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

# Run our code coverage tests.

if(LIBOPENCOR_CODE_COVERAGE)
    # Clean our old code coverage data.

    add_custom_target(clean_code_coverage_data
                      COMMAND ${FIND_EXE} ${CMAKE_CURRENT_BINARY_DIR} -type f -name \"${CMAKE_PROJECT_NAME}*.prof*\" -print | xargs /bin/rm -f
                      COMMENT "Cleaning old coverage data...")

    # Clean our old code coverage data when building libOpenCOR.

    add_dependencies(${CMAKE_PROJECT_NAME} clean_code_coverage_data)

    # Prepare for code coverage.

    foreach(TEST_TARGET ${TEST_TARGETS})
        list(APPEND COMMANDS COMMAND LLVM_PROFILE_FILE=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_${TEST_TARGET}.profraw ./${TEST_TARGET} > /dev/null 2> /dev/null)
    endforeach()

    set(PROFDATA_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.profdata)

    add_custom_target(prepare_code_coverage
                      ${COMMANDS}
                      COMMAND ${LLVM_PROFDATA_EXE} merge -sparse ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}*.profraw -o ${PROFDATA_FILE}
                      DEPENDS clean_code_coverage_data ${TEST_TARGETS}
                      COMMENT "Preparing for code coverage...")

    # Run our code coverage tests.

    set(LLVM_COV_OPTIONS -ignore-filename-regex="prebuilt.*" -instr-profile=${PROFDATA_FILE})

    add_target(code_coverage
               COMMAND ${LLVM_COV_EXE} report $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${LLVM_COV_OPTIONS} -use-color
               DEPENDS prepare_code_coverage
               COMMENT "Running code coverage tests...")

    # Run our code coverage report.

    set(COVERAGE_REPORT_DIR code_coverage_report)

    add_target(code_coverage_report
               COMMAND ${LLVM_COV_EXE} show $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${LLVM_COV_OPTIONS} -format=html -show-branches=count -show-line-counts-or-regions -o ${COVERAGE_REPORT_DIR}
               COMMAND echo "The coverage report can be found at ${CMAKE_CURRENT_BINARY_DIR}/${COVERAGE_REPORT_DIR}/index.html."
               DEPENDS prepare_code_coverage
               COMMENT "Running code coverage report...")
endif()

# Run our memory checks.

if(LIBOPENCOR_MEMORY_CHECKS)
    add_target(memory_checks
               COMMAND ${PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/memory_checks.py ${CMAKE_CURRENT_BINARY_DIR} ${TEST_TARGETS}
               DEPENDS ${TEST_TARGETS}
               COMMENT "Running memory checks...")
endif()
