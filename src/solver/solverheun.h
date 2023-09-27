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

#include "solverode.h"

namespace libOpenCOR {

class SolverHeun: public SolverOde
{
    friend class Solver;

public:
    ~SolverHeun() override;

    SolverHeun(const SolverHeun &pOther) = delete;
    SolverHeun(SolverHeun &&pOther) noexcept = delete;

    SolverHeun &operator=(const SolverHeun &pRhs) = delete;
    SolverHeun &operator=(SolverHeun &&pRhs) noexcept = delete;

    bool initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;
    bool solve(double &pVoi, double pVoiEnd) const override;

private:
    class Impl;

    Impl *pimpl();
    const Impl *pimpl() const;

    explicit SolverHeun();
};

} // namespace libOpenCOR