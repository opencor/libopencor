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

namespace libOpenCOR {

// clang-format off
static constexpr auto LIBOPENCOR_VERSION {@LIBOPENCOR_VERSION@};
// clang-format on
static constexpr auto LIBOPENCOR_VERSION_STRING {"@LIBOPENCOR_VERSION_STRING@"};

unsigned int firstDigit(unsigned int pTwoDigitNumber);
unsigned int secondDigit(unsigned int pTwoDigitNumber);

} // namespace libOpenCOR
