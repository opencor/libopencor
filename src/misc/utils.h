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

#include <memory>
#include <string>

namespace libOpenCOR {

/*---GRY--- THE BELOW METHOD IS TO BE ENABLED ONCE WE ACTUALLY MAKE USE OF IT IN libOpenCOR.
bool fuzzyCompare(double pNb1, double pNb2);
*/

std::string uniqueFileName();

std::string downloadFile(const std::string &pUrl);

std::tuple<std::shared_ptr<char[]>, size_t> fileContents(const std::string &pFileName);

} // namespace libOpenCOR
