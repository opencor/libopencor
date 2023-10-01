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

#include "unittestingexport.h"

#include <filesystem>
#include <string>
#include <vector>

namespace libOpenCOR {

bool LIBOPENCOR_UNIT_TESTING_EXPORT fuzzyCompare(double pNb1, double pNb2);

#ifdef BUILDING_USING_MSVC
std::string wideStringToString(const std::wstring &pString);

std::string forwardSlashPath(const std::string &pPath);
#endif

std::filesystem::path stringToPath(const std::string &pString);

std::tuple<bool, std::string> retrieveFileInfo(const std::string &pFileNameOrUrl);

#ifndef __EMSCRIPTEN__
std::tuple<bool, std::filesystem::path> downloadFile(const std::string &pUrl);

std::vector<unsigned char> fileContents(const std::filesystem::path &pFilePath);
#endif

std::string LIBOPENCOR_UNIT_TESTING_EXPORT toString(bool pBoolean);
std::string toString(int pNumber);
std::string toString(size_t pNumber);
std::string toString(double pNumber);
std::string toString(const std::vector<unsigned char> &pBytes);

bool toBool(const std::string &pString, bool &pOk);
int LIBOPENCOR_UNIT_TESTING_EXPORT toInt(const std::string &pString, bool &pOk);
size_t LIBOPENCOR_UNIT_TESTING_EXPORT toSizeT(const std::string &pString, bool &pOk);
double LIBOPENCOR_UNIT_TESTING_EXPORT toDouble(const std::string &pString, bool &pOk);

} // namespace libOpenCOR
