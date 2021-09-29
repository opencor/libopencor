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

if(LIBOPENCOR_DOCUMENTATION)
    include(ExternalProject)

    set(EXTERNAL_PROJECT_BUILD_DIR ${CMAKE_BINARY_DIR}/ext)

    set_property(DIRECTORY PROPERTY EP_BASE ${EXTERNAL_PROJECT_BUILD_DIR})

    ExternalProject_Add(GeneralDocumentationBuild
        SOURCE_DIR
            ${EXTERNAL_PROJECT_BUILD_DIR}/doc/general
        GIT_REPOSITORY
            https://github.com/opencor/general-documentation
        CMAKE_ARGS
            -DMODE=${CMAKE_PROJECT_NAME}
        INSTALL_COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_PROJECT_BUILD_DIR}/Build/GeneralDocumentationBuild/html
                                               ${CMAKE_BINARY_DIR}/doc/html
    )
endif()
