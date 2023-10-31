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

#include "libopencor/solverforwardeuler.h"

namespace libOpenCOR {

class SolverForwardEuler::Impl: public SolverOde::Impl
{
public:
    // Properties information.

    static const Solver::Type TYPE = Solver::Type::ODE;
    static const std::string ID;
    static const std::string NAME;

    static const std::string STEP_ID;
    static const std::string STEP_NAME;
    static constexpr double STEP_DEFAULT_VALUE = 1.0;

    // Solver.

    static SolverPtr create();
    static SolverPropertyPtrVector propertiesInfo();

    double mStep = STEP_DEFAULT_VALUE;

    explicit Impl();

    StringStringMap propertiesId() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;

    bool solve(double &pVoi, double pVoiEnd) const override;
};

} // namespace libOpenCOR
