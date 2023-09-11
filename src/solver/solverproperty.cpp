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

#include "solverproperty_p.h"

namespace libOpenCOR {

SolverProperty::Impl::Impl(Type pType, const std::string &pName, const std::vector<std::string> &pListValues,
                           const std::string &pDefaultValue, bool pHasVoiValue)
    : mType(pType)
    , mName(pName)
    , mListValues(pListValues)
    , mDefaultValue(pDefaultValue)
    , mHasVoiUnit(pHasVoiValue)
{
}

SolverProperty::SolverProperty(Type pType, const std::string &pName, const std::vector<std::string> &pListValues,
                               const std::string &pDefaultValue, bool pHasVoiValue)
    : mPimpl(new Impl(pType, pName, pListValues, pDefaultValue, pHasVoiValue))
{
}

SolverProperty::~SolverProperty()
{
    delete mPimpl;
}

SolverProperty::Type SolverProperty::type() const
{
    return mPimpl->mType;
}

std::string SolverProperty::name() const
{
    return mPimpl->mName;
}

std::vector<std::string> SolverProperty::listValues() const
{
    return mPimpl->mListValues;
}

std::string SolverProperty::defaultValue() const
{
    return mPimpl->mDefaultValue;
}

bool SolverProperty::hasVoiUnit() const
{
    return mPimpl->mHasVoiUnit;
}

} // namespace libOpenCOR
