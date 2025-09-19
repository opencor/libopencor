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

set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The backend library to OpenCOR")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/distrib/readme.txt")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_EXECUTABLES "${CMAKE_PROJECT_NAME}" "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Physiome Project")
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")

if(WIN32)
    # Select NSIS, if available, and ZIP as the packagers on Windows.

    if(   EXISTS "C:/Program Files/NSIS/makensis.exe"
       OR EXISTS "C:/Program Files (x86)/NSIS/makensis.exe")
        set(NSIS_FOUND TRUE)
        set(NSIS_GENERATOR NSIS)
    endif()

    set(CPACK_GENERATOR ${NSIS_GENERATOR} ZIP)
    set(CPACK_SYSTEM_NAME "-Windows")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

    # Specify the default installation directory.

    if(NSIS_FOUND)
        set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    endif()
elseif(APPLE)
    # Select productbuild and ZIP as the packagers on macOS.

    set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${CMAKE_PROJECT_NAME})

    set(CPACK_GENERATOR productbuild ZIP)
    set(CPACK_SYSTEM_NAME "-macOS")
    set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/distrib/package.cmake)

    set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/distrib/welcome.txt")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/distrib/readme.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

    set(CPACK_SET_DESTDIR TRUE)
else()
    # Select TGZ as the packager on Linux.

    set(CPACK_GENERATOR TGZ)
    set(CPACK_SYSTEM_NAME "-Linux")
endif()

# Specify the package file name.

if(${LIBOPENCOR_TARGET_ARCHITECTURE} STREQUAL "Intel")
    set(LIBRARY_ARCHITECTURE "-Intel")
else()
    set(LIBRARY_ARCHITECTURE "-ARM")
endif()

if(LIBOPENCOR_SHARED_LIBS)
    set(LIBRARY_TYPE "-Shared")
else()
    set(LIBRARY_TYPE "-Static")
endif()

if(WIN32)
    if(RELEASE_MODE)
        set(LIBRARY_MODE "-Release")
    else()
        set(LIBRARY_MODE "-Debug")
    endif()
endif()

set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}${CPACK_SYSTEM_NAME}${LIBRARY_ARCHITECTURE}${LIBRARY_TYPE}${LIBRARY_MODE})

# Package libOpenCOR.

include(CPack)
