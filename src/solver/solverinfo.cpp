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

#include "solverinfo_p.h"

namespace libOpenCOR {

SolverInfoPtr SolverInfo::Impl::create(const SolverPropertyPtrVector &pProperties, HiddenProperties pHiddenProperties)
{
    return SolverInfoPtr {new SolverInfo {pProperties, pHiddenProperties}};
}

SolverInfo::Impl::Impl(const SolverPropertyPtrVector &pProperties, HiddenProperties pHiddenProperties)
    : mProperties(pProperties)
    , mHiddenProperties(pHiddenProperties)
{
}

SolverInfo::SolverInfo(const SolverPropertyPtrVector &pProperties, HiddenProperties pHiddenProperties)
    : mPimpl(new Impl(pProperties, pHiddenProperties))
{
}

#ifndef CODE_COVERAGE_ENABLED
SolverInfo::~SolverInfo()
{
    delete mPimpl;
}
#endif

SolverPropertyPtrVector SolverInfo::properties() const
{
    return mPimpl->mProperties;
}

StringVector SolverInfo::hiddenProperties(const StringStringMap &pProperties) const
{
    return mPimpl->mHiddenProperties(pProperties);
}

} // namespace libOpenCOR
