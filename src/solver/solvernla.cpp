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

namespace {
thread_local uintptr_t sNlaSolverAddress = 0; // NOLINT
thread_local bool sNlaSolveFailed = false; // NOLINT
} // namespace

void nlaSolve(uintptr_t pNlaSolverAddress, void (*pObjectiveFunction)(double *, double *, void *),
              double *pU, size_t pN, void *pData) noexcept
{
    // Solve the given NLA system, unless an NLA system could not be solved since resetNlaSolveFailed() was last called.
    // Indeed, our model would then be computed using some wrong values anyway and the NLA solver would remove the
    // issues that explain why an NLA system could not be solved (since it removes its issues each time it is used),
    // should it then successfully solve another (or the same) NLA system.
    // Note: this function is called from our compiled code, so it must not let an exception escape. Indeed, unwinding
    //       through the frames of our compiled code is not supported (and, on Windows, it terminates the process).

    if (!sNlaSolveFailed) {
        sNlaSolveFailed = !reinterpret_cast<SolverNla *>(pNlaSolverAddress)->solve(pObjectiveFunction, pU, pN, pData); // NOLINT
    }
}

bool nlaSolveFailed()
{
    return sNlaSolveFailed;
}

void resetNlaSolveFailed()
{
    sNlaSolveFailed = false;
}

extern "C" uintptr_t nlaSolverAddress()
{
    return sNlaSolverAddress;
}

void setNlaSolverAddress(uintptr_t pAddress)
{
    sNlaSolverAddress = pAddress;
}

SolverNla::Impl::Impl(const std::string &pId, const std::string &pName)
    : Solver::Impl(pId, pName)
{
}

SolverNla::SolverNla(std::unique_ptr<Impl> pPimpl)
    : Solver(std::move(pPimpl))
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

Solver::Type SolverNla::type() const noexcept
{
    return Type::NLA;
}

bool SolverNla::solve(ComputeObjectiveFunction pComputeObjectiveFunction, double *pU, size_t pN, void *pUserData)
{
    return pimpl()->solve(pComputeObjectiveFunction, pU, pN, pUserData);
}

} // namespace libOpenCOR
