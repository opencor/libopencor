/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "libopencor/export.h"

#include <string>

/**
 * Some functions to retrieve the version of libOpenCOR.
 */

namespace libOpenCOR {

/**
 * Return the version of libOpenCOR as a number.
 *
 * @return A number that represents the version of libOpenCOR.
 */

unsigned int LIBOPENCOR_EXPORT version();

/**
 * Return the version of libOpenCOR as a string. The version string is in the format x.y.z, where the "."s are literal,
 * and x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libOpenCOR.
 */

std::string LIBOPENCOR_EXPORT versionString();

/**
 * Return the version of Clang as a number.
 *
 * @return A number that represents the version of Clang.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// unsigned int LIBOPENCOR_EXPORT clangVersion();

/**
 * Return the version of Clang as a string. The version string is in the format x.y.z, where the "."s are literal, and
 * x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the patch
 * level.
 *
 * @return A string that represents the version of Clang.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// std::string LIBOPENCOR_EXPORT clangVersionString();

/**
 * Return the version of libCellML as a number.
 *
 * @return A number that represents the version of libCellML.
 */

unsigned int LIBOPENCOR_EXPORT libcellmlVersion();

/**
 * Return the version of libCellML as a string. The version string is in the format x.y.z, where the "."s are literal,
 * and x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libCellML.
 */

std::string LIBOPENCOR_EXPORT libcellmlVersionString();

/**
 * Return the version of libCOMBINE as a number.
 *
 * @return A number that represents the version of libCOMBINE.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// unsigned int LIBOPENCOR_EXPORT libcombineVersion();

/**
 * Return the version of libCOMBINE as a string. The version string is in the format x.y.z, where the "."s are literal,
 * and x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libCOMBINE.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// std::string LIBOPENCOR_EXPORT libcombineVersionString();

/**
 * Return the version of libcurl as a number.
 *
 * @return A number that represents the version of libcurl.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// unsigned int LIBOPENCOR_EXPORT libcurlVersion();

/**
 * Return the version of libcurl as a string. The version string is in the format x.y.z, where the "."s are literal,
 * and x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libcurl.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// std::string LIBOPENCOR_EXPORT libcurlVersionString();

/**
 * Return the version of libSEDML as a number.
 *
 * @return A number that represents the version of libSEDML.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// unsigned int LIBOPENCOR_EXPORT libsedmlVersion();

/**
 * Return the version of libSEDML as a string. The version string is in the format x.y.z, where the "."s are literal,
 * and x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libSEDML.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// std::string LIBOPENCOR_EXPORT libsedmlVersionString();

/**
 * Return the version of LLVM as a number.
 *
 * @return A number that represents the version of LLVM.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// unsigned int LIBOPENCOR_EXPORT llvmVersion();

/**
 * Return the version of LLVM as a string. The version string is in the format x.y.z, where the "."s are literal, and
 * x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the patch
 * level.
 *
 * @return A string that represents the version of LLVM.
 */

//---GRY--- BUILDING_JAVASCRIPT_BINDINGS.
// std::string LIBOPENCOR_EXPORT llvmVersionString();

/**
 * Return the version of SUNDIALS as a number.
 *
 * @return A number that represents the version of SUNDIALS.
 */

unsigned int LIBOPENCOR_EXPORT sundialsVersion();

/**
 * Return the version of SUNDIALS as a string. The version string is in the format x.y.z, where the "."s are literal,
 * and x, y, and z represent counting numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of SUNDIALS.
 */

std::string LIBOPENCOR_EXPORT sundialsVersionString();

} // namespace libOpenCOR
