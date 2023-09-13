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

SolverInfoPtr SolverInfo::Impl::create(Solver::Type pType, const std::tuple<std::string, std::string> &pName,
                                       const std::vector<SolverPropertyPtr> &pProperties)
{
    return SolverInfoPtr {new SolverInfo {pType, pName, pProperties}};
}

SolverInfo::Impl::Impl(Solver::Type pType, const std::tuple<std::string, std::string> &pName,
                       const std::vector<SolverPropertyPtr> &pProperties)
    : mType(pType)
    , mName(pName)
    , mProperties(pProperties)
{
}

SolverInfo::SolverInfo(Solver::Type pType, const std::tuple<std::string, std::string> &pName,
                       const std::vector<SolverPropertyPtr> &pProperties)
    : mPimpl(new Impl(pType, pName, pProperties))
{
}

#ifndef COVERAGE_ENABLED
SolverInfo::~SolverInfo()
{
    delete mPimpl;
}
#endif

Solver::Type SolverInfo::type() const
{
    return mPimpl->mType;
}

std::tuple<std::string, std::string> SolverInfo::name() const
{
    return mPimpl->mName;
}

std::vector<SolverPropertyPtr> SolverInfo::properties() const
{
    return mPimpl->mProperties;
}

} // namespace libOpenCOR
