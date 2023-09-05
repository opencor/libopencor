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

#include "solver_p.h"

#include "libopencor/solverforwardeuler.h"

namespace libOpenCOR {

Solver::Impl::Impl(Solver::Type pType, Solver::Method pMethod, const std::string &pName,
                   const std::vector<SolverPropertyPtr> &pProperties)
    : Logger::Impl()
    , mType(pType)
    , mMethod(pMethod)
    , mName(pName)
    , mProperties(pProperties)
{
}

std::vector<std::string> Solver::Impl::properties() const
{
    std::vector<std::string> res;

    res.reserve(mProperties.size());

    for (const auto &property : mProperties) {
        res.push_back(property->name());
    }

    return res;
}

Solver::Solver(Impl *pPimpl)
    : Logger(pPimpl)
{
}

Solver::Impl *Solver::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

const Solver::Impl *Solver::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}

SolverPtr Solver::create(Method pMethod)
{
    //---GRY---
    (void)pMethod;
    return std::shared_ptr<Solver> {new SolverForwardEuler {}};
}

Solver::Type Solver::type() const
{
    return pimpl()->mType;
}

Solver::Method Solver::method() const
{
    return pimpl()->mMethod;
}

std::string Solver::name() const
{
    return pimpl()->mName;
}

std::vector<std::string> Solver::properties() const
{
    return pimpl()->properties();
}

} // namespace libOpenCOR
