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

#include "tests/utils.h"

#include <cstring>

namespace libOpenCOR {

std::string resourcePath(const std::string &resourceRelativePath)
{
    return std::string(RESOURCE_LOCATION) + "/" + resourceRelativePath;
}

#ifdef BUILDING_USING_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
std::vector<unsigned char> charArrayToVector(const char *contents)
#ifdef BUILDING_USING_CLANG
#    pragma clang diagnostic pop
#endif
{
    return {contents, contents + strlen(contents)}; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

} // namespace libOpenCOR
