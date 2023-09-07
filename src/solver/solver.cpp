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
#include "solverinfo_p.h"

#include "libopencor/solverforwardeuler.h"

#include <cassert>

namespace libOpenCOR {

std::map<std::string, SolverInfoPtr> Solver::Impl::sSolversInfo; // NOLINT

bool Solver::Impl::registerSolver(Type pType, const std::string &pName, const std::vector<SolverPropertyPtr> &pProperties)
{
    auto res = true;

    if (auto iter = sSolversInfo.find(pName); iter == sSolversInfo.end()) {
        sSolversInfo[pName] = SolverInfo::Impl::create(pType, pName, pProperties);
    } else {
        res = false;
    }

    assert(res == true);

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

} // namespace libOpenCOR
