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

set(PACKAGE_NAME zlib)
set(PACKAGE_VERSION 1.2.11)
set(PACKAGE_REPOSITORY zlib)
set(GIT_TAG v1.2.11-libopencor)
set(RELEASE_TAG v1.2.11-libopencor)
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_NAME})

if(WIN32)
    if(RELEASE_MODE)
        set(DEBUG_TAG)
    else()
        set(DEBUG_TAG d)
    endif()

    set(STATIC_LIBRARY lib/zlib${DEBUG_TAG}${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(STATIC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(SHA1_FILES
    ${STATIC_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_ZLIB)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             c4be57eacfb7b16300032b1899ed7e58140ce839
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES c4151de005db5a70d308f37ff539779f50982a38)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             8d21ae751d6d1e049c80dd73a729636461651f6a
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES a7f3f70e5469c2262a110a1d79d3052f9b681abb)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         48fe846a4d708e208eaf7f992d2596ad82cf863d
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 82c44538bc862239311fd4da5ed795dd62dfe681)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         342d1bbd140f7b5f0a25fef0eea19c0fa758cb79
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES d8031204aefc9e5bcfe6669610ff143cfdd9a986)
    endif()
else()
    if(WIN32)
        set(PACKAGE_C_FLAGS "${CMAKE_C_FLAGS} /wd4267")
    endif()

    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        CMAKE_CACHE_ARGS
            -DCMAKE_C_FLAGS:STRING=${PACKAGE_C_FLAGS}
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${STATIC_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let other third-party libraries find and use our package.

set(ZLIB_LIBRARY ${INSTALL_DIR}/${STATIC_LIBRARY} PARENT_SCOPE)
