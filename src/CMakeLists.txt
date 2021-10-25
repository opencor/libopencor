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

include(GenerateExportHeader)

# Configure the version file.

foreach(VERSION_PART PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH)
    if(${${VERSION_PART}} LESS 10)
        set(${VERSION_PART}_PAD 0)
    endif()
endforeach()

set(LIBOPENCOR_VERSION 0x${PROJECT_VERSION_MAJOR_PAD}${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR_PAD}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH_PAD}${PROJECT_VERSION_PATCH})
set(LIBOPENCOR_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(VERSION_P_HEADER_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/version_p.h.in)
set(VERSION_P_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/version_p.h)

configure_file(${VERSION_P_HEADER_FILE_IN} ${VERSION_P_HEADER_FILE})

# Build libOpenCOR.

set(GIT_API_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/version.h
)

set(API_EXPORT_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/api/${CMAKE_PROJECT_NAME_LC}/export.h)

set(API_HEADER_FILES
    ${GIT_API_HEADER_FILES}
    ${API_EXPORT_HEADER_FILE}
)

set(GIT_API_MODULE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module/${CMAKE_PROJECT_NAME_LC})
set(API_MODULE_FILE ${GIT_API_MODULE_FILE})

set(GIT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
)

set(SOURCE_FILES
    ${GIT_SOURCE_FILES}
)

set(GIT_DIRECT_HEADER_FILES
)

set(GIT_HEADER_FILES
    ${GIT_DIRECT_HEADER_FILES}
    ${VERSION_P_HEADER_FILE_IN}
)

set(HEADER_FILES
    ${GIT_DIRECT_HEADER_FILES}
    ${VERSION_P_HEADER_FILE}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/api
                    api)

add_library(${CMAKE_PROJECT_NAME}
            ${API_HEADER_FILES}
            ${SOURCE_FILES}
            ${HEADER_FILES})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
                      DEBUG_POSTFIX d
                      PREFIX ""
                      VERSION ${PROJECT_VERSION})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/api>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module>
                           $<INSTALL_INTERFACE:include/>
                           $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME_LC}/module>)

set(GIT_API_HEADER_FILES ${GIT_API_HEADER_FILES} PARENT_SCOPE)
set(GIT_API_MODULE_FILE ${GIT_API_MODULE_FILE} PARENT_SCOPE)
set(GIT_SOURCE_FILES ${GIT_SOURCE_FILES} PARENT_SCOPE)
set(GIT_HEADER_FILES ${GIT_HEADER_FILES} PARENT_SCOPE)

# Generate the export header file.

generate_export_header(${CMAKE_PROJECT_NAME}
                       EXPORT_FILE_NAME ${API_EXPORT_HEADER_FILE}
                       BASE_NAME LIBOPENCOR)

# Treat warnings as errors.

if(LIBOPENCOR_WARNINGS_AS_ERRORS)
    treat_warnings_as_errors(${CMAKE_PROJECT_NAME})
endif()

# Analyse the code.

if(LIBOPENCOR_CODE_ANALYSIS)
    configure_clang_and_clang_tidy(${CMAKE_PROJECT_NAME})
endif()

# Get ready for code coverage using gcov.

if(LIBOPENCOR_CODE_COVERAGE_GCOV)
    # Update our compilation and linking flags.

    add_target_property(${CMAKE_PROJECT_NAME} COMPILE_FLAGS ${CODE_COVERAGE_GCOV_COMPILER_FLAGS})
    add_target_property(${CMAKE_PROJECT_NAME} LINK_FLAGS ${CODE_COVERAGE_GCOV_LINKER_FLAGS})

    # Share some paths with our tests.

    set(GCOV_ANALYSIS_PATH ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${CMAKE_PROJECT_NAME}.dir PARENT_SCOPE)
    set(GCOV_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
endif()

# Get ready for code coverage using llvm-cov.

if(LIBOPENCOR_CODE_COVERAGE_LLVM_COV)
    add_target_property(${CMAKE_PROJECT_NAME} COMPILE_FLAGS ${CODE_COVERAGE_LLVM_COV_COMPILER_FLAGS})
    add_target_property(${CMAKE_PROJECT_NAME} LINK_FLAGS ${CODE_COVERAGE_LLVM_COV_LINKER_FLAGS})
endif()

# Build our different bindings.

add_subdirectory(bindings)

# Install libOpenCOR itself.

install(TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES ${API_HEADER_FILES}
        DESTINATION include/${CMAKE_PROJECT_NAME_LC})

install(FILES ${API_MODULE_FILE}
        DESTINATION include/${CMAKE_PROJECT_NAME_LC}/module)

# Install libOpenCOR's CMake files.

include(CMakePackageConfigHelpers)

if(WIN32)
    set(CMAKE_PACKAGE_DIR cmake/${PROJECT_NAME})
else()
    set(CMAKE_PACKAGE_DIR lib/cmake/${PROJECT_NAME})
endif()

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
        FILE ${CMAKE_PROJECT_NAME}Targets.cmake
        DESTINATION ${CMAKE_PACKAGE_DIR})

set(CONFIG_CMAKE_IN_FILE ${PROJECT_SOURCE_DIR}/cmake/config.cmake.in)
set(CONFIG_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake)

configure_package_config_file(${CONFIG_CMAKE_IN_FILE} ${CONFIG_CMAKE_FILE}
                              INSTALL_DESTINATION ${CMAKE_PACKAGE_DIR})

set(CONFIG_VERSION_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake)

write_basic_package_version_file(${CONFIG_VERSION_CMAKE_FILE}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CONFIG_CMAKE_FILE}
              ${CONFIG_VERSION_CMAKE_FILE}
        DESTINATION ${CMAKE_PACKAGE_DIR})

# Uninstall libOpenCOR.

set(UNINSTALL_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

configure_file(${PROJECT_SOURCE_DIR}/cmake/uninstall.cmake.in ${UNINSTALL_CMAKE_FILE} @ONLY)

add_target(uninstall
           COMMAND ${CMAKE_COMMAND} -P ${UNINSTALL_CMAKE_FILE}
           COMMENT "Uninstall the project...")
