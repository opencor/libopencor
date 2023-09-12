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

class LIBOPENCOR_EXPORT SolverForwardEuler: public Solver
{
    friend class Solver;

public:
    static const std::string StepId;
    static constexpr double StepDefaultValue = 1.0;

    ~SolverForwardEuler() override;

    SolverForwardEuler(const SolverForwardEuler &pOther) = delete;
    SolverForwardEuler(SolverForwardEuler &&pOther) noexcept = delete;

    SolverForwardEuler &operator=(const SolverForwardEuler &pRhs) = delete;
    SolverForwardEuler &operator=(SolverForwardEuler &&pRhs) noexcept = delete;

private:
    class Impl;

    double mStep = StepDefaultValue;

    explicit SolverForwardEuler();

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR
