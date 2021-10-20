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

function(remove_empty_directory DIR)
    file(GLOB DIR_FILES ${DIR}/*)

    if(NOT DIR_FILES)
        execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${DIR}
                        RESULT_VARIABLE RESULT)

        if(RESULT EQUAL 0)
            get_filename_component(PARENT_DIR ${DIR} DIRECTORY)

            remove_empty_directory(${PARENT_DIR})
        endif()
    endif()
endfunction()

set(MANIFEST_FILE "@PROJECT_BINARY_DIR@/install_manifest.txt")

if(NOT EXISTS ${MANIFEST_FILE})
    message(FATAL_ERROR "${MANIFEST_FILE} could not be found.")
endif()

file(STRINGS ${MANIFEST_FILE} FILES)

foreach(FILE ${FILES})
    if(EXISTS ${FILE} OR IS_SYMLINK ${FILE})
        message(STATUS "Uninstalling: ${FILE}")

        execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${FILE}
                        RESULT_VARIABLE RESULT)

        if(RESULT EQUAL 0)
            get_filename_component(DIR ${FILE} DIRECTORY)

            remove_empty_directory(${DIR})
        else()
            message(FATAL_ERROR "${FILE} could not be uninstalled.")
        endif()
    else()
        message(STATUS "${FILE} could not be found.")
    endif()
endforeach()
