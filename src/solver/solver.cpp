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
#include "solverforwardeuler.h"
#include "solverinfo_p.h"

#include "libopencor/solverproperty.h"

#include <cassert>

namespace libOpenCOR {

static std::map<std::string, SolverInfoPtr> sSolversInfo; // NOLINT
static std::map<std::string, SolverCreate> sSolversCreate; // NOLINT

bool Solver::Impl::registerSolver(Type pType, const std::string &pName, SolverCreate pCreate,
                                  const std::vector<SolverPropertyPtr> &pProperties)
{
    auto res = true;

#ifndef COVERAGE_ENABLED
    if (auto iter = sSolversInfo.find(pName); iter == sSolversInfo.end()) {
#endif
        sSolversInfo[pName] = SolverInfo::Impl::create(pType, pName, pProperties);
        sSolversCreate[pName] = pCreate;
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
    return std::shared_ptr<SolverProperty> {new SolverProperty(pType, pName, pListValues, pDefaultValue, pHasVoiValue)};
}

Solver::Solver(Impl *pPimpl)
    : Logger(pPimpl)
{
}

Solver::Impl *Solver::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const Solver::Impl *Solver::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

SolverPtr Solver::create(const std::string &pNameOrKisaoId)
{
    if (auto iter = sSolversCreate.find(pNameOrKisaoId); iter != sSolversCreate.end()) {
        return iter->second();
    }

    return {};
}

std::vector<SolverInfoPtr> Solver::solversInfo()
{
    std::vector<SolverInfoPtr> res;

    res.reserve(sSolversInfo.size());

    for (const auto &solverInfo : sSolversInfo) {
        res.push_back(solverInfo.second);
    }

    return res;
}

} // namespace libOpenCOR
