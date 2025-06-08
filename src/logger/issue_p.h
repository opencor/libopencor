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

#include "libopencor/issue.h"

namespace libOpenCOR {

class Issue::Impl
{
public:
    Type mType;
    std::string mDescription;

    explicit Impl(Type pType, const std::string &pDescription);

    Type type() const;
    std::string typeAsString() const;
    std::string description() const;
};

} // namespace libOpenCOR
