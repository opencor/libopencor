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

# Build our different third-party libraries as external projects and statically
# link them to libOpenCOR.

include(packages)

# Prerequisite third-party libraries, respecting their own prerequisites.

add_subdirectory(libxml2)
add_subdirectory(OpenSSL)
add_subdirectory(zlib)

add_subdirectory(libSBML) # Requires libxml2.

add_subdirectory(libNuML) # Requires libSBML and libxml2.

add_subdirectory(zipper) # Requires zlib.

# Required third-party libraries.

add_subdirectory(libCellML) # Requires libxml2 and zlib.
add_subdirectory(libcurl) # Requires OpenSSL.
add_subdirectory(libCOMBINE) # Requires libSBML, libxml2, zipper and zlib.
add_subdirectory(libSEDML) # Requires libNuML, libSBML and libxml2.
