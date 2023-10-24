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

namespace libOpenCOR {

//---GRY--- REMOVE NOLINT ONCE nlaSolver() IS FULLY IMPLEMENTED.
void nlaSolve(SolverNla *pSolver, void (*pObjectiveFunction)(double *, double *, void *),
              double *pU, int pN, void *pData) // NOLINT
{
    // Retrieve the NLA solver with which we should solve our NLA system.

    (void)pObjectiveFunction;
    (void)pU;
    (void)pN;
    (void)pData;

    printf(">>> pSolver: %p\n", static_cast<void *>(pSolver)); // NOLINT

    // OpenCOR::Solver::NlaSolver *nlaSolver = OpenCOR::Solver::nlaSolver(pRuntime);

    // ASSERT_NE(nlaSolver, nullptr);

    // nlaSolver->solve(pFunction, pParameters, pSize, pUserData);
}

bool SolverNla::Impl::initialise(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    mComputeSystem = pComputeSystem;

    mU = pU;
    mN = pN;

    mUserData = pUserData;

    return true;
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

} // namespace libOpenCOR
