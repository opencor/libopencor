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

# Generate our base64 encoder.

set(BASE64ENCODER ${CMAKE_BINARY_DIR}/base64encoder)

try_compile(BASE64ENCODER_EXE ${CMAKE_BINARY_DIR}
            SOURCES ${CMAKE_SOURCE_DIR}/cmake/base64encoder.cpp
                    ${CMAKE_SOURCE_DIR}/extern/modp_b64/modp_b64.cc
            COPY_FILE ${BASE64ENCODER})

if(NOT BASE64ENCODER_EXE)
    message(FATAL_ERROR "base64encoder could not be built...")
endif()

# Copy of our version file over.

file(COPY ${CMAKE_SOURCE_DIR}/VERSION.txt
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Include our different tests.

include(api/file/tests.cmake)
include(api/logger/tests.cmake)
include(api/sed/tests.cmake)
include(api/solver/tests.cmake)
include(api/version/tests.cmake)

include(misc/compiler/tests.cmake)

include(support/cellml/tests.cmake)
include(support/combine/tests.cmake)
include(support/sedml/tests.cmake)

# Prepare our different tests.

set(GIT_TESTS_SOURCE_FILES)
set(GIT_TESTS_HEADER_FILES)

set(RESOURCE_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/res)

set(UTILS_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp)
set(UTILS_HEADER_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/utils.h.in)
set(UTILS_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/utils.h)

file(READ res/cellml_2.cellml CELLML_CONTENTS)
file(READ res/error.cellml ERROR_CELLML_CONTENTS)
file(READ res/warning.cellml WARNING_CELLML_CONTENTS)
file(READ res/cellml_2.sedml SEDML_CONTENTS)
file(READ res/api/sed/absolute_cellml_file.sedml SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS)
file(READ res/api/sed/remote_cellml_file.sedml SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS)
file(READ res/api/sed/simulations.sedml SEDML_WITH_SIMULATIONS_CONTENTS)

string(REPLACE "\"" "\\\"" CELLML_CONTENTS "${CELLML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" CELLML_CONTENTS_C "${CELLML_CONTENTS}")

string(REPLACE "\"" "\\\"" ERROR_CELLML_CONTENTS "${ERROR_CELLML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" ERROR_CELLML_CONTENTS_C "${ERROR_CELLML_CONTENTS}")

string(REPLACE "\"" "\\\"" WARNING_CELLML_CONTENTS "${WARNING_CELLML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" WARNING_CELLML_CONTENTS_C "${WARNING_CELLML_CONTENTS}")

string(REPLACE "\"" "\\\"" SEDML_CONTENTS "${SEDML_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SEDML_CONTENTS_C "${SEDML_CONTENTS}")

string(REPLACE "\"" "\\\"" SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS "${SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS_C "${SEDML_WITH_ABSOLUTE_CELLML_FILE_CONTENTS}")

string(REPLACE "\"" "\\\"" SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS "${SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS_C "${SEDML_WITH_REMOTE_CELLML_FILE_CONTENTS}")

string(REPLACE "\"" "\\\"" SEDML_WITH_SIMULATIONS_CONTENTS "${SEDML_WITH_SIMULATIONS_CONTENTS}")
string(REGEX REPLACE "\n" "\" \\n\"" SEDML_WITH_SIMULATIONS_CONTENTS_C "${SEDML_WITH_SIMULATIONS_CONTENTS}")

execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/cellml_2.omex"
                OUTPUT_VARIABLE BASE64_COMBINE_ARCHIVE_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/no_manifest_file.omex"
                OUTPUT_VARIABLE BASE64_COMBINE_ARCHIVE_WITH_NO_MANIFEST_FILE_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/no_master_file.omex"
                OUTPUT_VARIABLE BASE64_COMBINE_ARCHIVE_WITH_NO_MASTER_FILE_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/cellml_file_as_master_file.omex"
                OUTPUT_VARIABLE BASE64_COMBINE_ARCHIVE_WITH_CELLML_FILE_AS_MASTER_FILE_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/unknown_direct_cellml_file.omex"
                OUTPUT_VARIABLE BASE64_COMBINE_ARCHIVE_WITH_UNKNOWN_DIRECT_CELLML_FILE_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/unknown_indirect_cellml_file.omex"
                OUTPUT_VARIABLE BASE64_COMBINE_ARCHIVE_WITH_UNKNOWN_INDIRECT_CELLML_FILE_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/unknown_sedml_file.omex"
                OUTPUT_VARIABLE BASE64_COMBINE_ARCHIVE_WITH_UNKNOWN_SEDML_FILE_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/file/dataset_135.omex"
                OUTPUT_VARIABLE BASE64_DATASET_135_OMEX_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/file/dataset_135.json"
                OUTPUT_VARIABLE BASE64_DATASET_135_JSON_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/file/dataset_157.omex"
                OUTPUT_VARIABLE BASE64_DATASET_157_OMEX_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/file/dataset_157.json"
                OUTPUT_VARIABLE BASE64_DATASET_157_JSON_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

file(READ res/api/solver/ode.cellml SOLVER_ODE_CONTENTS)
file(READ res/api/solver/nla1.cellml SOLVER_NLA1_CONTENTS)
file(READ res/api/solver/nla2.cellml SOLVER_NLA2_CONTENTS)

string(REPLACE "\"" "\\\"" SOLVER_ODE_CONTENTS "${SOLVER_ODE_CONTENTS}")
string(REPLACE "\"" "\\\"" SOLVER_NLA1_CONTENTS "${SOLVER_NLA1_CONTENTS}")
string(REPLACE "\"" "\\\"" SOLVER_NLA2_CONTENTS "${SOLVER_NLA2_CONTENTS}")

file(READ res/api/sed/algebraic.cellml ALGEBRAIC_CONTENTS)
file(READ res/api/sed/dae.cellml DAE_CONTENTS)
file(READ res/api/sed/math.cellml MATH_CONTENTS)
file(READ res/api/sed/nla.cellml NLA_CONTENTS)
file(READ res/api/sed/overconstrained.cellml OVERCONSTRAINED_CONTENTS)
file(READ res/api/sed/underconstrained.cellml UNDERCONSTRAINED_CONTENTS)
file(READ res/api/sed/unsuitably_constrained.cellml UNSUITABLY_CONSTRAINED_CONTENTS)

string(REPLACE "\"" "\\\"" ALGEBRAIC_CONTENTS "${ALGEBRAIC_CONTENTS}")
string(REPLACE "\"" "\\\"" DAE_CONTENTS "${DAE_CONTENTS}")
string(REPLACE "\"" "\\\"" MATH_CONTENTS "${MATH_CONTENTS}")
string(REPLACE "\"" "\\\"" NLA_CONTENTS "${NLA_CONTENTS}")
string(REPLACE "\"" "\\\"" OVERCONSTRAINED_CONTENTS "${OVERCONSTRAINED_CONTENTS}")
string(REPLACE "\"" "\\\"" UNDERCONSTRAINED_CONTENTS "${UNDERCONSTRAINED_CONTENTS}")
string(REPLACE "\"" "\\\"" UNSUITABLY_CONSTRAINED_CONTENTS "${UNSUITABLY_CONSTRAINED_CONTENTS}")

execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/sed_changes.omex"
                OUTPUT_VARIABLE BASE64_SED_CHANGES_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/invalid_sed_changes.omex"
                OUTPUT_VARIABLE BASE64_INVALID_SED_CHANGES_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/sed/unsupported_sed_changes.omex"
                OUTPUT_VARIABLE BASE64_UNSUPPORTED_SED_CHANGES_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/solver/algebraic_sed_changes.omex"
                OUTPUT_VARIABLE BASE64_ALGEBRAIC_SED_CHANGES_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${BASE64ENCODER} "${CMAKE_CURRENT_SOURCE_DIR}/res/api/solver/ode_sed_changes.omex"
                OUTPUT_VARIABLE BASE64_ODE_SED_CHANGES_CONTENTS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file(${UTILS_HEADER_FILE_IN} ${UTILS_HEADER_FILE})

include_directories(${CMAKE_SOURCE_DIR}/src/misc
                    ${CMAKE_SOURCE_DIR}/src/solver
                    ${CMAKE_SOURCE_DIR}/src/support/cellml
                    ${CMAKE_SOURCE_DIR}/src/support/combine
                    ${CMAKE_SOURCE_DIR}/src/support/sedml
                    ${CMAKE_BINARY_DIR})

foreach(TEST ${TESTS})
    if(LIBOPENCOR_UNIT_TESTING)
        if(NOT "${${TEST}_CATEGORY}" STREQUAL "")
            set(TEST_CATEGORY "${${TEST}_CATEGORY}_")
        endif()

        prepare_test(${TEST_CATEGORY}${TEST}_tests
                     ${${TEST}_SOURCE_FILES}
                     ${UTILS_SOURCE_FILE})
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
               COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure
               DEPENDS ${TEST_TARGETS}
               WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
               USES_TERMINAL
               COMMENT "Running unit testing...")
endif()

# Let libOpenCOR know whether unit testing is enabled.

if(LIBOPENCOR_UNIT_TESTING)
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
