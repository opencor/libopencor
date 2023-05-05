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
    # General documentation.

    build_documentation(general_documentation
                        general-documentation
                        .
                        -DMODE=${CMAKE_PROJECT_NAME})

    # User and developer documentations.

    foreach(DOCUMENTATION_NAME user developer)
        build_documentation(${DOCUMENTATION_NAME}_documentation
                            libopencor-${DOCUMENTATION_NAME}-documentation
                            ${DOCUMENTATION_NAME}
                            -DPYTHON_EXECUTABLE=${PYTHON_EXE}
                            -DSPHINX_EXECUTABLE=${SPHINX_EXE})
    endforeach()

    # API documentation.

    string(TIMESTAMP CURRENT_YEAR "%Y")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/footer.html.in ${CMAKE_CURRENT_BINARY_DIR}/footer.html)

    add_custom_target(api_documentation
                      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/doc/html/api/
                      COMMAND ${DOXYGEN_EXE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/doc/html/res/pics/twitter.png ${CMAKE_BINARY_DIR}/doc/html/api/twitter.png
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/doc/html/res/pics/vpr.png ${CMAKE_BINARY_DIR}/doc/html/api/vpr.png
                      COMMAND ${PATCH_EXE} ${CMAKE_BINARY_DIR}/doc/html/api/resize.js ${CMAKE_CURRENT_SOURCE_DIR}/resize.js.patch
                      DEPENDS general_documentation
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Generate our documentation.

    add_target(documentation
               COMMAND echo "The documentation can be found at ${CMAKE_BINARY_DIR}/doc/html/index.html."
               DEPENDS general_documentation
                       api_documentation
                       user_documentation
                       developer_documentation
               COMMENT "Generating the documentation...")
endif()
