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

LIBOPENCOR_EXPORT unsigned int version();

/**
 * Return the version of libOpenCOR as a string. The version string is in the
 * format x.y.z, where the "."s are literal, and x,y and z represent counting
 * numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libOpenCOR.
 */

LIBOPENCOR_EXPORT std::string versionString();

/**
 * Return the version of libCellML as a number.
 *
 * @return A number that represents the version of libCellML.
 */

LIBOPENCOR_EXPORT unsigned int libcellmlVersion();

/**
 * Return the version of libCellML as a string. The version string is in the
 * format x.y.z, where the "."s are literal, and x,y and z represent counting
 * numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libCellML.
 */

LIBOPENCOR_EXPORT std::string libcellmlVersionString();

/**
 * Return the version of libCOMBINE as a number.
 *
 * @return A number that represents the version of libCOMBINE.
 */

LIBOPENCOR_EXPORT unsigned int libcombineVersion();

/**
 * Return the version of libCOMBINE as a string. The version string is in the
 * format x.y.z, where the "."s are literal, and x,y and z represent counting
 * numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libCOMBINE.
 */

LIBOPENCOR_EXPORT std::string libcombineVersionString();

/**
 * Return the version of libSEDML as a number.
 *
 * @return A number that represents the version of libSEDML.
 */

LIBOPENCOR_EXPORT unsigned int libsedmlVersion();

/**
 * Return the version of libSEDML as a string. The version string is in the
 * format x.y.z, where the "."s are literal, and x,y and z represent counting
 * numbers, in which case x is the major version, y the minor version, and z the
 * patch level.
 *
 * @return A string that represents the version of libSEDML.
 */

LIBOPENCOR_EXPORT std::string libsedmlVersionString();

} // namespace libOpenCOR
