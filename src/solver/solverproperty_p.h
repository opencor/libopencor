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

#include "logger_p.h"

#include "libopencor/solverproperty.h"

namespace libOpenCOR {

class SolverProperty::Impl
{
public:
    Type mType = Type::Double;
    std::string mName;
    SolverPropertyValue mValue = 0.0;
    std::vector<std::string> mListValues;
    SolverPropertyValue mDefaultValue = 0.0;
    bool mHasVoiUnit = false;

    explicit Impl(Type pType, const std::string &pName, const std::vector<std::string> &pListValues,
                  const SolverPropertyValue &pDefaultValue, bool pHasVoiValue);
};

} // namespace libOpenCOR
