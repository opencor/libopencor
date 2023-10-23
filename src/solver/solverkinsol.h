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

#include "solvernla.h"

namespace libOpenCOR {

class SolverKinsol: public SolverNla
{
    friend class Solver;

public:
    ~SolverKinsol() override;

    SolverKinsol(const SolverKinsol &pOther) = delete;
    SolverKinsol(SolverKinsol &&pOther) noexcept = delete;

    SolverKinsol &operator=(const SolverKinsol &pRhs) = delete;
    SolverKinsol &operator=(SolverKinsol &&pRhs) noexcept = delete;

    SolverInfoPtr info() const override;

    bool initialise(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData) override;

    bool solve() override;

private:
    class Impl;

    Impl *pimpl();
    const Impl *pimpl() const;

    explicit SolverKinsol();
};

} // namespace libOpenCOR