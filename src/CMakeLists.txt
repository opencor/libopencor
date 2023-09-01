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

# Enable compiler caching.

if(LIBOPENCOR_COMPILER_CACHING)
    if(BUILDCACHE_EXE)
        set(CMAKE_C_COMPILER_LAUNCHER ${BUILDCACHE_EXE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${BUILDCACHE_EXE})
    elseif(CLCACHE_EXE)
        set(CLCACHEWRAPPER ${CMAKE_CURRENT_BINARY_DIR}/clcachewrapper)

        execute_process(COMMAND ${CMAKE_C_COMPILER} /O2 /Fe${CLCACHEWRAPPER} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/clcachewrapper.c
                        RESULT_VARIABLE RESULT
                        OUTPUT_QUIET ERROR_QUIET)

        if(RESULT EQUAL 0)
            set(CMAKE_C_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
            set(CMAKE_CXX_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
        endif()
    else()
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
    endif()
endif()

# Replace some compiler flags when building using MSVC.

if(BUILDING_USING_MSVC)
    # Replace the /Zi option (i.e. enable debugging information), if present, with the /Z7 option (i.e. enable old-style
    # debugging information) otherwise caching (be it with buildcache or clcache) won't work.

    if(BUILDCACHE_EXE OR CLCACHE_EXE)
        replace_compiler_flag("/Zi" "/Z7")
    endif()
endif()

# Build our different third-party libraries.

add_subdirectory(3rdparty)

# Configure the version file.

foreach(VERSION_PART PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH)
    if(${${VERSION_PART}} LESS 10)
        set(${VERSION_PART}_PAD 0)
    endif()
endforeach()

set(LIBOPENCOR_VERSION 0x${PROJECT_VERSION_MAJOR_PAD}${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR_PAD}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH_PAD}${PROJECT_VERSION_PATCH})
set(LIBOPENCOR_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(VERSION_HEADER_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in)
set(VERSION_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.h)

configure_file(${VERSION_HEADER_FILE_IN} ${VERSION_HEADER_FILE})

# Build libOpenCOR.

if(NOT WIN32 AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(GIT_API_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/issue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverforwardeuler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/version.h
)

set(API_EXPORT_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/api/${CMAKE_PROJECT_NAME_LC}/export.h)

set(API_HEADER_FILES
    ${GIT_API_HEADER_FILES}
    ${API_EXPORT_HEADER_FILE}
)

set(GIT_API_MODULE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module/${CMAKE_PROJECT_NAME_LC})
set(API_MODULE_FILE ${GIT_API_MODULE_FILE})

set(INTERNAL_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverforwardeuler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile.cpp
)

if(NOT EMSCRIPTEN)
    list(APPEND INTERNAL_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler.cpp
    )
endif()

set(GIT_API_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/issue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
)

set(GIT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/file_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/issue_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/logger_p.h
    ${GIT_API_SOURCE_FILES}
    ${INTERNAL_SOURCE_FILES}
)

set(SOURCE_FILES
    ${GIT_API_SOURCE_FILES}
    ${INTERNAL_SOURCE_FILES}
)

set(INTERNAL_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solver_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverforwardeuler_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime_p.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/unittestingexport.h
)

if(NOT EMSCRIPTEN)
    list(APPEND INTERNAL_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler_p.h
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler.h
    )
endif()

set(GIT_HEADER_FILES
    ${INTERNAL_HEADER_FILES}
    ${THIRD_PARTY_HEADER_FILES}
    ${VERSION_HEADER_FILE_IN}
)

set(HEADER_FILES
    ${INTERNAL_HEADER_FILES}
    ${VERSION_HEADER_FILE}
)

include_directories(.
                    file
                    logger
                    misc
                    support/cellml
                    support/combine
                    support/sedml
                    ${CMAKE_CURRENT_BINARY_DIR})

set(JAVASCRIPT_SOURCE_DIR tests/bindings/javascript)

if(EMSCRIPTEN)
    set(JAVASCRIPT_BINDINGS_DIR ${CMAKE_BINARY_DIR}/../../../../${JAVASCRIPT_SOURCE_DIR})

    get_filename_component(JAVASCRIPT_BINDINGS_DIR "${JAVASCRIPT_BINDINGS_DIR}" REALPATH)
else()
    set(JAVASCRIPT_BINDINGS_DIR ${CMAKE_BINARY_DIR}/${JAVASCRIPT_SOURCE_DIR})
endif()

if (EMSCRIPTEN)
    add_executable(${CMAKE_PROJECT_NAME}
                   ${API_HEADER_FILES}
                   ${SOURCE_FILES}
                   ${HEADER_FILES})

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
                          COMPILE_FLAGS "-O3"
                          LINK_FLAGS "-O3 -sALLOW_MEMORY_GROWTH -sEXPORT_ES6=1 -sEXPORTED_FUNCTIONS=[_malloc,_free] -sINLINING_LIMIT --bind")

    # Copy our generated .js and .wasm files over.

    foreach(FILE_EXTENSION js wasm)
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.${FILE_EXTENSION}
                                                            ${JAVASCRIPT_BINDINGS_DIR}/${CMAKE_PROJECT_NAME}.${FILE_EXTENSION})
    endforeach()

    # Let people know how to run our test web page.

    if(LIBOPENCOR_JAVASCRIPT_UNIT_TESTING)
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E echo "-- JavaScript test web page has been copied to: ${JAVASCRIPT_BINDINGS_DIR}/index.html"
                           COMMAND ${CMAKE_COMMAND} -E echo "                          and can be run using: emrun --browser chrome ${JAVASCRIPT_BINDINGS_DIR}/index.html")
    endif()
else()
    add_library(${CMAKE_PROJECT_NAME}
                ${API_HEADER_FILES}
                ${SOURCE_FILES}
                ${HEADER_FILES})

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
                          CXX_VISIBILITY_PRESET hidden
                          DEBUG_POSTFIX d
                          PREFIX ""
                          VERSION ${PROJECT_VERSION}
                          VISIBILITY_INLINES_HIDDEN ON)
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/api>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>)
target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module>
                           $<INSTALL_INTERFACE:include/>
                           $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME_LC}/module>)

# Configure libOpenCOR.

configure_target(${CMAKE_PROJECT_NAME})

# Generate the export header file, or don't use any if we are building our JavaScript bindings.

if(EMSCRIPTEN)
    configure_file(noexports.h ${API_EXPORT_HEADER_FILE} COPYONLY)
else()
    include(GenerateExportHeader)

    generate_export_header(${CMAKE_PROJECT_NAME}
                           EXPORT_FILE_NAME ${API_EXPORT_HEADER_FILE}
                           BASE_NAME LIBOPENCOR)
endif()

# Copy over our export header file for unit testing.

configure_file(unittestingexport.h unittestingexport.h COPYONLY)

# Don't allow unresolved symbols.
# Note: MSVC doesn't allow unresolved symbols, so we only need to look for them if we are building using GNU or Clang.

if(BUILDING_USING_GNU)
    SET(CMAKE_SHARED_LINKER_FLAGS -Wl,--no-undefined)
elseif(BUILDING_USING_CLANG)
    SET(CMAKE_SHARED_LINKER_FLAGS -Wl,-undefined,error)
endif()

# Get ready for code coverage.

if(LIBOPENCOR_CODE_COVERAGE)
    add_target_property(${CMAKE_PROJECT_NAME} COMPILE_FLAGS ${CODE_COVERAGE_COMPILER_FLAGS})
    add_target_property(${CMAKE_PROJECT_NAME} LINK_FLAGS ${CODE_COVERAGE_LINKER_FLAGS})
endif()

# Build our different bindings.

add_subdirectory(bindings)

list(APPEND GIT_SOURCE_FILES
    ${JAVASCRIPT_SOURCE_FILES}
    ${PYTHON_SOURCE_FILES}
)

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

set(CONFIG_CMAKE_IN_FILE ${PROJECT_SOURCE_DIR}/cmake/packaging/config.cmake.in)
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

configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/uninstall.cmake.in ${UNINSTALL_CMAKE_FILE} @ONLY)

add_target(uninstall
           COMMAND ${CMAKE_COMMAND} -P ${UNINSTALL_CMAKE_FILE}
           COMMENT "Uninstall the project...")

# Share some file information with the rest of our building system.

set(GIT_API_HEADER_FILES ${GIT_API_HEADER_FILES} PARENT_SCOPE)
set(GIT_API_MODULE_FILE ${GIT_API_MODULE_FILE} PARENT_SCOPE)
set(GIT_SOURCE_FILES ${GIT_SOURCE_FILES} PARENT_SCOPE)
set(GIT_HEADER_FILES ${GIT_HEADER_FILES} PARENT_SCOPE)
