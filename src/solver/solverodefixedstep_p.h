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

#include "solverode_p.h"

#include "libopencor/solverodefixedstep.h"

namespace libOpenCOR {

class SolverOdeFixedStep::Impl: public SolverOde::Impl
{
public:
    using SolverOde::Impl::duplicate;

    double mStep = 1.0;

    explicit Impl(const std::string &pId, const std::string &pName);

    SolverPtr duplicate(const SolverOdeFixedStepPtr &pSolver);

    StringStringMap properties() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;
};

} // namespace libOpenCOR
