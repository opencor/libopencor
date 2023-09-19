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

#include "libopencor/solverode.h"

namespace libOpenCOR {

class LIBOPENCOR_EXPORT SolverUnknown: public SolverOde
{
    friend class Solver;

public:
    ~SolverUnknown() override;

    SolverUnknown(const SolverUnknown &pOther) = delete;
    SolverUnknown(SolverUnknown &&pOther) noexcept = delete;

    SolverUnknown &operator=(const SolverUnknown &pRhs) = delete;
    SolverUnknown &operator=(SolverUnknown &&pRhs) noexcept = delete;

    bool initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;
    void solve(double &pVoi, double pVoiEnd) const override;

private:
    class Impl;

    explicit SolverUnknown();

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR
