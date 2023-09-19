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

#include "solverforwardeuler.h"
#include "solverode_p.h"

namespace libOpenCOR {

class SolverForwardEuler::Impl: public SolverOde::Impl
{
public:
    static const Solver::Type Type = Solver::Type::ODE;
    static const std::string Name;
    static const std::string KisaoId;

    static const std::string StepName;
    static const std::string StepKisaoId;
    static constexpr double StepDefaultValue = 1.0;

    static SolverOdePtr create();
    static std::vector<SolverPropertyPtr> propertiesInfo();

    double mStep = StepDefaultValue;

    explicit Impl();

    std::map<std::string, std::string> propertiesKisaoId() const override;

    bool initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                    SolverOde::ComputeRates pComputeRates) override;
    void solve(double &pVoi, double pVoiEnd) const override;
};

} // namespace libOpenCOR
