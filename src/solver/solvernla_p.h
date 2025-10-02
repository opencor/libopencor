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

#pragma once

#include "solver_p.h"

#include "libopencor/solvernla.h"

namespace libOpenCOR {

#ifdef __EMSCRIPTEN__
void nlaSolve(uintptr_t pNlaSolverAddress, intptr_t pWasmInstanceFunctionsId, size_t pObjectiveFunctionIndex, double *pU, size_t pN, void *pData);
#else
void nlaSolve(uintptr_t pNlaSolverAddress, void (*pObjectiveFunction)(double *, double *, void *),
              double *pU, size_t pN, void *pData);
#endif

class SolverNla::Impl: public Solver::Impl
{
public:
    explicit Impl(const std::string &pId, const std::string &pName);

#ifdef __EMSCRIPTEN__
    virtual bool solve(intptr_t pWasmInstanceFunctionsId, size_t pComputeObjectiveFunctionIndex, double *pU, size_t pN, void *pUserData) = 0;
#else
    virtual bool solve(ComputeObjectiveFunction pComputeObjectiveFunction, double *pU, size_t pN, void *pUserData) = 0;
#endif
};

} // namespace libOpenCOR
