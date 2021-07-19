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
set(LIBOPENCOR_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

configure_file(version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

# Build libOpenCOR.

set(EXPORT_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/api/libopencor/export.h)

set(SOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)

set(API_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/api/libopencor/version.h
)

set(API_MODULE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/api/libopencor/module/libopencor)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/api
    api
    api/libopencor/module
)

add_library(${CMAKE_PROJECT_NAME}
    ${SOURCE_FILES}
    ${API_HEADER_FILES}
    ${API_MODULE_FILE}
    ${EXPORT_HEADER_FILE}
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX d
    PREFIX ""
    VERSION ${PROJECT_VERSION}
)

# Generate the export header file.

generate_export_header(${CMAKE_PROJECT_NAME}
    EXPORT_FILE_NAME ${EXPORT_HEADER_FILE}
    BASE_NAME LIBOPENCOR
)

# Use Clang-Tidy.

if(LIBOPENCOR_CLANG_TIDY)
    configure_clang_and_clang_tidy(${CMAKE_PROJECT_NAME})
endif()

# Build our different bindings.

add_subdirectory(bindings)
