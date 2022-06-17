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

#include "utils.h"

#include <cmath>

namespace libOpenCOR {

bool fuzzyCompare(double pNb1, double pNb2)
{
    static constexpr double ONE_TRILLION = 1000000000000.0;

    return fabs(pNb1 - pNb2) * ONE_TRILLION <= fmin(fabs(pNb1), fabs(pNb2));
}

} // namespace libOpenCOR
