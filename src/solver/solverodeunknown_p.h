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
#include "solverodeunknown.h"

namespace libOpenCOR {

class SolverOdeUnknown::Impl: public SolverOde::Impl
{
public:
    static SolverOdePtr create();

    std::map<std::string, std::string> propertiesKisaoId() const override;

    bool initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                    SolverOde::ComputeRates pComputeRates) override;
    bool solve(double pVoiFrom, double pVoiTo) const override;
};

} // namespace libOpenCOR
