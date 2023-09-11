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
#include "solverforwardeuler_p.h"
#include "solverinfo_p.h"
#include "solverunknown_p.h"

#include <cassert>

namespace libOpenCOR {

std::map<std::string, SolverCreate> Solver::Impl::sSolversCreate; // NOLINT
std::vector<SolverInfoPtr> Solver::Impl::sSolversInfo; // NOLINT

bool Solver::Impl::registerSolver(Type pType, const std::string &pName, SolverCreate pCreate,
                                  const std::vector<SolverPropertyPtr> &pProperties)
{
//---GRY--- ALSO HAVE A KISAO ID BASED ENTRY IN sSolversCreate...
    auto res = true;

#ifndef COVERAGE_ENABLED
    if (auto iter = Solver::Impl::sSolversCreate.find(pName); iter == Solver::Impl::sSolversCreate.end()) {
#endif
        Solver::Impl::sSolversCreate[pName] = pCreate;
        Solver::Impl::sSolversInfo.push_back(SolverInfo::Impl::create(pType, pName, pProperties));
#ifndef COVERAGE_ENABLED
    } else {
        res = false;
    }

    assert(res == true);
#endif

    return res;
}

SolverPropertyPtr Solver::Impl::createProperty(SolverProperty::Type pType, const std::string &pName,
                                               const std::vector<std::string> &pListValues,
                                               const SolverPropertyValue &pDefaultValue, bool pHasVoiValue)
{
    return SolverPropertyPtr {new SolverProperty(pType, pName, pListValues, pDefaultValue, pHasVoiValue)};
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

SolverPtr Solver::create(const std::string &pNameOrKisaoId)
{
    if (auto iter = Solver::Impl::sSolversCreate.find(pNameOrKisaoId); iter != Solver::Impl::sSolversCreate.end()) {
        return iter->second();
    }

    return SolverUnknown::Impl::create();
}

std::vector<SolverInfoPtr> Solver::solversInfo()
{
    static auto initialised = false;

    if (!initialised) {
        initialised = true;

        Solver::Impl::registerSolver(Solver::Type::ODE, "Forward Euler", SolverForwardEuler::Impl::create,
                                     {Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, "Step", {}, 1.0, true)});
    }

    return Solver::Impl::sSolversInfo;
}

bool Solver::isValid() const
{
    return pimpl()->mIsValid;
}

} // namespace libOpenCOR
