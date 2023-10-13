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

#include "libopencor/solver.h"

namespace libOpenCOR {

class SolverNla: public Solver
{
public:
    using ComputeRates = void (*)(double pVoi, double *pStates, double *pRates, double *pVariables);

    SolverNla() = delete;
    ~SolverNla() override = default;

    SolverNla(const SolverNla &pOther) = delete;
    SolverNla(SolverNla &&pOther) noexcept = delete;

    SolverNla &operator=(const SolverNla &pRhs) = delete;
    SolverNla &operator=(SolverNla &&pRhs) noexcept = delete;

    virtual bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                            ComputeRates pComputeRates) = 0;
    virtual bool reinitialise(double pVoi);

    virtual bool solve(double &pVoi, double pVoiEnd) const = 0;

protected:
    class Impl;

    Impl *pimpl();
    const Impl *pimpl() const;

    explicit SolverNla(Impl *pPimpl);
};

} // namespace libOpenCOR
