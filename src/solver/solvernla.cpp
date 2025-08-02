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

#include "solvernla_p.h"

#include <sstream>

namespace libOpenCOR {

#ifdef __EMSCRIPTEN__
void nlaSolve(uintptr_t pNlaSolverAddress, size_t pObjectiveFunctionIndex, double *pU, size_t pN, void *pData)
{
    reinterpret_cast<SolverNla *>(pNlaSolverAddress)->solve(pObjectiveFunctionIndex, pU, pN, pData);
}
#else
void nlaSolve(uintptr_t pNlaSolverAddress, void (*pObjectiveFunction)(double *, double *, void *),
              double *pU, size_t pN, void *pData)
{
    reinterpret_cast<SolverNla *>(pNlaSolverAddress)->solve(pObjectiveFunction, pU, pN, pData); // NOLINT
}
#endif

SolverNla::Impl::Impl(const std::string &pId, const std::string &pName)
    : Solver::Impl(pId, pName)
{
}

SolverNla::SolverNla(Impl *pPimpl)
    : Solver(pPimpl)
{
}

SolverNla::Impl *SolverNla::pimpl()
{
    return static_cast<Impl *>(Solver::pimpl());
}

const SolverNla::Impl *SolverNla::pimpl() const
{
    return static_cast<const Impl *>(Solver::pimpl());
}

Solver::Type SolverNla::type() const
{
    return Type::NLA;
}

#ifdef __EMSCRIPTEN__
bool SolverNla::solve(size_t pComputeObjectiveFunctionIndex, double *pU, size_t pN, void *pUserData)
{
    return pimpl()->solve(pComputeObjectiveFunctionIndex, pU, pN, pUserData);
}
#else
bool SolverNla::solve(ComputeObjectiveFunction pComputeObjectiveFunction, double *pU, size_t pN, void *pUserData)
{
    return pimpl()->solve(pComputeObjectiveFunction, pU, pN, pUserData);
}
#endif

} // namespace libOpenCOR
